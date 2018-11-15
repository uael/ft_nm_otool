/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fopen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static t_stream	g_open[256];

t_stream		*ft_fopen(char const *filename, int flags, char *buf, size_t s)
{
	int fd;

	if ((fd = open(filename, flags, 0666)) < 0 || fd >= 256)
	{
		ft_dprintf(STDERR_FILENO, "%m\n");
		return (NULL);
	}
	g_open[fd] = (t_stream){
		.buf = (uint8_t *)buf,
		.buf_size = s,
		.fd = fd,
		.flags = FT_FPERM | FT_FNORD,
		.lbf = -1,
		.write = stdiowrite,
		.lock = -1,
	};
	return (g_open + fd);
}

int				ft_fclose(t_stream *s)
{
	if (s->fd < 0 || s->fd > 256)
		return (ft_error(WUT, EINVAL));
	if (g_open + s->fd != s)
		return (ft_error(WUT, EINVAL));
	ft_fflush(s);
	return (close(s->fd));
}
