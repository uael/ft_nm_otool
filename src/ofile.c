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

static int			ar_info_peek(struct s_obj const *const obj, size_t *off,
						struct s_ar_info *const info)
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

int			ar_load(struct s_obj const *const obj,
						struct s_ofile_collector const *const collector,
						void *const user)
{
	char const *const	mag = obj_peek(obj, 0, SARMAG);
	struct s_ar_info		info;
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
		if ((err = load(&(struct s_obj){
			.ofile = OFILE_AR, .target = obj->target,
			.buf = info.obj, .size = info.size,
			.name = info.name, .name_len = info.name_len, }, collector, user)))
			return err;
		off += info.size;
	}
	return (err == OFILE_E_NO_ARHDR ? 0 : err);
}

static struct s_loader const g_loaders[] = {
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

int			load(struct s_obj *const obj,
						struct s_ofile_collector const *const collector,
						void *const user)
{
	unsigned	i;

	i = 0;
	while (i < COUNT_OF(g_loaders) &&
		g_loaders[i].magic != *(uint32_t *)obj->buf)
		if ((obj->ofile == OFILE_AR && i == 3)
			|| (obj->ofile == OFILE_FAT && i == 5))
		{
			errno = EBADMACHO;
			return (OFILE_E_INVAL_MAGIC);
		}
		else
			++i;
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
						struct s_ofile_collector const *const collector,
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
	err = load(&(struct s_obj){
		.ofile = OFILE_MH, .target = target,
		.buf = buf, .size = size, }, collector, user);
	munmap(buf, size);
	return (err);
}
