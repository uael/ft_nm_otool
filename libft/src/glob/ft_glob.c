/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/ft_glob.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal/internal.h"

/*
** ft_glob, ft_globfree -
** find pathnames matching a pattern, free memory from ft_glob()
*/

static char		**alloc_pathv(t_glob *pglob)
{
	if ((pglob->gl_flags & FT_GLOB_DOOFFS))
	{
		if (!(pglob->gl_pathv = ft_malloc(sizeof(char *) \
								* (pglob->gl_pathc + pglob->gl_offs + 1))))
			return (NULL);
		return (pglob->gl_pathv + pglob->gl_offs);
	}
	else
	{
		if (!(pglob->gl_pathv = ft_malloc(sizeof(char *) * (pglob->gl_pathc + 1))))
			return (NULL);
		return (pglob->gl_pathv);
	}
}

static int		copy_match_to_glob_struct(t_match *match_list, t_glob *pglob)
{
	char	**av;

	pglob->gl_pathc = list_len(match_list);
	if (!(av = alloc_pathv(pglob)))
	{
		matchdtor(match_list);
		return (FT_GLOB_NOSPACE);
	}
	while (match_list)
	{
		*av = (char *)(match_list->buf);
		av++;
		match_list = match_list->next;
	}
	*av = NULL;
	return (FT_GLOB_SUCCESS);
}

int				ft_glob(const char *pattern, int flags, t_glob *pglob)
{
	t_glob_env		glob_env;
	char			pat_buf[PATH_MAX];
	int				ret;

	if ((flags & ~__FT_GLOB_FLAGS))
		return (FT_GLOB_NOSYS);
	if (!(flags & FT_GLOB_NOCHECK) && !*pattern)
		return (FT_GLOB_NOMATCH);
	ft_memset(&glob_env, 0, sizeof(t_glob_env));
	glob_sanitize_pattern(pat_buf, pattern);
	pglob->gl_flags = flags;
	glob_env.flags = &(pglob->gl_flags);
	glob_env.pattern = pat_buf;
	if ((ret = glob_climb_tree(&glob_env)))
	{
		matchdtor(glob_env.match_list);
		return (ret);
	}
	return (copy_match_to_glob_struct(glob_env.match_list, pglob));
}

void			ft_globfree(t_glob *pglob)
{
	char	**av;

	if (!pglob->gl_pathv)
		return ;
	if ((pglob->gl_flags & FT_GLOB_DOOFFS))
		av = pglob->gl_pathv + pglob->gl_offs;
	else
		av = pglob->gl_pathv;
	while (pglob->gl_pathc--)
	{
		ft_free((t_byte *)(*av) - sizeof(t_match) + sizeof(t_byte *));
		av++;
	}
	ft_free(pglob->gl_pathv);
	pglob->gl_pathv = NULL;
}
