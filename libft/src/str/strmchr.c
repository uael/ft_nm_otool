/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/strmchr.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline char	*ft_strmchr(char const *s, char const *c)
{
	while (*s)
		if (ft_strchr(c, *s++))
			return ((char *)s - 1);
	return (NULL);
}
