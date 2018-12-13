/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/internal/fmtpct.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

ssize_t			iofmt_fmtpct(t_stream *s, t_fmt *f, t_varg arg)
{
	(void)s;
	(void)arg;
	if (f->xp && !f->p)
		f->p = -1;
	f->beg = "%";
	f->end = f->beg + ft_strnlen(f->beg,
		(size_t)(f->p < 0 ? INT_MAX : f->p));
	if (f->p < 0 && *f->end)
		return (-(errno = EOVERFLOW));
	f->p = (int32_t)(f->end - f->beg);
	return (0);
}
