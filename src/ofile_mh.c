/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_mh.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <errno.h>

static size_t const	g_header_sizes[] = {
	[false] = sizeof(struct mach_header),
	[true] = sizeof(struct mach_header_64)
};

static int			collect(uint32_t ncmds, struct s_obj const *obj,
						struct s_ofile_collector const *const collector,
						void *const user)
{
	struct load_command const	*lc;
	int							err;
	size_t						off;
	uint32_t					cmd;

	err = 0;
	off = g_header_sizes[obj->m64];
	while (ncmds-- && err == 0)
	{
		if ((lc = obj_peek(obj, off, sizeof(*lc))) == NULL)
			return (OFILE_E_INVAL_LC);
		if ((lc = obj_peek(obj, off, obj_swap32(obj, lc->cmdsize))) == NULL)
			return (OFILE_E_INVAL_LC);
		cmd = obj_swap32(obj, lc->cmd);
		if (cmd < collector->ncollector && collector->collectors[cmd])
			err = collector->collectors[cmd](obj, off, user);
		off += obj_swap32(obj, lc->cmdsize);
	}
	return (err);
}

int					mh_load(struct s_obj const *obj,
						struct s_ofile_collector const *const collector,
						void *const user)
{
	struct mach_header const *const	hdr = obj_peek(obj, 0, sizeof(*hdr));
	struct s_obj					new_obj;

	if (hdr == NULL)
		return (OFILE_E_INVAL_MHHDR);
	new_obj = *obj;
	new_obj.arch_info = NXGetArchInfoFromCpuType(
		(cpu_type_t)obj_swap32(obj, (uint32_t)hdr->cputype),
		(cpu_subtype_t)obj_swap32(obj, (uint32_t)hdr->cpusubtype));
	if (new_obj.arch_info == NULL)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_ARCHINFO);
	}
	if (obj->target != NULL && obj->target != OFILE_NX_HOST
		&& (obj->target->cputype != new_obj.arch_info->cputype
			|| obj->target->cpusubtype != new_obj.arch_info->cpusubtype))
		return (0);
	if (collector->load)
		collector->load(&new_obj, new_obj.arch_info, user);
	return (collect(obj_swap32(obj, hdr->ncmds), &new_obj, collector, user));
}