/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fmt.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_FMT_H
# define IO_FMT_H

# include <stddef.h>

# include "internal.h"

# define ALT_FORM   (1U<<('#'-' '))
# define ZERO_PAD   (1U<<('0'-' '))
# define LEFT_ADJ   (1U<<('-'-' '))
# define PAD_POS    (1U<<(' '-' '))
# define MARK_POS   (1U<<('+'-' '))
# define GROUPED    (1U<<('\''-' '))

# define FLAGMASK (ALT_FORM|ZERO_PAD|LEFT_ADJ|PAD_POS|MARK_POS|GROUPED)

typedef union	u_varg
{
	uintmax_t	i;
	long double	f;
	void		*p;
}				t_varg;

typedef struct	s_fmt
{
	uint32_t	fl;
	int			w;
	int			p;
	char		*beg;
	char		*end;
	int			type;
	const char	*pref;
	int			pl;
	char		xp;
	wchar_t		wc[2];
	uint8_t		done;
}				t_fmt;

typedef struct	s_pad
{
	char		c;
	int			width;
	size_t		len;
	int			flags;
}				t_pad;

extern int		iofmt_parse(t_fmt *f, char **sp, va_list ap);
extern int		iofmt_poptype(t_varg *arg, int *type, char **s, va_list ap);
extern size_t	iofmt_out(t_stream *f, const char *s, size_t l);
extern void		iofmt_pad(t_stream *f, t_pad pad);
extern int		iofmt_eval(int t, t_fmt f, t_varg a, t_stream *s);

#endif
