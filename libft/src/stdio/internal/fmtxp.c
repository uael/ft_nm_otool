/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/internal/fmtxp.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

ssize_t	iofmt_fmtxp(t_stream *s, t_fmt *f, t_varg arg)
{
	int prec;

	(void)s;
	if (f->xp && f->p < 0)
		return (-(errno = EOVERFLOW));
	if (f->xp)
		f->fl &= ~ZERO_PAD;
	if (!arg.i && !f->p)
	{
		f->beg = f->end;
		return (0);
	}
	if ((prec = (int)(f->end - f->beg + !arg.i)) && prec > f->p)
		f->p = prec;
	return (0);
}
