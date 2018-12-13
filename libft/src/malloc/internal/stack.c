/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/stack.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "stack.h"

static int			stack_init(struct s_mpool *stack, void *_conf)
{
	const struct s_stack_conf	*conf = _conf;
	uint16_t					tail;
	
	if (!conf)
		return (-(errno = EINVAL));
	if (!conf->mem)
		return (-(errno = EINVAL));
	tail = (uint16_t)(conf->size / sizeof(struct s_chunk));
	if (tail < 1)
		return (-(errno = EINVAL));
	stack->size = conf->size;
	stack->head = conf->mem;
	stack->tail = stack->head + tail;
	*stack->tail = (struct s_chunk){ .off = tail, .rfc = 1 };
	*stack->head = (struct s_chunk){ .nxt = tail };
	return (0);
}

static void			trim(struct s_mpool *stack, struct s_chunk *chk, size_t sz)
{
	struct s_chunk	*nxt;
	struct s_chunk	*nxt_nxt;

	nxt = (struct s_chunk *)(chunk_mem(chk) + sz);
	*nxt = (struct s_chunk){ .nxt = chk->nxt, .prv = chk->off,
		.off = (uint16_t)(nxt - stack->head) };
	nxt_nxt = chunk_nxt(nxt, stack);
	if (!nxt_nxt->rfc)
		chunk_nxt(nxt_nxt, stack)->prv = nxt->off;
	else
		nxt_nxt->prv = nxt->off;
	chk->nxt = nxt->off;
}

static void			*stack_alloc(struct s_mpool *stack, size_t sz)
{
	struct s_chunk	*chk;

	if (!sz)
		return (NULL);
	sz = (sz + sizeof(struct s_chunk) - 1) & sizeof(struct s_chunk);
	chk = stack->head;
	while (chk->rfc || chunk_size(chk) < sz)
	{
		if (chk->nxt == stack->tail->off)
		{
			errno = ENOMEM;
			return (NULL);
		}
		chk = chunk_nxt(chk, stack);
	}
	if (chunk_size(chk) >= sz + sizeof(struct s_chunk))
		trim(stack, chk, sz);
	chk->rfc = 1;
	return ((void *)chunk_mem(chk));
}

static void			*stack_realloc(struct s_mpool *pool, void *ptr, size_t sz)
{
	(void)pool;
	(void)ptr;
	(void)sz;
	return (NULL);
}

static int			stack_free(struct s_mpool *stack, void *ptr)
{
	struct s_chunk *chk;
	struct s_chunk *prv;
	struct s_chunk *nxt;

	if (!ptr)
		return (-(errno = EINVAL));
	chk = (struct s_chunk *)ptr - 1;
	if (!chk->rfc || --chk->rfc)
		return (chk->rfc);
	prv = chunk_prv(chk, stack);
	nxt = chunk_nxt(chk, stack);
	if ((prv != chk && !prv->rfc) || !nxt->rfc)
	{
		if (!nxt->rfc)
			nxt = chunk_nxt(nxt, stack);
		if (!prv->rfc)
			chk = prv;
		chk->nxt = nxt->off;
		nxt->prv = chk->off;
	}
	return (0);
}

static struct s_mpool			g_stacks[FT_MALLOC_MAX_MPOOL];
static const struct s_mpool_def g_stack_stk = {
	.mpool_size = sizeof(struct s_mpool),
	.pools = g_stacks,
	.ops = {
		.init = stack_init,
		.fini = NULL,
		.alloc = stack_alloc,
		.realloc = stack_realloc,
		.free = stack_free,
	}
};

const struct s_mpool_def *g_stack_def = &g_stack_stk;
