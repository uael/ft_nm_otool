/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/ft_vfprintf.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal/internal.h"

inline size_t		iofmt_out(t_stream *f, const char *s, size_t l)
{
	if (!(f->flags & FT_FERRO))
		return (fwritex(f, (void *)s, l));
	return (l);
}

inline void			iofmt_pad(t_stream *f, t_pad p)
{
	char pad[256];

	if ((p.flags & (LEFT_ADJ | ZERO_PAD)) || p.len >= (size_t)p.width)
		return ;
	p.len = p.width - p.len;
	ft_memset(pad, p.c, p.len > sizeof(pad) ? sizeof(pad) : p.len);
	while (p.len >= sizeof(pad))
	{
		iofmt_out(f, pad, sizeof(pad));
		p.len -= sizeof(pad);
	}
	iofmt_out(f, pad, p.len);
}

static inline int	printarg(t_stream *f, char **pct, va_list ap)
{
	int		type;
	int		len;
	t_fmt	fm;
	t_varg	arg;

	if (iofmt_parse(&fm, pct, ap))
		return (-(errno = EOVERFLOW));
	if (iofmt_poptype(&arg, &type, pct, ap) < 0)
	{
		if (!**pct)
			return (0);
		type = 'c';
		arg.i = (uintmax_t)(*(*pct)++);
	}
	if ((len = iofmt_eval(type, fm, arg, f)) < 0)
		return (len == -2 ? -1 : 0);
	return (len);
}

inline int			ft_vfprintf(t_stream *f, char const *fmt, va_list ap)
{
	int		len;
	size_t	ret;
	char	*pct;

	ret = 0;
	while ((pct = ft_strchr(fmt, '%')))
	{
		if ((ret += iofmt_out(f, fmt, pct++ - fmt)) > INT_MAX)
			return (-(errno = EOVERFLOW));
		if (!*pct)
		{
			fmt = pct;
			break ;
		}
		if ((len = printarg(f, &pct, ap)) < 0 || len + ret > INT_MAX)
			return (-(errno = EOVERFLOW));
		ret += len;
		fmt = pct;
	}
	if ((ret += iofmt_out(f, fmt, ft_strlen(fmt))) > INT_MAX)
		return (-(errno = EOVERFLOW));
	return ((int)ret);
}
