/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gnl.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:30 by cmalfroy          #+#    #+#             */
/*   Updated: 2017/12/06 12:00:10 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/io.h"
#include "libft/ds.h"

#ifndef OPEN_MAX
# define OPEN_MAX UINT8_MAX
#endif

int	ft_getsln(int const fd, char **line, t_sds *sv)
{
	int st;

	if ((st = ft_getln(fd, line)) == 1 && line && *line)
	{
		ft_sdsapd(sv, *line);
		ft_sdsapd(sv, "\n");
	}
	return (st);
}

int	ft_getln(int const fd, char **line)
{
	static t_sds	c[OPEN_MAX];
	char			b[FT_PAGE_SIZE + 1];
	ssize_t			i;
	char			*eol;

	eol = NULL;
	if (line && *line)
		free(*line);
	*line = NULL;
	if (!line || fd < 0 || fd > OPEN_MAX)
		return (-1);
	while ((!c[fd].len || (eol || !(eol = ft_strchr(c[fd].buf, '\n')))))
		if ((i = read(fd, b, FT_PAGE_SIZE)) == 0)
			break ;
		else if (i == -1 || !(eol = ft_sdsmpush(c + fd, b, (size_t)i)))
			return (-1);
		else if ((eol = ft_strchr(eol, '\n')))
			break ;
	if (!(i = eol ? (eol - c[fd].buf + 1) : (ssize_t)c[fd].len))
		return (ft_pfree((void **)&c[fd].buf));
	if (!(*line = malloc((size_t)(i + (eol ? 1 : 0)) * sizeof(char))))
		return (-1);
	(*line)[ft_sdsnsht(c + fd, (size_t)i, *line) - (eol ? 1 : 0)] = '\0';
	return (1);
}
