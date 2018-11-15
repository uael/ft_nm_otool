/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/07 09:52:33 by alucas-           #+#    #+#             */
/*   Updated: 2017/11/07 09:53:34 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ds.h"

static inline char	hswap(t_map *self, char *key, char *val, uint32_t i)
{
	char tmp[self->ksz > self->vsz ? self->ksz : self->vsz];

	if (i < self->cap && !(self->bucks[i] & BUCKET_BOTH))
	{
		ft_memcpy(tmp, ((char *)self->keys + (i * self->ksz)), self->ksz);
		ft_memcpy(((char *)self->keys + (i * self->ksz)), key, self->ksz);
		ft_memcpy(key, tmp, self->ksz);
		ft_memcpy(tmp, ((char *)self->vals + (i * self->vsz)), self->vsz);
		ft_memcpy(((char *)self->vals + (i * self->vsz)), val, self->vsz);
		ft_memcpy(val, tmp, self->vsz);
		self->bucks[i] |= BUCKET_DELETED;
	}
	else
	{
		ft_memcpy((char *)self->keys + (i * self->ksz), key, self->ksz);
		ft_memcpy((char *)self->vals + (i * self->vsz), val, self->vsz);
		return (1);
	}
	return (0);
}

static inline void	reh1(t_map *self, uint32_t sz, uint8_t *bucks, uint32_t j)
{
	char		key[self->ksz];
	char		val[self->vsz];
	uint32_t	k;
	uint32_t	i;
	uint32_t	step;

	step = 0;
	ft_memcpy(key, ((char *)self->keys + (j * self->ksz)), self->ksz);
	ft_memcpy(val, ((char *)self->vals + (j * self->vsz)), self->vsz);
	self->bucks[j] |= BUCKET_DELETED;
	while (1)
	{
		k = self->hasher.hash(*(char **)key);
		i = k & (sz - 1);
		while ((bucks[i] & BUCKET_EMPTY) != BUCKET_EMPTY)
			i = (i + (++step)) & (sz - 1);
		bucks[i] &= ~BUCKET_EMPTY;
		if (hswap(self, key, val, i))
			break ;
	}
}

static inline void	reh(t_map *self, uint32_t sz, uint8_t *bucks)
{
	uint32_t j;

	j = 0;
	while (j != self->cap)
	{
		if (!(self->bucks[j] & BUCKET_BOTH))
			reh1(self, sz, bucks, (uint32_t)j);
		++j;
	}
	if (self->cap > sz)
	{
		self->keys = ft_realloc(self->keys, sz * self->ksz, sz * self->ksz);
		self->vals = ft_realloc(self->vals, sz * self->vsz, sz * self->vsz);
	}
	free(self->bucks);
	self->bucks = bucks;
	self->cap = sz;
	self->occupieds = self->len;
	self->upper_bound = (uint32_t)(self->cap * MAP_HASH_UPPER + 0.5);
}

size_t				ft_maprsz(t_map *self, uint32_t sz)
{
	uint8_t		*bucks;
	uint32_t	j;

	j = 1;
	bucks = NULL;
	if ((sz = pow2_next32(sz)) < 32)
		sz = 32;
	if (self->len >= (uint32_t)(sz * MAP_HASH_UPPER + 0.5))
		j = 0;
	else if ((bucks = (uint8_t *)ft_malloc(sz)))
	{
		ft_memset(bucks, BUCKET_EMPTY, sz);
		if (self->cap < sz)
		{
			self->keys = ft_realloc(self->keys, self->cap * self->ksz,
									sz * self->ksz);
			self->vals = ft_realloc(self->vals, self->cap * self->vsz,
									sz * self->vsz);
		}
	}
	if (j)
		reh(self, sz, bucks);
	return (self->cap);
}
