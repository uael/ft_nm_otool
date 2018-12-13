/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/stack.h                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_STACK_H
# define MALLOC_INTERNAL_STACK_H

# include "internal.h"

struct					s_chunk
{
	uint16_t			rfc;
	uint16_t			prv;
	uint16_t			off;
	uint16_t			nxt;
};

extern uintptr_t		chunk_mem(struct s_chunk *chunk);
extern size_t			chunk_size(struct s_chunk *chunk);
extern struct s_chunk	*chunk_nxt(struct s_chunk *chunk, struct s_mpool *pool);
extern struct s_chunk	*chunk_prv(struct s_chunk *chunk, struct s_mpool *pool);

struct					s_mpool
{
	uint8_t				mid;
	size_t				size: 56;
	struct s_chunk		*head;
	struct s_chunk		*tail;
};

#endif
