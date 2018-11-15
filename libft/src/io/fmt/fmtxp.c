/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fmt/fmtxp.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <libft.h>
#include <limits.h>

#include "internal.h"

ssize_t	iofmt_fmtxp(t_stream *s, t_fmt *f, t_varg arg)
{
	int prec;

	(void)s;
	if (f->xp && f->p < 0)
		return (ft_error(-1, EOVERFLOW));
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
