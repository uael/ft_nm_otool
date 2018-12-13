/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/heap.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

#define BITS (sizeof(uint64_t) * 8)

static int			heap_init(struct s_mpool *heap, void *conf)
{
	uint8_t			i;
	uint16_t		j;
	uint8_t			k;
	uint8_t			n;

	i = 0;
	(void)conf;
	while (i < COUNT_OF(heap->blks))
	{
		heap->blks[i].bins = NULL;
		heap->blks[i].nbyte = (uint16_t)(1 << i);
		k = 0;
		j = (uint16_t)(PAGE_SIZE / (1 << i));
		while (k < COUNT_OF(heap->blks->freemask))
		{
			n = (uint8_t)(j > BITS ? BITS : j);
			heap->blks[i].freemask[k++] =
				n == 64 ? 0xffffffffffffffff : ~(~0ULL << n);
			j = (uint16_t)(j > BITS ? j - BITS : 0);
		}
		++i;
	}
	return (0);
}

static void			heap_fini(struct s_mpool *pool)
{
	(void)pool;
}

static void			*heap_alloc(struct s_mpool *heap, size_t sz)
{
	struct s_blk	*blk;
	struct s_bin	*bin;
	uint64_t		bit;
	uint8_t			pos;

	if (sz == 0)
		return (NULL);
	if (sz > 2048)
		return (lrg_alloc(heap, sz));
	blk = heap->blks + (31 - __builtin_clz(sz));
	bin = bin_find(heap, blk, &pos);
	if (bin == NULL)
		return (NULL);
	bit = __builtin_ctzll(bin->binmap[pos]);
	bin->binmap[pos] &= ~(1 << bit);
	bin->ptrmap[pos] |= +(1 << bit);
	pos *= BITS;
	pos += bit;
	return (bin->mmap.mem + (blk->nbyte * pos));
}

static void			*heap_realloc(struct s_mpool *pool, void *ptr, size_t sz)
{
	(void)pool;
	(void)ptr;
	(void)sz;
	return (NULL);
}

static int			heap_free(struct s_mpool *heap, void *ptr)
{
	void			*mem;
	struct s_lrg	**plrg;
	struct s_bin	**pbin;

	if (ptr == NULL)
		return (0);
	if ((plrg = lrg_bymem(heap, ptr)))
		return (lrg_free(heap, plrg));
	mem = (void *)((uintptr_t)ptr & -PAGE_SIZE);
	if ((pbin = bin_bymem(heap, mem)))
		return (bin_free(heap, pbin, ptr, mem));
	return (0);
}

static struct s_mpool			g_heaps[FT_MALLOC_MAX_MPOOL];
static const struct s_mpool_def g_heap_stk = {
	.mpool_size = sizeof(struct s_mpool),
	.pools = g_heaps,
	.ops = {
		.init = heap_init,
		.fini = heap_fini,
		.alloc = heap_alloc,
		.realloc = heap_realloc,
		.free = heap_free,
	}
};

const struct s_mpool_def *g_heap_def = &g_heap_stk;
