/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strncpy.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline char	*ft_strncpy(char *dest, char const *src, size_t n)
{
	size_t i;

	i = 0;
	while (i < n && src[i])
	{
		*(dest + i) = *(src + i);
		++i;
	}
	if (src[i])
		*(dest + i) = '\0';
	else
		while (i < n)
			*(dest + i++) = '\0';
	return (dest);
}
