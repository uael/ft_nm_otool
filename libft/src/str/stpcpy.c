/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/stpcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline char		*ft_stpcpy(char *dest, char const *src)
{
	char		*d;
	const char	*s;

	d = dest;
	s = src;
	*d++ = *s;
	while (*s++ != '\0')
		*d++ = *s;
	return (d - 1);
}
