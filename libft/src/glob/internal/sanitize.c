/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob/internal/sanitize.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcanal <mc.maxcanal@gmail.com>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by mcanal            #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by mcanal           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

/*
** remove all the ugly stuffs from patterns (mostly extraneous slashes)
** note: dst and src might overlap
*/

char						*glob_sanitize_pattern(char *dst, const char *src)
{
	char	tmp_buf[PATH_MAX];
	char	*b;

	b = tmp_buf;
	while (*src && (size_t)(b - tmp_buf) < PATH_MAX)
	{
		if (ft_memcmp(src, "//", 2))
			*b++ = *src;
		src++;
	}
	*b = '\0';
	return (ft_strcpy(dst, tmp_buf));
}
