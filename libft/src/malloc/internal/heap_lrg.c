/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/heap_lrg.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heap.h"

#include <sys/mman.h>

int					lrg_init(struct s_lrg *lrg, size_t sz)
{
	sz = (sz + PAGE_SIZE - 1) & -PAGE_SIZE;
	lrg->mmap.mem = mmap(NULL, sz, PROT_READ | PROT_WRITE,
						 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (lrg->mmap.mem == MAP_FAILED)
		return (-errno);
	lrg->mmap.size = sz;
	return (0);
}

int					lrg_free(struct s_mpool *heap, struct s_lrg **plrg)
{
	struct s_lrg	*lrg;

	lrg = *plrg;
	munmap(lrg->mmap.mem, lrg->mmap.size);
	*plrg = lrg->next;
	lrg->next = heap->free_lrgs;
	heap->free_lrgs = lrg;
	return (0);
}

void				*lrg_alloc(struct s_mpool *heap, size_t sz)
{
	size_t			i;
	struct s_lrg	*lrg;

	if (!heap->free_lrgs) {
		lrg = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
				   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (lrg == MAP_FAILED)
			return (NULL);
		i = 0;
		while ((i += sizeof(struct s_bin)) < PAGE_SIZE)
		{
			*lrg = (struct s_lrg){ };
			lrg->next = heap->free_lrgs;
			heap->free_lrgs = lrg;
			++lrg;
		}
	}
	lrg = heap->free_lrgs;
	heap->free_lrgs = lrg->next;
	lrg->next = heap->lrgs;
	heap->lrgs = lrg;
	return (lrg_init(lrg, sz) ? NULL : lrg->mmap.mem);
}

struct s_lrg		**lrg_bymem(struct s_mpool *heap, void *mem)
{
	struct s_lrg	*hd;
	struct s_lrg	**phd;

	phd = &heap->lrgs;
	hd = heap->lrgs;
	while (hd)
	{
		if (hd->mmap.mem == mem)
			return (phd);
		phd = &hd->next;
		hd = hd->next;
	}
	return (NULL);
}
