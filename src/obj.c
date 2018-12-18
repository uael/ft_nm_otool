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
#include "../../osxcross/target/SDK/MacOSX10.11.sdk/usr/include/mach-o/loader.h"

#include <ft/cdefs.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>


/**
 * Mach-o object definition
 */
struct obj
{
	const uint8_t *buf;
	size_t len;
	bool m64;
	bool le;
};


/* --- Endianness --- */

/**
 * Swap 16 bits unsigned integer according to object endianness
 */
inline uint16_t obj_swap16(const struct obj *o, uint16_t u)
{
	return o->le ? OSSwapConstInt16(u) : u;
}

/**
 * Swap 32 bits unsigned integer according to object endianness
 */
inline uint32_t obj_swap32(const struct obj *o, uint32_t u)
{
	return o->le ? OSSwapConstInt32(u) : u;
}

/**
 * Swap 64 bits unsigned integer according to object endianness
 */
inline uint64_t obj_swap64(const struct obj *o, uint64_t u)
{
	return o->le ? OSSwapConstInt64(u) : u;
}


/* --- Architecture --- */

/**
 * Peek sized data at offset on Mach-o object
 */
inline bool obj_ism64(const struct obj *o)
{
	return o->m64;
}


/* --- Collection --- */

/**
 * Retrieve if this Mach-o object is 64 bits based
 */
inline const void *obj_peek(const struct obj *const o, size_t const off,
							size_t const len)
{
	/* `len` argument is only used for bound checking */
	if (off + len >= o->len) {
		errno = EBADMACHO;
		return NULL;
	}

	return o->buf + off;
}

/**
 * Different archive load
 */
static int load(const uint8_t *buf, size_t off, size_t len,
				const struct obj_collector *collector, void *user);

/**
 * Start at `mach_header` and collect each `load_command`
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 otherwise with `errno` set
 */
static inline int lc_load(const struct obj *o, size_t off,
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
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 otherwise with `errno` set
 */
static inline int fat_load(const struct obj *const obj, size_t off,
						   const struct obj_collector *const collector,
						   void *user)
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

		if (magic == NULL)
			return -1;

		/* There is no FAT recursion, so check for it */
		if (*magic == FAT_MAGIC || *magic == FAT_CIGAM) {
			errno = EBADMACHO;
			return -1;
		}

		/* Continue load at new offset */
		if (load(obj->buf, arch_off, obj->len, collector, user))
			return -1;

		off += sizeof *arch;
	}

	return 0;
}

/**
 * Different archive load
 * @param buf        [in] Mach-o object file buffer
 * @param off        [in] Load offset
 * @param len        [in] Mach-o object file size
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return                0 on success, -1 otherwise with `errno` set
 */
static int load(const uint8_t *buf, size_t off, size_t len,
				const struct obj_collector *collector, void *user)
{
	static const struct
	{
		uint32_t magic;
		bool m64, le;

		int (*load)(const struct obj *, size_t,
					const struct obj_collector *, void *);
	} loaders[] = {
		{ MH_MAGIC,    false, false, lc_load },
		{ MH_CIGAM,    false, true,  lc_load },
		{ MH_MAGIC_64, true,  false, lc_load },
		{ MH_CIGAM_64, true,  true,  lc_load },
		{ FAT_MAGIC,   false, false, fat_load },
		{ FAT_CIGAM,   false, true,  fat_load },
	};

	unsigned i;

	/* Using the Mach-o magic, dispatch to the appropriate loader,
	 * then retrieve LE mode and M64 */
	for (i = 0; i < COUNT_OF(loaders) &&
				loaders[i].magic != *(uint32_t *)(buf + off); ++i);

	/* Unable to find proper loader for this magic */
	if (i == COUNT_OF(loaders)) {
		errno = EBADMACHO;
		return -1;
	}

	const struct obj o = (struct obj){
		.buf = buf, .len = len,
		.m64 = loaders[i].m64, .le = loaders[i].le
	};

	/* Dispatch collection call-back's to correct loader,
	 * loader maybe the `err_load` one if zero match */
	return loaders[i].load(&o, off, collector, user);
}

/**
 * Collect though a Mach-o object
 */
int obj_collect(const char *const filename,
				const struct obj_collector *const collector, void *const user)
{
	int const fd = open(filename, O_RDONLY);
	struct stat st;

	/* Open the given file and check for validity */
	if (fd < 0 || fstat(fd, &st) < 0)
		return -1;

	size_t const len = (size_t)st.st_size;

	if (len < sizeof(uint32_t))
		return (errno = EBADMACHO), -1;
	if ((st.st_mode & S_IFDIR))
		return (errno = EISDIR), -1;

	/* Then map the whole file into a buffer,
	 * map file read only */
	void *const buf = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED || close(fd))
		return -1;

	/* Start loading,
	 * then unmap file and break */
	int const err = load(buf, 0, len, collector, user);
	munmap(buf, len);

	return err;
}
