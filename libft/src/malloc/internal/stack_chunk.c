/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*    malloc/internal/stack_chunk.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "stack.h"

inline uintptr_t		chunk_mem(struct s_chunk *chunk)
{
	return ((uintptr_t)(chunk + 1));
}

inline size_t			chunk_size(struct s_chunk *chunk)
{
	return ((chunk->nxt - chunk->off - 1) * sizeof(struct s_chunk));
}

inline struct s_chunk	*chunk_nxt(struct s_chunk *chunk, struct s_mpool *bin)
{
	return (bin->head + chunk->nxt);
}

inline struct s_chunk	*chunk_prv(struct s_chunk *chunk, struct s_mpool *bin)
{
	return (bin->head + chunk->prv);
}
