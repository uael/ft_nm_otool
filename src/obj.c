/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj.h"

#include <ft/cdefs.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

struct obj
{
	const uint8_t *buf;
	size_t len;
	bool m64;
	bool le;
};

uint16_t obj_swap16(const struct obj *o, uint16_t u)
{
	return o->le ? OSSwapConstInt16(u) : u;
}

uint32_t obj_swap32(const struct obj *o, uint32_t u)
{
	return o->le ? OSSwapConstInt32(u) : u;
}

uint64_t obj_swap64(const struct obj *o, uint64_t u)
{
	return o->le ? OSSwapConstInt64(u) : u;
}

bool obj_ism64(const struct obj *o)
{
	return o->m64;
}

const void *obj_peek(const struct obj *o, size_t off, size_t len)
{
	if (off + len >= o->len) {
		errno = EBADMACHO;
		return NULL;
	}

	return o->buf + off;
}

static int load(const uint8_t *buf, size_t off, size_t len,
				const struct obj_collector *collector, void *user);

/**
 * Start at `mach_header` and collect each `load_command`
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collector's
 * @return           0 on success, -1 otherwise with `errno` set
 */
static int lc_load(const struct obj *o, size_t off,
				   const struct obj_collector *collectors, void *user)
{
	const struct mach_header *const header = obj_peek(o, off, sizeof *header);

	if (header == NULL)
		return -1;

	static const size_t header_size[] = {
		[false] = sizeof(struct mach_header),
		[true]  = sizeof(struct mach_header_64)
	};

	int err = 0;
	off += header_size[o->m64];

	/* Loop though load command and collect each one
	 * command data is next to it's header */
	for (uint32_t ncmds = obj_swap32(o, header->ncmds); ncmds-- && err == 0;) {

		/* Peek the load command structure */
		const struct load_command *const lc = obj_peek(o, off, sizeof *lc);

		if (lc == NULL)
			return -1;

		uint32_t const cmd = obj_swap32(o, lc->cmd);

		/* Perform user collection if enabled */
		if (cmd < collectors->ncollector && collectors->collectors[cmd])
			err = collectors->collectors[cmd](o, off, user);

		off += lc->cmdsize;
	}

	return err;
}

/**
 * Start at `fat_header` and load each `fat_arch`
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collector's
 * @return           0 on success, -1 otherwise with `errno` set
 */
static int fat_load(const struct obj *const obj, size_t off,
					const struct obj_collector *const collector, void *user)
{
	const struct fat_header *const header = obj_peek(obj, off, sizeof *header);

	if (header == NULL)
		return -1;

	off += sizeof *header;

	/* Loop though FAT arch and load each one
	 * Mach-o header are accessible at `arch->offset` */
	for (uint32_t nfat_arch = obj_swap32(obj, header->nfat_arch);
		 nfat_arch--;) {

		/* Peek the FAT arch structure,
		 * then peek it's mach-o header offset */
		const struct fat_arch *const arch = obj_peek(obj, off, sizeof *arch);

		if (arch == NULL)
			return -1;

		uint32_t const arch_off = obj_swap32(obj, arch->offset);
		const uint32_t *const magic = obj_peek(obj, arch_off, sizeof *magic);

		/* There is no FAT recursion, so check for it */
		if (magic == NULL || *magic == FAT_MAGIC || *magic == FAT_CIGAM)
			return -1;

		/* Continue load at new offset */
		if (load(obj->buf, arch_off, obj->len, collector, user))
			return -1;

		off += sizeof *arch;
	}

	return 0;
}

/**
 * Dummy loader called on unknown or unsupported archive fromat
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collector's
 * @return           0 on success, -1 otherwise with `errno` set
 */
static int err_load(const struct obj *const obj, const size_t off,
					const struct obj_collector *const collector, void *user)
{
	(void)obj,
		(void)off,
		(void)collector;
	(void)user;

	errno = EBADMACHO;
	return -1;
}

static int load(const uint8_t *buf, size_t off, size_t len,
				const struct obj_collector *collector, void *user)
{
	static const struct
	{
		uint32_t magic;
		bool m64;
		bool le;

		int (*load)(const struct obj *, size_t,
					const struct obj_collector *, void *);
	} loaders[] = {
		{ MH_MAGIC,    false, false, lc_load },
		{ MH_CIGAM,    false, true,  lc_load },
		{ MH_MAGIC_64, true,  false, lc_load },
		{ MH_CIGAM_64, true,  true,  lc_load },
		{ FAT_MAGIC,   false, false, fat_load },
		{ FAT_CIGAM,   false, true,  fat_load },
		{ 0,           false, false, err_load },
	};

	unsigned i;

	for (i = 0; i < COUNT_OF(loaders) &&
				loaders[i].magic != *(uint32_t *)(buf + off); ++i);

	const struct obj o = (struct obj){
		.buf = buf, .len = len,
		.m64 = loaders[i].m64, .le = loaders[i].le
	};

	return loaders[i].load(&o, off, collector, user);
}

int obj_collect(const char *filename,
				const struct obj_collector *collector, void *user)
{
	int fd, err;
	struct stat st;
	void *buf;
	size_t len;

	if ((fd = open(filename, O_RDONLY)) < 0)
		return -1;

	if (fstat(fd, &st) < 0)
		return -1;

	if ((len = (size_t)st.st_size) < sizeof(uint32_t))
		return (errno = EBADMACHO), -1;

	if ((st.st_mode & S_IFDIR))
		return (errno = EISDIR), -1;

	if ((buf = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return -1;

	if (close(fd))
		return -1;

	err = load(buf, 0, len, collector, user);

	munmap(buf, len);
	return err;
}
