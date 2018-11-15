/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/strnchr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

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
