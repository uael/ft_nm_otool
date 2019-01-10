/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/stdio.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_STDIO_H
# define FT_STDIO_H

# include "string.h"

# include <stdarg.h>
# include <unistd.h>

# define FT_EOF (-1)

struct s_stream;

typedef size_t	(t_write)(struct s_stream *, const uint8_t *, size_t);

typedef struct	s_stream
{
	int			fd;
	int8_t		lbf;
	int8_t		mode;
	int8_t		flags;
	int8_t		lock;
	void		*cookie;
	t_write		*write;
	size_t		buf_size;
	uint8_t		*buf;
	uint8_t		*rpos;
	uint8_t		*rend;
	uint8_t		*wpos;
	uint8_t		*wbase;
	uint8_t		*wend;
}				t_stream;

extern t_stream	*g_stdout;
extern t_stream	*g_stderr;

extern size_t	ft_fwrite(t_stream *f, void const *src, size_t n, size_t isz);
extern int		ft_fflush(t_stream *f);
extern void		ft_fflushstd(void);

extern int		ft_asprintf(char **s, char const *fmt,
					...) __attribute__((format(printf,2,3)));
extern int		ft_dprintf(int fd, char const *fmt,
					...) __attribute__((format(printf,2,3)));
extern int		ft_fprintf(t_stream *f, char const *fmt,
					...) __attribute__((format(printf,2,3)));
extern int		ft_printf(char const *fmt,
					...) __attribute__((format(printf,1,2)));
extern int		ft_snprintf(char *s, size_t n, char const *fmt,
					...) __attribute__((format(printf,3,4)));
extern int		ft_sprintf(char *s, char const *fmt,
					...) __attribute__((format(printf,2,3)));

extern int		ft_vasprintf(char **s, char const *fmt, va_list ap);
extern int		ft_vdprintf(int fd, char const *fmt, va_list ap);
extern int		ft_vfprintf(t_stream *f, char const *fmt, va_list ap);
extern int		ft_vprintf(char const *fmt, va_list ap);
extern int		ft_vsnprintf(char *s, size_t n, char const *fmt, va_list ap);
extern int		ft_vsprintf(char *s, char const *fmt, va_list ap);

#endif
