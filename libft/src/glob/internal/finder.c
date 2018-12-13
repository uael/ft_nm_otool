/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/finder.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

char const	*glob_find_comma(char const *pat, char const *pat_end)
{
	int depth;

	depth = 0;
	while (pat < pat_end)
	{
		if (*pat == '{')
			++depth;
		else if (*pat == '}')
			--depth;
		else if (!depth && *pat == ',')
			return (pat);
		++pat;
	}
	return (NULL);
}

char const	*glob_find_opening_brace(char const *pat, char const *pat_start)
{
	if (!*pat)
		return (NULL);
	if (*pat == '{' && (pat == pat_start || *(pat - 1) != '\\'))
		return (pat);
	return (glob_find_opening_brace(pat + 1, pat_start));
}

void		sprglbcp(char *dst, char const *path)
{
	char		bs;
	char const	*p;

	p = path - 1;
	bs = 0;
	while (*++p)
	{
		if (bs || *p != '\\')
		{
			*dst = *p;
			dst++;
			bs = 0;
		}
		else if (*p == '\\')
			bs = 1;
	}
}
