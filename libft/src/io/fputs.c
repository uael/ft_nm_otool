/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fputs.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

int	ft_fputs(t_stream *f, char const *str)
{
	size_t len;

	if (!str || !(len = ft_strlen(str)))
		return (0);
	if ((len = fwritex(f, (const uint8_t *)str, len)) > INT_MAX)
		return (ft_error(WUT, EOVERFLOW));
	return ((int)len);
}
