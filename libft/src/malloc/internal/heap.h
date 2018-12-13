/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/heap.h                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_HEAP_H
# define MALLOC_INTERNAL_HEAP_H

# include "internal.h"

# include <stdbool.h>

# define MIN_ALLOC (128)

struct				s_mmap
{
	void			*mem;
	size_t			size;
};

struct				s_blk
{
	struct s_bin	*bins;
	uint16_t		nbyte;
	uint64_t		freemask[MIN_ALLOC / (sizeof(uint64_t) * 8)];
};

struct				s_bin
{
	struct s_blk	*blk;
	struct s_bin	*next;

	struct s_mmap	mmap;

	uint64_t		binmap[MIN_ALLOC / (sizeof(uint64_t) * 8)];
	uint64_t		ptrmap[MIN_ALLOC / (sizeof(uint64_t) * 8)];
};

int					bin_init(struct s_bin *bin, struct s_blk *blk);
int					bin_free(struct s_mpool *heap, struct s_bin **pbin,
						void *ptr, void *mem);
struct s_bin		**bin_bymem(struct s_mpool *heap, void *mem);
struct s_bin		*bin_find(struct s_mpool *heap, struct s_blk *blk,
						uint8_t *ppos);

struct				s_lrg
{
	struct s_lrg	*next;
	struct s_mmap	mmap;
};

int					lrg_init(struct s_lrg *lrg, size_t sz);
int					lrg_free(struct s_mpool *heap, struct s_lrg **plrg);
void				*lrg_alloc(struct s_mpool *heap, size_t sz);
struct s_lrg		**lrg_bymem(struct s_mpool *heap, void *mem);

struct				s_mpool
{
	uint8_t			mid;
	struct s_blk	blks[12];
	struct s_bin	*free_bins;
	struct s_lrg	*free_lrgs;
	struct s_lrg	*lrgs;
};

#endif
