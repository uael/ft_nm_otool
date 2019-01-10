/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <ft/cdefs.h>
#include <ft/stdlib.h>
#include <ft/string.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

char const *ofile_etoa(int const err)
{
	if (err == OFILE_E_INVAL_MAGIC)
		return ("Invalid magic number");
	if (err == OFILE_E_INVAL_FATHDR)
		return ("Invalid FAT header");
	if (err == OFILE_E_INVAL_FATARCH)
		return ("Invalid FAT architecture");
	if (err == OFILE_E_INVAL_ARCHINFO)
		return ("Invalid FAT architecture info");
	if (err == OFILE_E_INVAL_ARCHOBJ)
		return ("Invalid FAT architecture object");
	if (err == OFILE_E_INVAL_MHHDR)
		return ("Invalid Mach-o header");
	if (err == OFILE_E_INVAL_ARHDR)
		return ("Invalid archive header");
	if (err == OFILE_E_INVAL_AROBJHDR)
		return ("Invalid archive object header");
	if (err == OFILE_E_INVAL_LC)
		return ("Invalid load command size");
	if (err == OFILE_E_NOTFOUND_ARCH)
		return ("Architecture miss-match");
	return (ft_strerror(errno));
}

struct				obj
{
	enum ofile			ofile;
	NXArchInfo const	*target;
	uint8_t const		*buf;
	size_t				size;
	char const			*name;
	size_t				name_len;
	bool				m64;
	bool				le;
};

uint16_t			obj_swap16(struct obj const *const obj, uint16_t u)
{
	return (obj->le ? OSSwapConstInt16(u) : u);
}

uint32_t			obj_swap32(struct obj const *const obj, uint32_t const u)
{
	return (obj->le ? OSSwapConstInt32(u) : u);
}

uint64_t			obj_swap64(struct obj const *const obj, uint64_t const u)
{
	return (obj->le ? OSSwapConstInt64(u) : u);
}

bool				obj_ism64(struct obj const *const obj)
{
	return (obj->m64);
}

enum ofile			obj_ofile(struct obj const *const obj)
{
	return (obj->ofile);
}

NXArchInfo const	*obj_target(struct obj const *const obj)
{
	return (obj->target);
}

char const			*obj_name(struct obj const *const obj, size_t *out_len)
{
	if (out_len && obj->name_len)
		*out_len = obj->name_len;
	return (obj->name);
}

inline const void	*obj_peek(struct obj const *const obj, size_t const off,
						size_t const len)
{
	return (len == 0 || off + len > obj->size
		? ((errno = EBADMACHO), NULL)
		: obj->buf + off);
}

static int			load(struct obj *obj,
						struct ofile_collector const *collector, void *user);

static size_t const g_header_sizes[] = {
	sizeof(struct mach_header),
	sizeof(struct mach_header_64)
};

static inline int	mh_load(struct obj const *obj,
						struct ofile_collector const *const collector,
						void *const user)
{
	struct mach_header const *const header = obj_peek(obj, 0, sizeof *header);
	if (header == NULL)
		return (OFILE_E_INVAL_MHHDR);
	NXArchInfo const *const arch_info =
		NXGetArchInfoFromCpuType(
			(cpu_type_t)obj_swap32(obj, (uint32_t)header->cputype),
			(cpu_subtype_t)obj_swap32(obj, (uint32_t)header->cpusubtype));
	if (arch_info == NULL)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_ARCHINFO);
	}
	if (obj->target != NULL && obj->target != OFILE_NX_HOST &&
		(obj->target->cputype != arch_info->cputype ||
	    obj->target->cpusubtype != arch_info->cpusubtype))
		return (0);
	if (collector->load)
		collector->load(obj, arch_info, user);
	int err = 0;
	size_t off = g_header_sizes[obj->m64];
	uint32_t ncmds = obj_swap32(obj, header->ncmds);
	while (ncmds-- && err == 0)
	{
		struct load_command const *const lc = obj_peek(obj, off, sizeof *lc);
		if (lc == NULL)
			return (OFILE_E_INVAL_LC);
		uint32_t const cmd = obj_swap32(obj, lc->cmd);
		if (cmd < collector->ncollector && collector->collectors[cmd])
			err = collector->collectors[cmd](obj, off, user);
		off += obj_swap32(obj, lc->cmdsize);
	}
	return (err);
}

static size_t const	g_arch_sizes[] = {
	sizeof(struct fat_arch),
	sizeof(struct fat_arch_64)
};

static int			fat_load(struct obj const *const obj,
						struct ofile_collector const *const collector,
						void *const user)
{
	struct fat_header const *const header = obj_peek(obj, 0, sizeof *header);
	if (header == NULL)
		return (OFILE_E_INVAL_FATHDR);
	NXArchInfo const *target = obj->target != OFILE_NX_HOST
		? obj->target : NXGetArchInfoFromName("x86_64");
	uint32_t const nfat_arch = obj_swap32(obj, header->nfat_arch);
	const void *fat_archs = obj_peek(obj, sizeof *header,
		g_arch_sizes[obj->m64] * nfat_arch);
	if (fat_archs == NULL)
		return (OFILE_E_INVAL_FATARCH);
	if (target != OFILE_NX_ALL)
	{
		bool find = false;
		uint32_t i = 0;
		while (i < nfat_arch)
		{
			struct fat_arch const *const arch = obj->m64
				? (struct fat_arch *)((struct fat_arch_64 *)fat_archs + i)
				: (struct fat_arch *)fat_archs + i;
			NXArchInfo const *const info = NXGetArchInfoFromCpuType(
				(cpu_type_t)obj_swap32(obj, (uint32_t) arch->cputype),
				(cpu_subtype_t) obj_swap32(obj, (uint32_t)arch->cpusubtype));
			if (info == NULL)
			{
				errno = EBADMACHO;
				return (OFILE_E_INVAL_ARCHINFO);
			}
			if (target->cputype == info->cputype &&
				target->cpusubtype == info->cpusubtype)
				find = true;
			++i;
		}
		if (find == false && obj->target && obj->target != OFILE_NX_HOST)
			return (OFILE_E_NOTFOUND_ARCH);
		if (find == false)
			target = OFILE_NX_ALL;
	}
	uint32_t i = 0;
	while (i < nfat_arch)
	{
		uint64_t arch_off, arch_size;
		struct obj new_obj = { .target = target, .ofile = OFILE_FAT };
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
	return (0);
}

struct				ar_info
{
	char const		*name;
	size_t			name_len;

	uint8_t const	*obj;
	size_t			size;
};

static int			ar_info_peek(struct obj const *const obj, size_t *off,
						struct ar_info *const info)
{
	struct ar_hdr	hdr_cpy;

	struct ar_hdr const *const hdr = obj_peek(obj, *off, sizeof *hdr);
	if (hdr == NULL)
		return OFILE_E_NO_ARHDR;
	*off += sizeof *hdr;
	if (ft_strncmp(ARFMAG, hdr->ar_fmag, sizeof(ARFMAG) - 1) != 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_ARHDR);
	}
	ft_memcpy(&hdr_cpy, hdr, sizeof hdr_cpy);
	hdr_cpy.ar_fmag[0] = '\0';
	long long int const olen = ft_atoll(hdr_cpy.ar_size);
	if (olen <= 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_ARHDR);
	}
	info->size = (size_t)olen;
	info->obj = obj_peek(obj, *off, info->size);
	if (info->obj == NULL)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_ARHDR);
	}
	hdr_cpy.ar_date[0] = '\0';
	if (ft_strncmp(AR_EFMT1, hdr_cpy.ar_name, sizeof(AR_EFMT1) - 1) != 0)
	{
		char *spaces = ft_strchr(hdr_cpy.ar_name, ' ');
		if (spaces) *spaces = '\0';
		info->name = hdr->ar_name;
		info->name_len = ft_strnlen(hdr_cpy.ar_name, sizeof hdr->ar_name);
	}
	else
	{
		int const nlen = ft_atoi(hdr_cpy.ar_name + sizeof(AR_EFMT1) - 1);
		if (nlen <= 0 || (size_t)nlen >= info->size)
		{
			errno = EBADMACHO;
			return (OFILE_E_INVAL_ARHDR);
		}
		info->name_len = (size_t)nlen;
		info->name = obj_peek(obj, *off, info->name_len);
		if (info->name == NULL)
		{
			errno = EBADMACHO;
			return (OFILE_E_INVAL_ARHDR);
		}
		*off += info->name_len;
		info->obj += info->name_len;
		info->size -= info->name_len;
	}
	return (0);
}

static int			ar_load(struct obj const *const obj,
						struct ofile_collector const *const collector,
						void *const user)
{
	char const *const	mag = obj_peek(obj, 0, SARMAG);
	struct ar_info		info;
	size_t				off;
	int					err;

	if (mag == NULL || ft_strncmp(ARMAG, mag, SARMAG) != 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_MAGIC);
	}
	off = SARMAG;
	if ((err = ar_info_peek(obj, &off, &info)))
		return (err);
	if (ft_strncmp(info.name, SYMDEF, info.name_len) != 0
		&& ft_strncmp(info.name, SYMDEF_SORTED, info.name_len) != 0
		&& ft_strncmp(info.name, SYMDEF_64, info.name_len) != 0
	    && ft_strncmp(info.name, SYMDEF_64_SORTED, info.name_len) != 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_MAGIC);
	}
	off += info.size;
	while ((err = ar_info_peek(obj, &off, &info)) == 0)
	{
		if ((err = load(&(struct obj){
			.ofile = OFILE_AR, .target = obj->target,
			.buf = info.obj, .size = info.size,
			.name = info.name, .name_len = info.name_len, }, collector, user)))
			return err;
		off += info.size;
	}
	return (err == OFILE_E_NO_ARHDR ? 0 : err);
}

static struct {
	uint32_t const magic;
	bool const m64, le;
	int (*const load)(struct obj const *,
	                  struct ofile_collector const *, void *);
} const g_loaders[] = {
	{MH_MAGIC, false, false, mh_load},
	{MH_CIGAM, false, true, mh_load},
	{MH_MAGIC_64, true, false, mh_load},
	{MH_CIGAM_64, true, true, mh_load},
	{AR_MAGIC, false, false, ar_load},
	{AR_CIGAM, false, true, ar_load},
	{FAT_MAGIC, false, false, fat_load},
	{FAT_CIGAM, false, true, fat_load},
	{FAT_MAGIC_64, true, false, fat_load},
	{FAT_CIGAM_64, true, true, fat_load},
};

static int			load(struct obj *const obj,
						struct ofile_collector const *const collector,
						void *const user)
{
	unsigned	i;

	i = 0;
	while (i < COUNT_OF(g_loaders) &&
		g_loaders[i++].magic != *(uint32_t *)obj->buf)
		if ((obj->ofile == OFILE_AR && i == 2)
			|| (obj->ofile == OFILE_FAT && i == 4))
		{
			errno = EBADMACHO;
			return (OFILE_E_INVAL_MAGIC);
		}
	if (i == COUNT_OF(g_loaders))
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_MAGIC);
	}
	obj->m64 = g_loaders[i].m64;
	obj->le = g_loaders[i].le;
	return (g_loaders[i].load(obj, collector, user));
}

int 				ofile_collect(char const *const filename,
						NXArchInfo const *const target,
						struct ofile_collector const *const collector,
						void *const user)
{
	int			fd;
	struct stat	st;
	size_t		size;
	void		*buf;
	int			err;

	if ((fd = open(filename, O_RDONLY)) < 0 || fstat(fd, &st) < 0)
		return (-1);
	if ((size = (size_t)st.st_size) < sizeof(uint32_t))
	{
		errno = EBADMACHO;
		return (-1);
	}
	if ((st.st_mode & S_IFDIR))
	{
		errno = EISDIR;
		return -1;
	}
	buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED || close(fd)) return -1;
	err = load(&(struct obj){
		.ofile = OFILE_MH, .target = target,
		.buf = buf, .size = size, }, collector, user);
	munmap(buf, size);
	return (err);
}
