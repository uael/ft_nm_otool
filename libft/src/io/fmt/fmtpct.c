/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fmt/fmtpct.c                                    :+:      :+:    :+:   */
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
		return (ft_error(-1, EOVERFLOW));
	f->p = (int32_t)(f->end - f->beg);
	return (0);
}
