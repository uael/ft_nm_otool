/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strncmp.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline int	ft_strncmp(char const *s1, char const *s2, size_t n)
{
	while (n && ((uint8_t)*s1 == (uint8_t)*s2) && *s1)
	{
		++s1;
		++s2;
		--n;
	}
	return (n == 0 ? 0 : *(uint8_t *)s1 - *(uint8_t *)s2);
}
