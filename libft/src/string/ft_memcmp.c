/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_memcmp.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline int	ft_memcmp(void const *s1, void const *s2, size_t n)
{
	const uint8_t *c_s1 = s1;
	const uint8_t *c_s2 = s2;

	if (n != 0)
		while (n--)
			if (*c_s1++ != *c_s2++)
				return (*--c_s1 - *--c_s2);
	return (0);
}
