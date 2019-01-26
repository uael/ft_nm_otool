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

static inline void	dump(const char *const text, uint64_t const off,
						uint64_t const size, unsigned const padd)
{
	uint64_t i;
	uint64_t j;

	ft_printf("Contents of (__TEXT,__text) section\n");
	i = 0;
	while (i < size)
	{
		ft_printf("%0*llx\t", padd, off + i);
		j = 0;
		while (j < 0x10 && i + j < size)
			ft_printf("%02hhx ", text[i + j++]);
		ft_printf("\n");
		i += 0x10;
	}
}

int					segment_collect(t_obj const o, size_t off, void *const user)
{
	struct segment_command const *const	se = obj_peek(o, off, sizeof(*se));
	struct section const				*sect;
	uint32_t							nsects;
	uint32_t							size;
	char const							*text;

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
			size = obj_swap32(o, sect->size);
			if ((text = obj_peek(o, obj_swap32(o, sect->offset), size)) == NULL)
				return (-1);
			dump(text, obj_swap32(o, sect->addr), size, 8);
			break ;
		}
	return ((int)user & 0);
}

int					segment_64_collect(t_obj const o, size_t off,
						void *const user)
{
	struct segment_command_64 const *const	se = obj_peek(o, off, sizeof(*se));
	struct section_64 const					*sect;
	uint32_t								nsects;
	uint64_t								size;
	char const								*text;

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
			size = obj_swap64(o, sect->size);
			if ((text = obj_peek(o, obj_swap64(o, sect->offset), size)) == NULL)
				return (-1);
			dump(text, obj_swap64(o, sect->addr), size, 16);
			break ;
		}
	return ((int)user & 0);
}
