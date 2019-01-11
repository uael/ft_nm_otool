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

static struct s_loader const	g_loaders[] = {
	{ MH_MAGIC, false, false, mh_load },
	{ MH_CIGAM, false, true, mh_load },
	{ MH_MAGIC_64, true, false, mh_load },
	{ MH_CIGAM_64, true, true, mh_load },
	{ AR_MAGIC, false, false, ar_load },
	{ AR_CIGAM, false, true, ar_load },
	{ FAT_MAGIC, false, false, fat_load },
	{ FAT_CIGAM, false, true, fat_load },
	{ FAT_MAGIC_64, true, false, fat_load },
	{ FAT_CIGAM_64, true, true, fat_load },
};

int								load(struct s_obj *const obj,
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

int								ofile_collect(char const *const filename,
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
		return (-1);
	}
	buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED || close(fd))
		return (-1);
	err = load(&(struct s_obj){ .ofile = OFILE_MH, .target = target,
		.buf = buf, .size = size, }, collector, user);
	munmap(buf, size);
	return (err);
}
