/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strnchr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline char	*ft_strnchr(char const *s, int c, size_t n)
{
	if (c == 0)
		while (n--)
			if (!*s++)
				return ((char *)s - 1);
	if (c == 0)
		return (NULL);
	while (n-- && *s)
		if (*s++ == c)
			return ((char *)s - 1);
	return (NULL);
}
