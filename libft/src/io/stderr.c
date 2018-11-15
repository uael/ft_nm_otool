/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stderr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static uint8_t	g_buf[1];
static t_stream	g_f = {
	.buf = g_buf,
	.buf_size = 0,
	.fd = STDERR_FILENO,
	.flags = FT_FPERM | FT_FNORD,
	.lbf = -1,
	.write = stdiowrite,
	.lock = -1,
};
t_stream		*g_stderr = &g_f;
