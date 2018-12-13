/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strlcat.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline size_t	ft_strlcat(char *dest, char const *src, size_t size)
{
	size_t	slen;
	size_t	dlen;

	slen = (size_t)ft_strlen(src);
	dlen = (size_t)ft_strnlen(dest, size);
	if (dlen == size)
		return (size + slen);
	if (slen < size - dlen)
		ft_strncpy(dest + dlen, src, slen + 1);
	else
	{
		ft_strncpy(dest + dlen, src, size - dlen - 1);
		dest[size - 1] = '\0';
	}
	return (dlen + slen);
}
