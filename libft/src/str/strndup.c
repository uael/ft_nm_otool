/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/strndup.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline char	*ft_strndup(char const *src, size_t n)
{
	char	*dup;

	dup = ft_malloc((n + 1) * sizeof(char));
	ft_memcpy(dup, src, n);
	dup[n] = '\0';
	return (dup);
}
