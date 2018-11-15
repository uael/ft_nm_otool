/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/vsnprintf.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <limits.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include "vsnprintf.h"

static size_t	snwrite(t_stream *f, uint8_t const *s, size_t len)
{
	t_cookie	*c;
	size_t		k;

	c = f->cookie;
	k = f->wpos - f->wbase;
	if (c->n < k)
		k = c->n;
	if (k)
	{
		ft_memcpy(c->s, f->wbase, k);
		c->s += k;
		c->n -= k;
	}
	k = c->n < len ? c->n : len;
	if (k)
	{
		ft_memcpy(c->s, s, k);
		c->s += k;
		c->n -= k;
	}
	*c->s = 0;
	f->wbase = f->buf;
	f->wpos = f->buf;
	return (len);
}

int				ft_vsnprintf(char *s, size_t n, char const *fmt, va_list ap)
{
	uint8_t		buf[1];
	char		str[1];
	t_cookie	c;
	t_stream	f;

	c = (t_cookie){ .s = n ? s : str, .n = n ? n - 1 : 0 };
	f = (t_stream){
		.lbf = EOF,
		.write = snwrite,
		.lock = -1,
		.buf = buf,
		.cookie = &c
	};
	if (n > INT_MAX)
	{
		errno = EOVERFLOW;
		return (-1);
	}
	*c.s = 0;
	return (ft_vfprintf(&f, fmt, ap));
}
