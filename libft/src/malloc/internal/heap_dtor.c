/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/heap_dtor.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

#include <sys/mman.h>

void			heap_bins_dtor(struct s_heap *heap)
{
	//TODO
}

void			heap_lrgs_dtor(struct s_heap *heap)
{
	uint8_t			i;
	struct s_bin	*bin;
	struct s_bin	*next;
	struct s_bin	*mapped_bin;

	struct s_lrg	*lrg;
	struct s_lrg	*lrg_next;
	struct s_lrg	*mapped_lrg;

	i = 0;
	mapped_bin = NULL;
	while (i < COUNT_OF(heap->blks))
	{
		bin = heap->blks[i].bins;
		heap->blks[i].bins = NULL;
		while (bin)
		{
			next = bin->next;
			munmap(bin->mmap.mem, bin->mmap.size);
			++nb_free_pages;
			if (((uintptr_t)bin % PAGE_SIZE) == 0)
			{
				bin->next = mapped_bin;
				mapped_bin = bin;
			}
			bin = next;
		}
		++i;
	}
	bin = heap->free_bins;
	heap->free_bins = NULL;
	while (bin)
	{
		next = bin->next;

		if (((uintptr_t)bin % PAGE_SIZE) == 0)
		{
			bin->next = mapped_bin;
			mapped_bin = bin;
		}

		bin = next;
	}
	while (mapped_bin)
	{
		next = mapped_bin->next;
		munmap(bin, PAGE_SIZE);
		++nb_free_pages;
		mapped_bin = next;
	}

	mapped_lrg = NULL;
	lrg = heap->lrgs;
	while (lrg)
	{
		lrg_next = lrg->next;

		munmap(lrg->mmap.mem, lrg->mmap.size);
		++nb_free_pages;

		if (((uintptr_t)lrg % PAGE_SIZE) == 0)
		{
			lrg->next = mapped_lrg;
			mapped_lrg = lrg;
		}

		lrg = lrg_next;
	}
	lrg = heap->free_lrgs;
	while (lrg)
	{
		lrg_next = lrg->next;

		if (((uintptr_t)lrg % PAGE_SIZE) == 0)
		{
			lrg->next = mapped_lrg;
			mapped_lrg = lrg;
		}

		lrg = lrg_next;
	}
	while (mapped_lrg)
	{
		lrg_next = mapped_lrg->next;
		munmap(mapped_lrg, PAGE_SIZE);
		++nb_free_pages;
		mapped_lrg = lrg_next;
	}
}
