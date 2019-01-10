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

#include <errno.h>

static size_t const	g_sizes[] = {
	[false] = sizeof(struct fat_arch),
	[true] = sizeof(struct fat_arch_64)
};

static NXArchInfo const	*arch_info(struct s_obj const *const obj,
							uint32_t i, uint32_t nfat_arch,
							void const *fat_archs)
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

static int				target_update(struct s_obj const *const obj,
							NXArchInfo const **target, uint32_t nfat_arch,
							void const *fat_archs)
{

	bool		find;
	uint32_t	i;

	i = 0;
	find = false;
	while (i < nfat_arch)
	{
		info = arch_info(obj, i, nfat_arch, fat_archs);
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

static int				collect(struct s_obj const *const obj,
							struct s_ofile_collector const *const collector,
							void *const user)
{
	uint64_t arch_off, arch_size;
	struct s_obj new_obj = { .target = target, .ofile = OFILE_FAT };
	if (obj->m64)
	{
		struct fat_arch_64 const *const arch =
			(struct fat_arch_64 *)fat_archs + i;
		arch_off = obj_swap64(obj, arch->offset);
		arch_size = obj_swap64(obj, arch->size);
	}
	else
	{
		struct fat_arch const *const arch =
			(struct fat_arch *)fat_archs + i;
		arch_off = obj_swap32(obj, arch->offset);
		arch_size = obj_swap32(obj, arch->size);
	}
	new_obj.size = arch_size;
	if ((new_obj.buf = obj_peek(obj, arch_off, arch_size)) == NULL)
		return (OFILE_E_INVAL_ARCHINFO);
	int err;
	if ((err = load(&new_obj, collector, user)))
		return (err);
	++i;
}

int						fat_load(struct s_obj const *const obj,
							struct s_ofile_collector const *const collector,
							void *const user)
{
	struct fat_header const	*const header = obj_peek(obj, 0, sizeof *header);
	NXArchInfo const		*target;
	uint32_t				nfat_arch;
	void const				*fat_archs;
	int						err;

	if (header == NULL)
		return (OFILE_E_INVAL_FATHDR);
	target = obj->target != OFILE_NX_HOST
		? obj->target : NXGetArchInfoFromName("x86_64");
	nfat_arch = obj_swap32(obj, header->nfat_arch);
	fat_archs = obj_peek(obj, sizeof *header, g_sizes[obj->m64] * nfat_arch);
	if (fat_archs == NULL)
		return (OFILE_E_INVAL_FATARCH);
	if (target != OFILE_NX_ALL
		&& (err = target_update(obj, &target, nfat_arch, fat_archs)))
		return (err);

	uint32_t i;

	i = 0;
	while (i < nfat_arch)
		collect(obj, collector, user);
	return (0);
}
