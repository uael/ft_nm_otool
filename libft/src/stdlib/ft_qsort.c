/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string/ft_shsort.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/ctype.h"
#include "ft/stdlib.h"
#include "ft/string.h"

#include <errno.h>
#include <stdlib.h>

static void	swap(void *p1, void *p2, void *tmp, size_t width)
{
	char	*s1;
	char	*s2;

	s1 = (char *)p1;
	s2 = (char *)p2;
	ft_memcpy(tmp, s1, width);
	ft_memcpy(s1, s2, width);
	ft_memcpy(s2, tmp, width);
}

void		ft_qsort(void *base, size_t nel, size_t width, t_ncmp *cmp)
{
	char	*cbase;
	uint8_t tmp[width];
	size_t	i;
	size_t	j;

	if (nel <= 1)
		return ;
	cbase = (char *)base;
	i = 1;
	j = 1;
	while (i < nel)
	{
		if (cmp(cbase, cbase + (i * width), width) > 0)
		{
			swap(cbase + (i * width), cbase + (j * width), tmp, width);
			j++;
		}
		i++;
	}
	swap(cbase, cbase + ((j - 1) * width), tmp, width);
	ft_qsort(cbase, j - 1, width, cmp);
	ft_qsort(cbase + (j * width), nel - j, width, cmp);
}
