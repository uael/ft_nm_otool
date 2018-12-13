/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/internal/eval.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

#include <float.h>

static t_fmtcb	*g_fmts[] = {
	['%' - '%'] = iofmt_fmtpct,
	['s' - '%'] = iofmt_fmts,
	['S' - '%'] = iofmt_fmtsu,
	['p' - '%'] = iofmt_fmtp,
	['X' - '%'] = iofmt_fmtx,
	['x' - '%'] = iofmt_fmtx,
	['O' - '%'] = iofmt_fmto,
	['o' - '%'] = iofmt_fmto,
	['D' - '%'] = iofmt_fmtdi,
	['d' - '%'] = iofmt_fmtdi,
	['i' - '%'] = iofmt_fmtdi,
	['u' - '%'] = iofmt_fmtu,
	['U' - '%'] = iofmt_fmtu,
	['c' - '%'] = iofmt_fmtc,
	['C' - '%'] = iofmt_fmtcu,
	['m' - '%'] = iofmt_fmtm,
};

static ssize_t	evalt(t_stream *s, t_fmt *f, t_varg arg)
{
	t_fmtcb *cb;

	if ((cb = g_fmts[f->type - '%']))
		return (cb(s, f, arg));
	return (-(errno = EINVAL));
}

inline int		iofmt_eval(int t, t_fmt f, t_varg a, t_stream *s)
{
	char		buf[sizeof(uintmax_t) * 3 + 3 + LDBL_MANT_DIG / 4];
	ssize_t		r;

	f.type = t;
	f.pref = "-+   0X0x";
	f.end = buf + sizeof(buf);
	(f.fl & LEFT_ADJ) ? (f.fl &= ~ZERO_PAD) : 0;
	if ((r = evalt(s, &f, a)) || f.done)
		return ((int)r);
	if (f.p < f.end - f.beg)
		f.p = (int32_t)(f.end - f.beg);
	if (f.p > INT_MAX - f.pl)
		return (-(errno = EOVERFLOW));
	if (f.w < f.pl + f.p)
		f.w = (int16_t)(f.pl + f.p);
	iofmt_pad(s, (t_pad){' ', f.w, (size_t)(f.pl + f.p), f.fl});
	iofmt_out(s, f.pref, (size_t)f.pl);
	iofmt_pad(s, (t_pad){'0', f.w, (size_t)(f.pl + f.p), f.fl ^ ZERO_PAD});
	iofmt_pad(s, (t_pad){'0', f.p, f.end - f.beg, 0});
	iofmt_out(s, f.beg, f.end - f.beg);
	iofmt_pad(s, (t_pad){' ', f.w, (size_t)(f.pl + f.p), f.fl ^ LEFT_ADJ});
	return (f.w);
}
