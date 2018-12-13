/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/brace.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static int			glob_boom(t_glob_env *e, char const *brace_buf)
{
	char const	*pat_save;
	t_match		*match_save;
	t_match		*link;

	pat_save = e->pattern;
	match_save = e->match_list;
	glob_sanitize_pattern((char *)brace_buf, brace_buf);
	e->pattern = brace_buf;
	e->match_list = NULL;
	glob_climb_tree(e);
	if (!e->match_list)
		matchctoradd(e->pattern, true, \
					!(*(e->flags) & FT_GLOB_NOSORT), &e->match_list);
	if (match_save)
	{
		link = match_save;
		while (link->next)
			link = link->next;
		link->next = e->match_list;
		e->match_list = match_save;
	}
	e->pattern = pat_save;
	return (-1);
}

static int			glob_copy_pattern_and_boom(t_glob_env *e, \
						char const *pat_end, char const *pat, size_t pat_size)
{
	char		brace_buf[PATH_MAX];
	size_t		before_size;
	size_t		after_size;

	before_size = (size_t)\
		(glob_find_opening_brace(e->pattern, e->pattern) - e->pattern);
	after_size = ft_strlen(pat_end);
	if (!(before_size + pat_size + after_size) \
			|| before_size + pat_size + after_size + 1 > PATH_MAX)
		return (-1);
	ft_memcpy(brace_buf, e->pattern, before_size);
	ft_memcpy(brace_buf + before_size, pat, pat_size);
	ft_memcpy(brace_buf + before_size + pat_size, pat_end, after_size + 1);
	return (glob_boom(e, brace_buf));
}

static int			glob_find_sub_brace(t_glob_env *e, char const *comma, \
										char const *pat, char const *pat_end)
{
	if (!comma)
	{
		return (glob_copy_pattern_and_boom(e, \
							pat_end + 1, pat + 1, (size_t)(pat_end - pat) - 1));
	}
	glob_copy_pattern_and_boom(e, \
						pat_end + 1, pat + 1, (size_t)(comma - pat) - 1);
	return (glob_find_sub_brace(e, glob_find_comma(comma + 1, pat_end), \
								comma, pat_end));
}

int					glob_brace(t_glob_env *e)
{
	char const	*pat_start;
	char const	*pat_end;
	char const	*comma;

	if (!(*(e->flags) & FT_GLOB_BRACE))
		return (FT_GLOB_SUCCESS);
	if (!(pat_start = glob_find_opening_brace(e->pattern, e->pattern)))
		return (FT_GLOB_SUCCESS);
	if (!(pat_end = is_there_a_closing_bracket(pat_start, *(e->flags), '}', \
		'{')))
		return (FT_GLOB_SUCCESS);
	if (!(comma = glob_find_comma(pat_start + 1, pat_end)))
		return (FT_GLOB_SUCCESS);
	return (glob_find_sub_brace(e, comma, pat_start, pat_end));
}
