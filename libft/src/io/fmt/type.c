/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fmt/type.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "internal.h"

static uint8_t const g_states[]['z' - '%' + 1] = {
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_INT,
		['i' - '%'] = T_INT,
		['o' - '%'] = T_UINT,
		['u' - '%'] = T_UINT,
		['x' - '%'] = T_UINT,
		['X' - '%'] = T_UINT,
		['e' - '%'] = T_DBL,
		['c' - '%'] = T_CHAR,
		['C' - '%'] = T_INT,
		['s' - '%'] = T_PTR,
		['S' - '%'] = T_PTR,
		['p' - '%'] = T_UIPTR,
		['n' - '%'] = T_PTR,
		['m' - '%'] = T_NOARG,
		['l' - '%'] = T_LPRE,
		['h' - '%'] = T_HPRE,
		['z' - '%'] = T_ZTPRE,
		['j' - '%'] = T_JPRE,
		['t' - '%'] = T_ZTPRE,
		['D' - '%'] = T_LONG,
		['O' - '%'] = T_ULONG,
		['U' - '%'] = T_ULONG,
	},
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_LONG,
		['i' - '%'] = T_LONG,
		['o' - '%'] = T_ULONG,
		['u' - '%'] = T_ULONG,
		['x' - '%'] = T_ULONG,
		['X' - '%'] = T_ULONG,
		['c' - '%'] = T_INT,
		['s' - '%'] = T_PTR,
		['n' - '%'] = T_PTR,
		['l' - '%'] = T_LLPRE,
		['D' - '%'] = T_LONG,
		['O' - '%'] = T_ULONG,
		['U' - '%'] = T_ULONG,
		['p' - '%'] = T_UIPTR
	},
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_LLONG,
		['i' - '%'] = T_LLONG,
		['o' - '%'] = T_ULLONG,
		['u' - '%'] = T_ULLONG,
		['x' - '%'] = T_ULLONG,
		['X' - '%'] = T_ULLONG,
		['n' - '%'] = T_PTR,
		['D' - '%'] = T_LLONG,
		['O' - '%'] = T_ULLONG,
		['U' - '%'] = T_ULLONG,
		['p' - '%'] = T_UIPTR
	},
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_SHORT,
		['i' - '%'] = T_SHORT,
		['o' - '%'] = T_USHORT,
		['u' - '%'] = T_USHORT,
		['x' - '%'] = T_USHORT,
		['X' - '%'] = T_USHORT,
		['n' - '%'] = T_PTR,
		['h' - '%'] = T_HHPRE,
		['D' - '%'] = T_INT,
		['O' - '%'] = T_UINT,
		['U' - '%'] = T_ULONG,
		['S' - '%'] = T_PTR,
		['C' - '%'] = T_INT
	},
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_CHAR,
		['i' - '%'] = T_CHAR,
		['o' - '%'] = T_UCHAR,
		['u' - '%'] = T_UCHAR,
		['x' - '%'] = T_UCHAR,
		['X' - '%'] = T_UCHAR,
		['n' - '%'] = T_PTR,
		['D' - '%'] = T_USHORT,
		['O' - '%'] = T_USHORT,
		['U' - '%'] = T_ULONG,
		['S' - '%'] = T_PTR,
		['C' - '%'] = T_INT
	},
	{
		['%' - '%'] = T_NOARG,
		['n' - '%'] = T_PTR,
	},
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_PDIFF,
		['i' - '%'] = T_PDIFF,
		['o' - '%'] = T_SIZET,
		['u' - '%'] = T_SIZET,
		['x' - '%'] = T_SIZET,
		['X' - '%'] = T_SIZET,
		['D' - '%'] = T_PDIFF,
		['O' - '%'] = T_SIZET,
		['U' - '%'] = T_SIZET,
		['n' - '%'] = T_PTR,
		['S' - '%'] = T_PTR,
		['C' - '%'] = T_PDIFF
	},
	{
		['%' - '%'] = T_NOARG,
		['d' - '%'] = T_IMAX,
		['i' - '%'] = T_IMAX,
		['o' - '%'] = T_UMAX,
		['u' - '%'] = T_UMAX,
		['x' - '%'] = T_UMAX,
		['X' - '%'] = T_UMAX,
		['D' - '%'] = T_IMAX,
		['O' - '%'] = T_UMAX,
		['U' - '%'] = T_UMAX,
		['n' - '%'] = T_PTR,
		['S' - '%'] = T_PTR,
		['C' - '%'] = T_IMAX
	}
};

static inline void	poparg2(t_varg *arg, int type, va_list ap)
{
	if (type == T_SIZET)
		arg->i = va_arg(ap, size_t);
	else if (type == T_IMAX)
		arg->i = (uintmax_t)va_arg(ap, intmax_t);
	else if (type == T_UMAX)
		arg->i = va_arg(ap, uintmax_t);
	else if (type == T_PDIFF)
		arg->i = (uintmax_t)va_arg(ap, ssize_t);
	else if (type == T_UIPTR)
		arg->i = (uintptr_t)va_arg(ap, void *);
	else if (type == T_DBL)
		arg->f = va_arg(ap, double);
	else if (type == T_LDBL)
		arg->f = va_arg(ap, long double);
}

static inline void	poparg(t_varg *arg, int type, va_list ap)
{
	if (type == T_PTR)
		arg->p = va_arg(ap, void *);
	else if (type == T_INT)
		arg->i = (uintmax_t)va_arg(ap, int);
	else if (type == T_UINT)
		arg->i = va_arg(ap, unsigned int);
	else if (type == T_LONG)
		arg->i = (uintmax_t)va_arg(ap, long);
	else if (type == T_ULONG)
		arg->i = va_arg(ap, unsigned long);
	else if (type == T_ULLONG)
		arg->i = va_arg(ap, unsigned long long);
	else if (type == T_SHORT)
		arg->i = (uintmax_t)(short)va_arg(ap, int);
	else if (type == T_USHORT)
		arg->i = (unsigned short)va_arg(ap, int);
	else if (type == T_CHAR)
		arg->i = (uintmax_t)(signed char)va_arg(ap, int);
	else if (type == T_UCHAR)
		arg->i = (unsigned char)va_arg(ap, int);
	else if (type == T_LLONG)
		arg->i = (uintmax_t)va_arg(ap, long long);
	else
		poparg2(arg, type, ap);
}

int					iofmt_poptype(t_varg *arg, int *type, char **s, va_list ap)
{
	uint32_t	st;
	uint32_t	ps;

	st = 0;
	if (**s != '%' && ((unsigned)**s - 'A' > 'z' - 'A'))
		return (-1);
	ps = st;
	if (!(st = g_states[st][**s - '%']))
		return (-1);
	++*s;
	while (st - 1 < T_STOP)
	{
		if (**s != '%' && ((unsigned)**s - 'A' > 'z' - 'A'))
			return (-1);
		ps = st;
		if (!(st = g_states[st][**s - '%']))
			return (-1);
		++*s;
	}
	if (st != T_NOARG)
		poparg(arg, st, ap);
	*type = (*s)[-1];
	if (ps && (*type & 15) == 3)
		*type &= ~32;
	return (st);
}
