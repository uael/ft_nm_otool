/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fwrite.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>
#include <stdio.h>

#include "internal.h"

static void	flushinit(t_stream *s)
{
	static int		init = 0;
	static t_stream	*last = NULL;

	if (!init)
	{
		atexit(ft_fflushstd);
		init = 1;
	}
	if (s == g_stdin || s == g_stdout || s == g_stderr)
	{
		if (last && last != s)
			ft_fflush(last);
		last = s;
	}
}

static int	towrite(t_stream *f)
{
	flushinit(f);
	if (f->wend)
		return (0);
	f->mode |= f->mode - 1;
	if (f->flags & FT_FNOWR)
	{
		f->flags |= FT_FERRO;
		return (EOF);
	}
	f->rpos = 0;
	f->rend = 0;
	f->wpos = f->buf;
	f->wbase = f->buf;
	f->wend = f->buf + f->buf_size;
	return (0);
}

size_t		fwritex(t_stream *f, uint8_t const *s, size_t l)
{
	size_t i;
	size_t n;

	i = 0;
	if (towrite(f))
		return (0);
	if (l > (size_t)(f->wend - f->wpos))
		return (f->write(f, s, l));
	if (f->lbf >= 0)
	{
		i = l;
		while (i && s[i - 1] != '\n')
			--i;
		if (i)
		{
			n = f->write(f, s, i);
			if (n < i)
				return (n);
			s += i;
			l -= i;
		}
	}
	ft_memcpy(f->wpos, s, l);
	f->wpos += l;
	return (l + i);
}

size_t		ft_fwrite(t_stream *f, void const *src, size_t isz, size_t n)
{
	size_t k;
	size_t l;

	l = isz * n;
	if (!isz)
		n = 0;
	k = fwritex(f, src, l);
	return (k == l ? n : k / isz);
}
