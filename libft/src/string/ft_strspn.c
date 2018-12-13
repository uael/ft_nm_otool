/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_strspn.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/string.h"

inline size_t	ft_strspn(const char *s, const char *accept)
{
	const char	*p = s;
	const char	*a;
	size_t		c;

	c = 0;
	while (*p)
	{
		a = accept;
		while (*a)
			if (*p == *a++)
				break ;
		if (*a == '\0')
			return (c);
		++c;
		++p;
	}
	return (c);
}
