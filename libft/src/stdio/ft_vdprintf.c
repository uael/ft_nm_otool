/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/ft_vdprintf.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal/internal.h"

int	ft_vdprintf(int fd, char const *fmt, va_list ap)
{
	t_stream f;

	if (fd == 1)
		return (ft_vfprintf(g_stdout, fmt, ap));
	if (fd == 2)
		return (ft_vfprintf(g_stderr, fmt, ap));
	f = (t_stream){
		.fd = fd,
		.lbf = FT_EOF,
		.write = stdiowrite,
		.buf = (void *)fmt,
		.buf_size = 0,
		.lock = -1
	};
	return (ft_vfprintf(&f, fmt, ap));
}
