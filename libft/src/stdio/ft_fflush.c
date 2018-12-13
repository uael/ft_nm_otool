/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stdio/ft_fflush.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/stdio.h"

void	ft_fflushstd(void)
{
	ft_fflush(g_stderr);
	ft_fflush(g_stdout);
}

int		ft_fflush(t_stream *f)
{
	if (f->wpos > f->wbase)
	{
		f->write(f, 0, 0);
		if (!f->wpos)
			return (FT_EOF);
	}
	f->wpos = 0;
	f->wbase = 0;
	f->wend = 0;
	f->rpos = 0;
	f->rend = 0;
	return (0);
}
