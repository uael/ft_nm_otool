/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/mempcpy.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline size_t	ft_mempcpy(void *dst, void const *src, size_t n)
{
	size_t			c;
	uint8_t			*d;
	uint8_t const	*s;

	c = 0;
	d = (uint8_t *)dst;
	s = (uint8_t const *)src;
	while (n)
	{
		if (ft_isprint(*s))
		{
			*d++ = *s++;
			++c;
		}
		else
			++s;
		--n;
	}
	return (c);
}
