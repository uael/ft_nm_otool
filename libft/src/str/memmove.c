/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str/memccpy.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:44:14 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/08 14:29:11 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/str.h"

inline void	*ft_memmove(void *dst, void const *src, size_t len)
{
	if (dst < src)
		return (ft_memcpy(dst, src, len));
	if (dst > src)
		return (ft_memrcpy(dst, src, len));
	return (dst);
}
