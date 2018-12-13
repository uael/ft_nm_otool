/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/climb_tree.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** glob_climb_tree - glob util function to check recursively a directory tree
*/

#include "internal.h"

static int	glob_just_copy_pattern(t_glob_env *e)
{
	if (e->match_list)
		return (FT_GLOB_SUCCESS);
	if (!(*(e->flags) & FT_GLOB_NOCHECK))
		return (FT_GLOB_NOMATCH);
	*(e->flags) |= FT_GLOB_NOMATCH_FLAG;
	return (matchctoradd(e->pattern, true, \
						!(*(e->flags) & FT_GLOB_NOSORT), &e->match_list));
}

static int	glob_pre_check_file(t_glob_env *e, struct dirent *d)
{
	if ((*(e->flags) & FT_GLOB_ONLYDIR) && !IS_DIR(d))
		return (FT_GLOB_SUCCESS);
	if (!glob_match(e->sub_pat_buf, d->d_name, *(e->flags)))
		return (FT_GLOB_SUCCESS);
	if (*(d->d_name) == '.' && !show_hidden_files(*(e->flags), *e->sub_pat_buf))
		return (FT_GLOB_SUCCESS);
	return (42);
}

static int	glob_check_file(t_glob_env *e, struct dirent *d, \
							int depth, char *path_buf)
{
	size_t		len;

	if (!glob_pre_check_file(e, d))
		return (FT_GLOB_SUCCESS);
	if (depth == 1)
	{
		if (glob_append_file_name(path_buf, d->d_name) != FT_GLOB_SUCCESS)
			return (FT_GLOB_NOSPACE);
		if (!ft_memcmp(path_buf, "./", 2) && ft_memcmp(e->pattern, "./", 2))
			path_buf += 2;
		len = ft_strlen(path_buf);
		if ((*(e->flags) & (FT_GLOB_MARK | FT_GLOB_ONLYDIR)) \
				&& len && *(path_buf + len - 1) != '/')
			ft_memcpy(path_buf + len, "/", 2);
		if (matchctoradd(path_buf, false, \
						!(*(e->flags) & FT_GLOB_NOSORT), &e->match_list))
			return (FT_GLOB_NOSPACE);
		if (*(e->flags) & (FT_GLOB_MARK | FT_GLOB_ONLYDIR))
			*(path_buf + len) = '\0';
	}
	else if (IS_DIR(d) && (show_files(e->flags, e->pattern) \
				|| (ft_strcmp(d->d_name, "..") && ft_strcmp(d->d_name, "."))))
		return (FT_GLOB_BOOM_BABY);
	return (FT_GLOB_SUCCESS);
}

int			glob_read_dir(t_glob_env *e, int depth, char const *dirname)
{
	DIR				*dir;
	struct dirent	*d;
	int				ret;
	char			path_buf[PATH_MAX];

	glob_init_dir_name(&dirname, path_buf, e->pattern);
	if ((ret = glob_actually_open_some_folder(e, dirname, depth, &dir)))
		return (ret - 1);
	while ((d = readdir(dir)))
		if ((ret = glob_check_file(e, d, depth, path_buf)) \
			== FT_GLOB_BOOM_BABY && depth > 1)
		{
			if (glob_store_dir_name(path_buf, dirname, d->d_name))
				return (FT_GLOB_NOSPACE);
			if (glob_read_dir(e, depth - 1, path_buf))
				return (glob_close_dir(dir, *(e->flags)) | FT_GLOB_ABORTED);
			remove_last_dir_from_path(path_buf);
			if (!glob_get_sub_pattern(e->sub_pat_buf, e->pattern, depth, \
									*(e->flags)))
				return (FT_GLOB_NOSPACE);
		}
		else if (ret != FT_GLOB_SUCCESS)
			return (glob_close_dir(dir, *(e->flags)) | FT_GLOB_ABORTED);
	return (glob_close_dir(dir, *(e->flags)) == FT_GLOB_ABORTED ? \
			FT_GLOB_ABORTED : FT_GLOB_SUCCESS);
}

int			glob_climb_tree(t_glob_env *e)
{
	int			depth;
	int			ret;
	char const	*magic;

	if ((ret = glob_brace(e)))
		return (ret > 0 ? ret : FT_GLOB_SUCCESS);
	depth = glob_count_depth(e->pattern);
	depth -= show_files(e->flags, e->pattern) ? 0 : 1;
	if ((magic = is_magic(e->magic_buf, e->pattern, e->flags)) \
			&& magic != e->pattern)
	{
		depth = 1 + glob_count_depth(e->pattern) - glob_count_depth(magic);
		depth -= show_files(e->flags, e->pattern) ? 0 : 1;
	}
	if ((*(e->flags) & FT_GLOB_NOMAGIC) \
			|| !ft_strcmp("/", e->pattern) || !ft_strcmp("./", e->pattern))
	{
		return (matchctoradd(e->pattern, false, \
							!(*(e->flags) & FT_GLOB_NOSORT), &e->match_list));
	}
	if (depth > MAX_DEPTH || depth < 1)
		return (FT_GLOB_NOSPACE);
	if ((ret = glob_read_dir(e, depth, magic == e->pattern ? NULL : magic)))
		return (ret);
	return (glob_just_copy_pattern(e));
}
