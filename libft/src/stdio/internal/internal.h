/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/internal/internal.h                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STDIO_INTERNAL_INTERNAL_H
# define STDIO_INTERNAL_INTERNAL_H

# include "ft/ctype.h"
# include "ft/stdio.h"
# include "ft/stdlib.h"

# include <errno.h>
# include <limits.h>

# define FT_FPERM (1 << 0)
# define FT_FNORD (1 << 1)
# define FT_FERRO (1 << 2)
# define FT_FNOWR (1 << 3)

# define FT_BUFSIZ (4096)

size_t			fwritex(t_stream *f, uint8_t const *s, size_t l);
size_t			stdiowrite(t_stream *f, uint8_t const *s, size_t len);

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
	double		f;
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

int				iofmt_parse(t_fmt *f, char **sp, va_list ap);
int				iofmt_poptype(t_varg *arg, int *type, char **s, va_list ap);
size_t			iofmt_out(t_stream *f, const char *s, size_t l);
void			iofmt_pad(t_stream *f, t_pad pad);
int				iofmt_eval(int t, t_fmt f, t_varg a, t_stream *s);

enum {
	T_BARE, T_LPRE, T_LLPRE, T_HPRE, T_HHPRE, T_BIGLPRE,
	T_ZTPRE, T_JPRE,
	T_STOP,
	T_PTR, T_INT, T_UINT, T_ULLONG,
	T_LONG, T_ULONG,
	T_SHORT, T_USHORT, T_CHAR, T_UCHAR,
	T_LLONG, T_SIZET, T_IMAX, T_UMAX, T_PDIFF, T_UIPTR,
	T_DBL, T_LDBL,
	T_NOARG,
	T_MAXSTATE
};

typedef ssize_t	(t_fmtcb)(t_stream *s, t_fmt *f, t_varg arg);

ssize_t			iofmt_fmtxp(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtpct(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmts(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtsu(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtp(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtx(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmto(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtdi(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtu(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtc(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtcu(t_stream *s, t_fmt *f, t_varg arg);
ssize_t			iofmt_fmtm(t_stream *s, t_fmt *f, t_varg arg);

#endif
