/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/show.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

int	show_hidden_files(int flags, char pat_start)
{
	return (!(flags & FT_GLOB_MAGCHAR) \
			|| (flags & FT_GLOB_PERIOD) \
			|| pat_start == '.');
}

int	show_files(int *flags, char const *pattern)
{
	if (*pattern && *(pattern + 1) \
		&& *(pattern + ft_strlen(pattern) - 1) == '/')
	{
		*flags |= FT_GLOB_ONLYDIR;
		return (false);
	}
	return (true);
}
