/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/vdprintf.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "internal.h"

int	ft_vdprintf(int fd, char const *fmt, va_list ap)
{
	t_stream f;

	f = (t_stream){
		.fd = fd,
		.lbf = EOF,
		.write = stdiowrite,
		.buf = (void *)fmt,
		.buf_size = 0,
		.lock = -1
	};
	return (ft_vfprintf(&f, fmt, ap));
}
