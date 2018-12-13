/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strestr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline char	*ft_strestr(char const *s1, char const *s2)
{
	char const *s3;
	char const *s4;

	s3 = s1;
	s4 = s2;
	while (*s1)
		++s1;
	while (*s2)
		++s2;
	while ((*(uint8_t *)s1 == *(uint8_t *)s2) && s1 > s3 && s2 > s4)
	{
		--s1;
		--s2;
	}
	return (s2 == s4 ? (char *)s3 : NULL);
}
