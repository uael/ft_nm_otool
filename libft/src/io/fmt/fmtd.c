/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fmt/fmtd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <limits.h>

#include "internal.h"

static char *g_xdigits = "0123456789ABCDEF";

ssize_t	iofmt_fmtp(t_stream *s, t_fmt *f, t_varg arg)
{
	uintmax_t	x;
	char		*z;

	x = arg.i;
	z = f->end;
	while (x)
	{
		*--z = (char)(g_xdigits[x & 15] | (f->type & 32));
		x >>= 4;
	}
	f->beg = z;
	f->pref += ('x' >> 4);
	f->pl = 2;
	return (iofmt_fmtxp(s, f, arg));
}

ssize_t	iofmt_fmtx(t_stream *s, t_fmt *f, t_varg arg)
{
	uintmax_t	x;
	char		*z;

	(void)s;
	x = arg.i;
	z = f->end;
	while (x)
	{
		*--z = (char)(g_xdigits[x & 15] | (f->type & 32));
		x >>= 4;
	}
	f->beg = z;
	if ((f->fl & ALT_FORM) && arg.i)
	{
		f->pref += (f->type >> 4);
		f->pl = 2;
	}
	return (iofmt_fmtxp(s, f, arg));
}

ssize_t	iofmt_fmto(t_stream *s, t_fmt *f, t_varg arg)
{
	uintmax_t	x;
	char		*z;

	(void)s;
	x = arg.i;
	z = f->end;
	while (x)
	{
		*--z = (char)('0' + (x & 7));
		x >>= 3;
	}
	f->beg = z;
	if ((f->fl & ALT_FORM) && f->p < f->end - f->beg + 1)
		f->p = (int32_t)(f->end - f->beg + 1);
	return (iofmt_fmtxp(s, f, arg));
}

ssize_t	iofmt_fmtdi(t_stream *s, t_fmt *f, t_varg arg)
{
	(void)s;
	f->pl = 1;
	if (arg.i > INTMAX_MAX)
		arg.i = (uintmax_t)-arg.i;
	else if ((f->fl & MARK_POS))
		++f->pref;
	else if ((f->fl & PAD_POS))
		f->pref += 2;
	else
		f->pl = 0;
	return (iofmt_fmtu(s, f, arg));
}

ssize_t	iofmt_fmtu(t_stream *s, t_fmt *f, t_varg arg)
{
	uintmax_t		x;
	char			*z;
	unsigned long	y;

	(void)s;
	x = arg.i;
	z = f->end;
	while (x > ULONG_MAX)
	{
		*--z = (char)('0' + x % 10);
		x /= 10;
	}
	y = x;
	while (y)
	{
		*--z = (char)('0' + y % 10);
		y /= 10;
	}
	f->beg = z;
	return (iofmt_fmtxp(s, f, arg));
}
