/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/internal/fmts.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

#include <float.h>
#include <stdlib.h>

ssize_t			iofmt_fmts(t_stream *s, t_fmt *f, t_varg arg)
{
	(void)s;
	f->beg = arg.p ? arg.p : "(null)";
	f->end = f->beg + ft_strnlen(f->beg,
		(size_t)(f->p < 0 ? INT_MAX : f->p));
	if (f->p < 0 && *f->end)
		return (-(errno = EOVERFLOW));
	f->p = (int32_t)(f->end - f->beg);
	return (0);
}

static ssize_t	suout(t_stream *s, t_fmt *f, t_varg arg)
{
	size_t	i;
	int		l;
	wchar_t	*ws;
	char	mb[MB_CUR_MAX];

	iofmt_pad(s, (t_pad){' ', f->w, (size_t)f->p, f->fl});
	iofmt_pad(s, (t_pad){'0', f->w, (size_t)f->p, f->fl ^ ZERO_PAD});
	iofmt_pad(s, (t_pad){'0', f->p, f->end - f->beg, 0});
	ws = arg.p;
	i = 0;
	while (i < (size_t)f->p && *ws &&
		(i + (l = ft_wctomb(mb, *ws++))) <= (size_t)f->p)
	{
		iofmt_out(s, mb, (size_t)l);
		i += l;
	}
	iofmt_pad(s, (t_pad){' ', f->w, (size_t)f->p, f->fl ^ LEFT_ADJ});
	return (f->w > f->p ? f->w : f->p);
}

ssize_t			iofmt_fmtsu(t_stream *s, t_fmt *f, t_varg arg)
{
	size_t	i;
	int		l;
	wchar_t	*ws;
	char	mb[MB_CUR_MAX];

	if (!(ws = arg.p))
		return (iofmt_fmts(s, f, arg));
	f->done = 1;
	i = 0;
	l = 0;
	while (i < (size_t)f->p && *ws &&
		(l = ft_wctomb(mb, *ws++)) >= 0 && (size_t)l <= (size_t)(f->p - i))
		i += l;
	if (l < 0)
		return (-2);
	if (i > INT_MAX)
		return (-(errno = EOVERFLOW));
	f->p = (int)i;
	return (suout(s, f, arg));
}

ssize_t			iofmt_fmtc(t_stream *s, t_fmt *f, t_varg arg)
{
	(void)s;
	f->p = 1;
	f->beg = f->end - f->p;
	*f->beg = (char)arg.i;
	return (0);
}

ssize_t			iofmt_fmtcu(t_stream *s, t_fmt *f, t_varg arg)
{
	(void)s;
	f->wc[0] = (wchar_t)arg.i;
	f->wc[1] = 0;
	arg.p = f->wc;
	if (!*((wchar_t	*)arg.p))
		return (1);
	f->p = -1;
	return (iofmt_fmtsu(s, f, arg));
}
