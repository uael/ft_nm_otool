/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/match.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** glob_match - glob util function to check if a pattern actually match
*/

#include "internal.h"

static int		handle_rev_char_class(char const *pat, char const *str, \
									int flags, int matched)
{
	if (!*pat)
		return (false);
	if (*pat == '\\' && !(flags & FT_GLOB_NOESCAPE))
	{
		return (handle_rev_char_class(pat + 2, str, flags, \
									matched & (*(pat + 1) != *str)));
	}
	if (*pat == '[' && *(pat + 1) == ']')
	{
		return (matched & (*str != *pat) ? glob_match(pat + 2, \
												str + 1, flags) : false);
	}
	if (*pat == ']' && *(pat - 2) != '[')
		return (matched ? glob_match(pat + 1, str + 1, flags) : false);
	if (*(pat + 1) == '-' && *(pat + 2) != ']')
	{
		return (handle_rev_char_class(pat + 3, str, flags, matched &
										(*str < *pat || *str > *(pat + 2))));
	}
	return (handle_rev_char_class(pat + 1, str, flags,
										matched & (*str != *pat)));
}

static int		handle_char_class(char const *pat, char const *str, int flags, \
							int matched)
{
	if (!*pat)
		return (false);
	if (*pat == '\\' && !(flags & FT_GLOB_NOESCAPE))
	{
		return (handle_char_class(pat + 2, str, flags, \
									matched | (*(pat + 1) == *str)));
	}
	if (*pat == '[' && *(pat + 1) == ']')
	{
		return (matched | (*str == *pat) ? \
			glob_match(pat + 2, str + 1, flags) : false);
	}
	if (*pat == ']' && *(pat - 1) != '[')
	{
		return (matched ? glob_match(pat + 1, str + 1, flags) : false);
	}
	if (*(pat + 1) == '-' && *(pat + 2) != ']')
	{
		return (handle_char_class(pat + 3, str, flags, \
							matched | (*str >= *pat && *str <= *(pat + 2))));
	}
	return (handle_char_class(pat + 1, str, flags, \
							matched | (*str == *pat)));
}

static int		handle_str_wildcard(char const *pat, char const *str, \
									int flags, int depth)
{
	if (depth > MAX_DEPTH)
		return (false);
	if (!*pat)
		return (true);
	if (*pat == '*')
		return (handle_str_wildcard(pat + 1, str, flags, depth));
	if (glob_match(pat, str, flags))
		return (true);
	if (!*str)
		return (glob_match(pat, str, flags));
	return (handle_str_wildcard(pat, str + 1, flags, depth + 1));
}

int				glob_match(char const *pat, char const *str, int flags)
{
	if (*pat == '\\' && !(flags & FT_GLOB_NOESCAPE))
	{
		return ((*(pat + 1) == *str) ?
				glob_match(pat + 2, str + 1, flags) : false);
	}
	if (*pat == '[')
	{
		if (*(pat + 1) == '!' || *(pat + 1) == '^')
			return (handle_rev_char_class(pat + 2, str, flags, true));
		else
			return (handle_char_class(pat + 1, str, flags, false));
	}
	if (*pat == '*')
		return (handle_str_wildcard(pat + 1, str, flags, 1));
	if (*pat == '?')
		return (*str ? glob_match(pat + 1, str + 1, flags) : false);
	if (*pat != *str)
		return (false);
	if (!*pat)
		return (!*str ? true : false);
	return (glob_match(pat + 1, str + 1, flags));
}
