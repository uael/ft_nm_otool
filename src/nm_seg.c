/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm_seg.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

#include <ft/string.h>

#include <errno.h>

static int			sects_update(struct s_nm_context *const ctx,
						struct section const *const sect)
{
	if (sect == NULL)
		return (NM_E_INVAL_SECTION);
	if (ft_strcmp("__text", sect->sectname) == 0)
		ctx->sects[ctx->nsects++] = 't';
	else if (ft_strcmp("__data", sect->sectname) == 0)
		ctx->sects[ctx->nsects++] = 'd';
	else if (ft_strcmp("__bss", sect->sectname) == 0)
		ctx->sects[ctx->nsects++] = 'b';
	else
		ctx->sects[ctx->nsects++] = 's';
	return (0);
}

int					segment_collect(t_obj const o, size_t off,
						void *const user)
{
	static size_t const					sect_sz = sizeof(struct section);
	struct s_nm_context *const			ctx = user;
	struct segment_command const *const	se = obj_peek(o, off, sizeof(*se));
	uint32_t							nsects;
	int									err;

	if (se == NULL)
		return (NM_E_INVAL_SEGMENT);
	if ((se->cmd == LC_SEGMENT_64) != o->m64)
	{
		errno = EBADARCH;
		return (NM_E_INVAL_ARCH);
	}
	off += sizeof(*se);
	nsects = obj_swap32(o, se->nsects);
	if (ctx->nsects + nsects >= COUNT_OF(ctx->sects))
	{
		errno = EBADMACHO;
		return (NM_E_INVAL_SECTION_COUNT);
	}
	err = 0;
	while (nsects-- && !(err = sects_update(ctx, obj_peek(o, off, sect_sz))))
		off += sect_sz;
	return (err);
}

int					segment_64_collect(t_obj const o, size_t off,
						void *const user)
{
	static size_t const						sect_sz = sizeof(struct section_64);
	struct s_nm_context *const				ctx = user;
	struct segment_command_64 const *const	se = obj_peek(o, off, sizeof(*se));
	uint32_t								nsects;
	int										err;

	if (se == NULL)
		return (NM_E_INVAL_SEGMENT);
	if ((se->cmd == LC_SEGMENT_64) != o->m64)
	{
		errno = EBADARCH;
		return (NM_E_INVAL_ARCH);
	}
	off += sizeof(*se);
	nsects = obj_swap32(o, se->nsects);
	if (ctx->nsects + nsects >= COUNT_OF(ctx->sects))
	{
		errno = EBADMACHO;
		return (NM_E_INVAL_SECTION_COUNT);
	}
	err = 0;
	while (nsects-- && !(err = sects_update(ctx, obj_peek(o, off, sect_sz))))
		off += sect_sz;
	return (err);
}
