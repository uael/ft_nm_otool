/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/string.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include "ft/cdefs.h"

extern void	ft_free(void *ptr);
extern void *ft_memalign(size_t alignment, size_t size);
extern void	*ft_malloc(size_t size);
extern void	*ft_realloc(void *ptr, size_t size);
extern void	*ft_reallocf(void *ptr, size_t size);
extern void	*ft_calloc(size_t nmemb, size_t size);

typedef struct s_mpool			*t_mpool;

struct							s_mpool_hdr
{
	uint8_t						mid;
};

struct							s_mpool_ops
{
	int							(*init)(struct s_mpool *, void *);
	void						(*fini)(struct s_mpool *);
	void						*(*alloc)(struct s_mpool *, size_t);
	void						*(*realloc)(struct s_mpool *, void *, size_t);
	int							(*free)(struct s_mpool *, void *);
};

struct							s_mpool_def
{
	size_t						mpool_size;
	t_mpool						pools;
	const struct s_mpool_ops	ops;
};

extern t_mpool					g_heap;
extern const struct s_mpool_def *g_heap_def;
extern const struct s_mpool_def *g_stack_def;

struct							s_stack_conf
{
	void						*mem;
	size_t						size;
};

extern int						ft_mpool(const struct s_mpool_def *def,
									void *conf, t_mpool *pool);

extern void						*ft_palloc(t_mpool pool, size_t sz);
extern int						ft_pfree(t_mpool pool, void *ptr);

#endif
