/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/internal/std.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static ssize_t	writeloop(int fd, uint8_t const *buf, size_t len)
{
	size_t	outc;
	ssize_t	out;

	outc = 0;
	while (outc < len)
	{
		if ((out = write(fd, buf + outc, len - outc)) > 0)
			outc += (size_t)out;
		else if (out && (errno != EAGAIN && errno != EINTR))
			return (-1);
	}
	return (ssize_t)outc;
}

static size_t	ioerror(t_stream *f)
{
	f->wpos = NULL;
	f->wbase = NULL;
	f->wend = NULL;
	f->flags |= FT_FERRO;
	return (0);
}

size_t			stdiowrite(t_stream *f, uint8_t const *s, size_t len)
{
	size_t	outc;
	ssize_t	out;

	if ((outc = f->wpos - f->wbase))
	{
		if ((out = writeloop(f->fd, f->wbase, outc)) < 0)
			return (ioerror(f));
		outc = (size_t)out;
	}
	out = 0;
	if (len && ((out = writeloop(f->fd, s, len)) < 0))
		return (ioerror(f));
	f->wend = f->buf + f->buf_size;
	f->wpos = f->buf;
	f->wbase = f->buf;
	return (outc + out);
}

static uint8_t	g_stderr_buf[0];
static t_stream	g_stderr_s = {
	.buf = g_stderr_buf,
	.buf_size = 0,
	.fd = STDERR_FILENO,
	.flags = FT_FPERM | FT_FNORD,
	.lbf = -1,
	.write = stdiowrite,
	.lock = -1,
};
t_stream		*g_stderr = &g_stderr_s;

static uint8_t	g_stdout_buf[FT_BUFSIZ];
static t_stream	g_stdout_s = {
	.buf = g_stdout_buf,
	.buf_size = sizeof(g_stdout_buf),
	.fd = STDOUT_FILENO,
	.flags = FT_FPERM | FT_FNORD,
	.lbf = -1,
	.write = stdiowrite,
	.lock = -1,
};
t_stream		*g_stdout = &g_stdout_s;
