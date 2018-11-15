/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft/io.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/17 10:01:44 by null             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_IO_H
# define LIBFT_IO_H

# include <stdarg.h>
# ifdef _MSC_VER
#  include <io.h>
# else
#  include <unistd.h>
# endif
# include <fcntl.h>

# include "int.h"
# include "str.h"
# include "ex.h"
# include "ds.h"

# ifndef FT_PAGE_SIZE
#  if defined PAGE_SIZE && PAGE_SIZE <= 4096
#   define FT_PAGE_SIZE PAGE_SIZE
#  elif defined PAGESIZE && PAGESIZE <= 4096
#   define FT_PAGE_SIZE PAGESIZE
#  else
#   define FT_PAGE_SIZE 4096
#  endif
# endif

# ifndef STDIN_FILENO
#  define STDIN_FILENO 0
# endif
# ifndef STDOUT_FILENO
#  define STDOUT_FILENO 1
# endif
# ifndef STDERR_FILENO
#  define STDERR_FILENO 2
# endif

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

extern t_stream	*g_stdin;
extern t_stream	*g_stdout;
extern t_stream	*g_stderr;

extern ssize_t	ft_read(int fd, void *buf, size_t sz);
extern ssize_t	ft_write(int fd, void const *buf, size_t sz);
extern int		ft_getln(int fd, char **line);
extern int		ft_getsln(int fd, char **line, t_sds *sv);

extern t_stream	*ft_fopen(char const *filename, int flags, char *buf, size_t s);
extern int		ft_fclose(t_stream *s);
extern size_t	ft_fwrite(t_stream *f, void const *src, size_t isz, size_t n);
extern int		ft_fputs(t_stream *f, char const *str);
extern int		ft_fputc(t_stream *f, int ch);
extern int		ft_fflush(t_stream *f);
extern void		ft_fflushstd(void);

extern int		ft_asprintf(char **s, char const *fmt, ...);
extern int		ft_dprintf(int fd, char const *fmt, ...);
extern int		ft_fprintf(t_stream *f, char const *fmt, ...);
extern int		ft_printf(char const *fmt, ...);
extern int		ft_snprintf(char *s, size_t n, char const *fmt, ...);
extern int		ft_sprintf(char *s, char const *fmt, ...);
extern int		ft_vasprintf(char **s, char const *fmt, va_list ap);
extern int		ft_vdprintf(int fd, char const *fmt, va_list ap);
extern int		ft_vfprintf(t_stream *f, char const *fmt, va_list ap);
extern int		ft_vprintf(char const *fmt, va_list ap);
extern int		ft_vsnprintf(char *s, size_t n, char const *fmt, va_list ap);
extern int		ft_vsprintf(char *s, char const *fmt, va_list ap);

typedef struct	s_ifs
{
	int			ifd;
	size_t		i;
	size_t		rd;
	ssize_t		lim;
	t_bool		print : 1;
	char		*buf;
	char		stack[FT_PAGE_SIZE + 1];
}				t_ifs;

extern void		ft_ifsctor(t_ifs *self, int ifd);
extern void		ft_ifsdtor(t_ifs *self);
extern int		ft_ifsopen(t_ifs *self, char const *filename);
extern int		ft_ifsclose(t_ifs *self);
extern char		ft_ifspeek(t_ifs *self, size_t i);
extern ssize_t	ft_ifsbuf(t_ifs *self, size_t sz, char **out);
extern ssize_t	ft_ifschr(t_ifs *self, size_t off, char c, char **out);
extern ssize_t	ft_ifsrd(t_ifs *s, void *b, size_t n);

#endif
