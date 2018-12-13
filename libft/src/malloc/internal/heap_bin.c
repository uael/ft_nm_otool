/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/heap_bin.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

#include <sys/mman.h>

#define BITS (sizeof(uint64_t) * BITSPERBYTE)

int					bin_init(struct s_bin *bin, struct s_blk *blk)
{
	uint8_t			i;

	bin->mmap.mem = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (bin->mmap.mem == MAP_FAILED)
		return (-errno);
	bin->blk = blk;
	bin->mmap.size = PAGE_SIZE;
	i = 0;
	while (i < COUNT_OF(bin->binmap))
	{
		bin->ptrmap[i] = 0x00;
		bin->binmap[i] = blk->freemask[i];
		++i;
	}
	return (0);
}

int					bin_free(struct s_mpool *heap, struct s_bin **pbin,
						void *ptr, void *mem)
{
	struct s_bin	*bin;
	uint8_t			i;
	uint8_t			pos;

	bin = *pbin;
	pos = (uint8_t)((ptr - mem) / bin->blk->nbyte);
	i = 0;
	while (pos > BITS)
	{
		++i;
		pos -= BITS;
	}
	if (!(bin->ptrmap[i] & (1 << pos)))
		return (-(errno = EINVAL));
	if ((bin->binmap[i] & (1 << pos)))
		return (-(errno = EINVAL));
	bin->binmap[i] |= +(1 << pos);
	bin->ptrmap[i] &= ~(1 << pos);
	if (bin->blk->bins->next
		&& ft_memcmp(bin->binmap, bin->blk->freemask, sizeof(bin->binmap)) == 0)
	{
		munmap(bin->mmap.mem, bin->mmap.size);
		*pbin = bin->next;
		bin->next = heap->free_bins;
		heap->free_bins = bin;
	}
	return (0);
}

struct s_bin		**bin_bymem(struct s_mpool *heap, void *mem)
{
	struct s_blk	*blk;
	struct s_bin	**phd;
	struct s_bin	*hd;
	uint8_t			i;

	i = 0;
	while (i < COUNT_OF(heap->blks))
	{
		blk = heap->blks + i;
		phd = &blk->bins;
		hd = blk->bins;
		while (hd)
		{
			if (hd->mmap.mem == mem)
				return (phd);
			phd = &hd->next;
			hd = hd->next;
		}
		++i;
	}
	return (NULL);
}

static struct s_bin	*bin_used(struct s_bin *bin, uint8_t *ppos)
{
	if (bin == NULL)
		return (NULL);
	*ppos = 0;
	while (*ppos < COUNT_OF(bin->binmap))
	{
		if (bin->binmap[*ppos])
			return (bin);
		++*ppos;
	}
	return (bin_used(bin->next, ppos));
}

struct s_bin		*bin_find(struct s_mpool *heap, struct s_blk *blk,
								uint8_t *ppos)
{
	size_t			i;
	struct s_bin	*bin;

	if ((bin = bin_used(blk->bins, ppos)))
		return (bin);
	if (!heap->free_bins)
	{
		bin = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (bin == MAP_FAILED)
			return (NULL);
		i = 0;
		while ((i += sizeof(struct s_bin)) < PAGE_SIZE)
		{
			*bin = (struct s_bin){ };
			bin->next = heap->free_bins;
			heap->free_bins = bin;
			++bin;
		}
	}
	bin = heap->free_bins;
	heap->free_bins = bin->next;
	bin->next = blk->bins;
	*ppos = 0;
	return (bin_init(bin, blk) ? NULL : (blk->bins = bin));
}
