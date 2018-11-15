/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/fflush.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

#include "internal.h"

void	ft_fflushstd(void)
{
	ft_fflush(g_stderr);
	ft_fflush(g_stdout);
	ft_fflush(g_stdin);
}

int		ft_fflush(t_stream *f)
{
	if (f->wpos > f->wbase)
	{
		f->write(f, 0, 0);
		if (!f->wpos)
			return (EOF);
	}
	f->wpos = 0;
	f->wbase = 0;
	f->wend = 0;
	f->rpos = 0;
	f->rend = 0;
	return (0);
}
