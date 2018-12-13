/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strrchr.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline char	*ft_strrchr(char const *s, int c)
{
	char *beg;

	beg = (char *)s;
	while (*s)
		++s;
	if (c == 0)
		return ((char *)s);
	while (s >= beg)
		if (*s-- == c)
			return ((char *)s + 1);
	return (NULL);
}
