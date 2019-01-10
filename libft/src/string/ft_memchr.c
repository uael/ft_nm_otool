/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_memchr.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline void	*ft_memchr(void const *s, int c, size_t n)
{
	const int	*c_s = s;
	uint8_t		*b_s;
	int			repeated_c;
	int			mask;

	repeated_c = (c & 0xFF) | ((c & 0xFF) << 8);
	repeated_c |= repeated_c << 16;
	while (n >= 4)
	{
		mask = *c_s++ ^ repeated_c;
		if ((mask & 0xff) == 0)
			return ((void *)c_s - 4);
		if ((mask & 0xff00) == 0)
			return ((void *)c_s - 3);
		if ((mask & 0xff0000) == 0)
			return ((void *)c_s - 2);
		if ((mask & 0xff000000) == 0)
			return ((void *)c_s - 1);
		n -= 4;
	}
	b_s = (uint8_t *)c_s;
	while (n--)
		if (*b_s++ == (uint8_t)c)
			return ((void *)b_s - 1);
	return (NULL);
}
