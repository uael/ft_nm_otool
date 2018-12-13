/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/ft_mpool.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft/malloc.h"
#include "ft/string.h"

#include <errno.h>

#define DTOR __attribute__((__destructor__)) void

const struct s_mpool_ops	*g_ops[FT_MALLOC_MAX_MPOOL];
t_mpool						g_pools[FT_MALLOC_MAX_MPOOL];

int							ft_mpool(const struct s_mpool_def *def,
								void *conf, t_mpool *_pool)
{
	uint8_t					i;
	t_mpool					pool;

	i = 0;
	*_pool = NULL;
	while (i < COUNT_OF(g_ops))
	{
		if (g_ops[i] == NULL)
			break ;
		++i;
	}
	if (i == COUNT_OF(g_ops))
		return (-(errno = ENOMEM));
	g_ops[i] = &def->ops;
	pool = (t_mpool)((uintptr_t)def->pools + (i * def->mpool_size));
	ft_memset(pool, 0, def->mpool_size);
	((struct s_mpool_hdr *)pool)->mid = i;
	*_pool = pool;
	return (g_ops[i]->init(pool, conf));
}

static DTOR					dtor(void)
{
	uint8_t					i;

	(void)dtor;
	i = 0;
	while (i < COUNT_OF(g_ops))
	{
		if (g_ops[i])
		{
			if (g_ops[i]->fini)
				g_ops[i]->fini(g_pools[i]);
			g_ops[i] = NULL;
			g_pools[i] = NULL;
		}
		++i;
	}
}
