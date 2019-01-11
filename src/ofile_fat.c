/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_fat.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <ft/string.h>

#include <errno.h>

static size_t const	g_sizes[] = {
	[false] = sizeof(struct fat_arch),
	[true] = sizeof(struct fat_arch_64)
};

static NXArchInfo const	*fat_arch(struct s_obj const *const obj,
							void const *fat_archs, uint32_t i)
{
	struct fat_arch const	*arch;
	NXArchInfo const		*info;

	arch = obj->m64
		? (struct fat_arch *)((struct fat_arch_64 *)fat_archs + i)
		: (struct fat_arch *)fat_archs + i;
	info = NXGetArchInfoFromCpuType(
		(cpu_type_t)obj_swap32(obj, (uint32_t)arch->cputype),
		(cpu_subtype_t)obj_swap32(obj, (uint32_t)arch->cpusubtype));
	if (info == NULL)
	{
		errno = EBADMACHO;
		return (NULL);
	}
	return (info);
}

static int				fat_target(struct s_obj const *const obj,
							NXArchInfo const **target, void const *fat_archs,
							uint32_t nfat_arch)
{
	NXArchInfo const	*info;
	bool				find;
	uint32_t			i;

	i = 0;
	find = false;
	while (i < nfat_arch)
	{
		info = fat_arch(obj, fat_archs, i);
		if (info == NULL)
			return (OFILE_E_INVAL_ARCHINFO);
		if ((*target)->cputype == info->cputype
			&& (*target)->cpusubtype == info->cpusubtype)
			find = true;
		++i;
	}
	if (find == false && obj->target && obj->target != OFILE_NX_HOST)
		return (OFILE_E_NOTFOUND_ARCH);
	if (find == false)
		*target = OFILE_NX_ALL;
	return (0);
}

static int				fat_object(struct s_obj const *const obj,
							void const *fat_archs, uint32_t i,
							struct s_obj *new_obj)
{
	struct fat_arch_64 const	*arch_64;
	struct fat_arch const		*arch;
	uint64_t					arch_off;
	uint64_t					arch_size;

	if (obj->m64)
	{
		arch_64 = (struct fat_arch_64 *)fat_archs + i;
		arch_off = obj_swap64(obj, arch_64->offset);
		arch_size = obj_swap64(obj, arch_64->size);
	}
	else
	{
		arch = (struct fat_arch *)fat_archs + i;
		arch_off = obj_swap32(obj, arch->offset);
		arch_size = obj_swap32(obj, arch->size);
	}
	new_obj->size = arch_size;
	if ((new_obj->buf = obj_peek(obj, arch_off, arch_size)) == NULL)
		return (OFILE_E_INVAL_ARCHINFO);
	return (0);
}

int						fat_load(struct s_obj const *const obj,
							struct s_ofile_collector const *const collector,
							void *const user)
{
	struct fat_header const *const	header = obj_peek(obj, 0, sizeof(*header));
	struct s_obj					new_obj;
	uint32_t						idx[2];
	void const						*fat_archs;
	int								err;

	if (header == NULL)
		return (OFILE_E_INVAL_FATHDR);
	ft_memset(&new_obj, 0, sizeof(struct s_obj));
	new_obj.target = obj->target != OFILE_NX_HOST
		? obj->target : NXGetArchInfoFromName("x86_64");
	idx[0] = obj_swap32(obj, header->nfat_arch);
	fat_archs = obj_peek(obj, sizeof(*header), g_sizes[obj->m64] * idx[0]);
	if (fat_archs == NULL)
		return (OFILE_E_INVAL_FATARCH);
	if (new_obj.target != OFILE_NX_ALL
		&& (err = fat_target(obj, &new_obj.target, fat_archs, idx[0])))
		return (err);
	new_obj.ofile = OFILE_FAT;
	idx[1] = 0;
	while (idx[1] < idx[0])
		if ((err = fat_object(obj, fat_archs, idx[1]++, &new_obj))
			|| (err = load(&new_obj, collector, user)))
			return (err);
	return (0);
}
