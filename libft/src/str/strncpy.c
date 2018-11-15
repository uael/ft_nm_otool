/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/strncpy.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

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
