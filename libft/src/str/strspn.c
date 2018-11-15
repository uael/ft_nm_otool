/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/strspn.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline size_t	ft_strspn(const char *s, const char *accept)
{
	const char	*p;
	const char	*a;
	size_t		c;

	c = 0;
	p = s - 1;
	while (*++p)
	{
		a = accept - 1;
		while (*++a)
			if (*p == *a)
				break ;
		if (*a == '\0')
			return (c);
		else
			++c;
	}
	return (c);
}
