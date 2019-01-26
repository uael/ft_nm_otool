/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   otool.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <ft/ctype.h>
#include <ft/opts.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>

static inline bool	dump_bytebybyte(t_obj const o)
{
	return (o->arch_info == NULL
		|| o->arch_info->cputype == CPU_TYPE_I386
		|| o->arch_info->cputype == CPU_TYPE_X86_64);
}

static inline int	dump(t_obj const o, uint64_t const addr,
						uint64_t const off, uint64_t const size)
{
	bool const	usual = dump_bytebybyte(o);
	uint64_t	i;
	uint64_t	j;
	char const	*txt;

	if ((txt = obj_peek(o, off, size)) == NULL)
		return (-1);
	ft_printf("Contents of (__TEXT,__text) section\n");
	i = 0;
	while (i < size)
	{
		ft_printf("%0*llx\t", o->m64 ? 16 : 8, addr + i);
		j = 0;
		if (usual)
		{
			while (j < 0x10 && i + j < size)
				ft_printf("%02hhx ", txt[i + j++]);
		}
		else
			while (j < 0x04 && i + (j * 4) < size)
				ft_printf("%08x ", obj_swap32(o, ((uint32_t *)(txt + i))[j++]));
		ft_printf("\n");
		i += 0x10;
	}
	return (0);
}

int					segment_collect(t_obj const o, size_t off, void *const user)
{
	struct segment_command const *const	se = obj_peek(o, off, sizeof(*se));
	struct section const				*sect;
	uint32_t							nsects;

	(void)user;
	if (se == NULL || (se->cmd == LC_SEGMENT_64) != o->m64)
		return (((errno = EBADARCH) & 0) - 1);
	off += sizeof(*se) - sizeof(*sect);
	nsects = obj_swap32(o, se->nsects);
	while (nsects--)
		if ((sect = obj_peek(o, off += sizeof(*sect), sizeof(*sect))) == NULL)
			return (-1);
		else if (ft_strcmp("__TEXT", sect->segname) == 0
			&& ft_strcmp("__text", sect->sectname) == 0)
		{
			return (dump(o, obj_swap32(o, sect->addr),
				obj_swap32(o, sect->offset), obj_swap32(o, sect->size)));
		}
	return (0);
}

int					segment_64_collect(t_obj const o, size_t off,
						void *const user)
{
	struct segment_command_64 const *const	se = obj_peek(o, off, sizeof(*se));
	struct section_64 const					*sect;
	uint32_t								nsects;

	(void)user;
	if (se == NULL || (se->cmd == LC_SEGMENT_64) != o->m64)
		return (((errno = EBADARCH) & 0) - 1);
	off += sizeof(*se) - sizeof(*sect);
	nsects = obj_swap32(o, se->nsects);
	while (nsects--)
		if ((sect = obj_peek(o, off += sizeof(*sect), sizeof(*sect))) == NULL)
			return (-1);
		else if (ft_strcmp("__TEXT", sect->segname) == 0
			&& ft_strcmp("__text", sect->sectname) == 0)
		{
			return (dump(o, obj_swap64(o, sect->addr),
				obj_swap64(o, sect->offset), obj_swap64(o, sect->size)));
		}
	return (0);
}
