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


/**
 * Mach-o object definition
 */
struct obj
{
	NXArchInfo const *target; /**< User target */

	uint8_t const *buf; /**< Object mapped buffer      */
	size_t size;        /**< Object mapped buffer size */

	bool fat; /**< Object come from fat    */
	bool m64; /**< Object is 64 bits       */
	bool le;  /**< Object is little-endian */
};


/* --- Endianness --- */

/**
 * Swap 16 bits unsigned integer according to object endianness
 */
inline uint16_t obj_swap16(struct obj const *o, uint16_t u)
{
	return o->le ? OSSwapConstInt16(u) : u;
}

/**
 * Swap 32 bits unsigned integer according to object endianness
 */
inline uint32_t obj_swap32(struct obj const *o, uint32_t u)
{
	return o->le ? OSSwapConstInt32(u) : u;
}

/**
 * Swap 64 bits unsigned integer according to object endianness
 */
inline uint64_t obj_swap64(struct obj const *o, uint64_t u)
{
	return o->le ? OSSwapConstInt64(u) : u;
}


/* --- Architecture --- */

/**
 * Retrieve if this Mach-o object is 64 bits based
 */
inline bool obj_ism64(struct obj const *o)
{
	return o->m64;
}

/**
 * Retrieve whatever Mach-o object is fat
 */
inline bool obj_isfat(struct obj const *o)
{
	return o->fat;
}

/**
 * Retrieve targeted object architecture info
 */
inline NXArchInfo const *obj_target(struct obj const *o)
{
	return o->target;
}


/* --- Collection --- */

/**
 * Peek sized data at offset on Mach-o object
 */
inline const void *obj_peek(struct obj const *const o, size_t const off,
							size_t const len)
{
	/* `len` argument is only used for bound checking */
	return off + len > o->size ? ((errno = EBADMACHO), NULL) : o->buf + off;
}

/**
 * Different archive load
 */
static int load(NXArchInfo const *target, bool fat,
                const uint8_t *buf, size_t size,
                const struct obj_collector *collector, void *user);

/**
 * Start at `mach_header` and collect each `load_command`
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 otherwise with `errno` set
 */
static inline int lc_load(struct obj const *o, size_t off,
						  const struct obj_collector *collectors, void *user)
{
	/* Didn't use the mach_header_64 struct since we only access fields
	 * which have the same size on both 32 and 64 struct */
	const struct mach_header *const header = obj_peek(o, off, sizeof *header);
	if (header == NULL) return OBJ_E_INVAL_MHHDR;

	/* Validate the Mach-o header arch info */
	NXArchInfo const *const arch_info =
		NXGetArchInfoFromCpuType(
			(cpu_type_t)obj_swap32(o, (uint32_t)header->cputype),
			(cpu_subtype_t)obj_swap32(o, (uint32_t)header->cpusubtype));

	if (arch_info == NULL)
		return (errno = EBADMACHO), OBJ_E_INVAL_ARCHINFO;

	/* Skip load if arch didn't match targeted one */
	if (o->target != NULL && o->target != OBJ_NX_HOST &&
		(o->target->cputype != arch_info->cputype ||
	    o->target->cpusubtype != arch_info->cpusubtype))
		return 0;

	static const size_t header_sizes[] = {
		[false] = sizeof(struct mach_header),
		[true]  = sizeof(struct mach_header_64)
	};

	int err = 0; /* Keep a mutable error slot to set on collection */
	off += header_sizes[o->m64];

	/* Loop though load command and collect each one,
	 * command data is next to it's header */
	for (uint32_t ncmds = obj_swap32(o, header->ncmds); ncmds-- && err == 0;) {

		/* Peek the load command structure */
		const struct load_command *const lc = obj_peek(o, off, sizeof *lc);
		if (lc == NULL) return OBJ_E_INVAL_LC;

		uint32_t const cmd = obj_swap32(o, lc->cmd);

		/* Perform user collection if enabled */
		if (cmd < collectors->ncollector && collectors->collectors[cmd])
			err = collectors->collectors[cmd](o, arch_info, off, user);

		off += obj_swap32(o, lc->cmdsize);
	}

	return err;
}

/**
 * Start at `fat_header` and load each `fat_arch`
 * @param o          [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 otherwise with `errno` set
 */
static inline int fat_load(struct obj const *const o, size_t const off,
						   const struct obj_collector *const collector,
						   void *user)
{
	/* There is no FAT recursion, so check for it */
	if (o->fat) return OBJ_E_FAT_RECURSION;

	const struct fat_header *const header = obj_peek(o, off, sizeof *header);
	if (header == NULL) return OBJ_E_INVAL_FATHDR;

	/* Retrieve the target architecture information,
	 * use an horrible hack here to get host arch info, hardcode bitch */
	NXArchInfo const *arch_info = o->target != OBJ_NX_HOST
	                              ? o->target : NXGetArchInfoFromName("x86_64");

	static size_t const arch_sizes[] = {
		[false] = sizeof(struct fat_arch),
		[true]  = sizeof(struct fat_arch_64)
	};

	/* Fat arch are consecutive in memory just after the fat header
	 * check for total validity */
	uint32_t const nfat_arch = obj_swap32(o, header->nfat_arch);
	const void *fat_archs    = obj_peek(o, off + sizeof *header,
	                                    arch_sizes[o->m64] * nfat_arch);
	if (fat_archs == NULL) return OBJ_E_INVAL_FATARCH;

	/* We got an arch info target form user, so find the appropriate one
	 * in the fat arch array, if none exists, target all arch instead */
	if (arch_info) {
		bool find = false;

		for (uint32_t i = 0; i < nfat_arch; ++i) {
			struct fat_arch const *const arch = o->m64
				? (struct fat_arch *)((struct fat_arch_64 *)fat_archs + i)
				: (struct fat_arch *)fat_archs + i;

			NXArchInfo const *const info = NXGetArchInfoFromCpuType(
				(cpu_type_t)obj_swap32(o, (uint32_t) arch->cputype),
				(cpu_subtype_t) obj_swap32(o, (uint32_t)arch->cpusubtype));

			if (info == NULL)
				return (errno = EBADMACHO), OBJ_E_INVAL_ARCHINFO;

			/* Skip load if arch didn't match targeted one */
			if (arch_info->cputype == info->cputype &&
				arch_info->cpusubtype == info->cpusubtype)
				find = true;
		}

		/* In case of no match and user target, fail as not found */
		if (find == false && o->target && o->target != OBJ_NX_HOST)
			return (errno = EBADMACHO), OBJ_E_NOTFOUND_ARCH;

		/* In case of no match, target all arch instead */
		if (find == false) arch_info = NULL;
	}

	/* Loop though FAT arch and load each one.
	 * Mach-o header are accessible at `arch->offset` */
	for (uint32_t i = 0; i < nfat_arch; ++i) {
		uint64_t arch_off, arch_size;

		/* Peek the FAT arch structure,
		 * then retrieve arch offset and size */
		if (o->m64) {
			struct fat_arch_64 const *const arch =
				(struct fat_arch_64 *)fat_archs + i;

			arch_off  = obj_swap64(o, arch->offset);
			arch_size = obj_swap64(o, arch->size);
		} else {
			struct fat_arch const *const arch =
				(struct fat_arch *)fat_archs + i;

			arch_off  = obj_swap32(o, arch->offset);
			arch_size = obj_swap32(o, arch->size);
		}
		
		/* Load at new offset */
		int const err = load(arch_info, true, o->buf + arch_off, arch_size,
		                     collector, user);
		if (err) return err;
	}

	return 0;
}

/**
 * Start at `ar_header` and load each ar object
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 otherwise with `errno` set
 */
static inline int ar_load(struct obj const *const obj, size_t off,
						  const struct obj_collector *const collector,
						  void *user)
{
	(void)obj;
	(void)off;
	(void)collector;
	(void)user;
	return 0;
}

/**
 * Different archive load
 * @param target     [in] Targeted arch
 * @param fat        [in] Come from fat object
 * @param buf        [in] Mach-o object file buffer
 * @param size       [in] Mach-o object file size
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return                0 on success, error code otherwise with `errno` set
 */
static int load(NXArchInfo const *target, bool fat,
                const uint8_t *buf, size_t size,
                const struct obj_collector *collector, void *user)
{
	static const struct {
		uint32_t magic;
		bool m64, le;

		int (*load)(struct obj const *, size_t,
					const struct obj_collector *, void *);
	} loaders[] = {
		{ MH_MAGIC,     false, false, lc_load  },
		{ MH_CIGAM,     false, true,  lc_load  },
		{ MH_MAGIC_64,  true,  false, lc_load  },
		{ MH_CIGAM_64,  true,  true,  lc_load  },
		{ FAT_MAGIC,    false, false, fat_load },
		{ FAT_CIGAM,    false, true,  fat_load },
		{ FAT_MAGIC_64, true,  false, fat_load },
		{ FAT_CIGAM_64, true,  true,  fat_load },
		{ AR_MAGIC,     false, false, ar_load  },
		{ AR_CIGAM,     false, true,  ar_load  },
	};

	unsigned i;

	/* Using the Mach-o magic, dispatch to the appropriate loader,
	 * then retrieve LE mode and M64 */
	for (i = 0; i < COUNT_OF(loaders) &&
				loaders[i].magic != *(uint32_t *)buf; ++i);

	/* Unable to find proper loader for this magic */
	if (i == COUNT_OF(loaders))
		return (errno = EBADMACHO), OBJ_E_INVAL_MAGIC;

	const struct obj o = (struct obj){
		.target = target, .fat = fat,
		.buf = buf, .size = size,
		.m64 = loaders[i].m64, .le = loaders[i].le
	};

	/* Dispatch collection call-back's to correct loader,
	 * loader maybe the `err_load` one if zero match */
	return loaders[i].load(&o, 0, collector, user);
}

/**
 * Collect though a Mach-o object
 */
int obj_collect(const char *const filename, NXArchInfo const *arch_info,
				const struct obj_collector *const collector, void *const user)
{
	int const fd = open(filename, O_RDONLY);
	struct stat st;

	/* Open the given file and check for validity */
	if (fd < 0 || fstat(fd, &st) < 0)
		return -1;

	size_t const size = (size_t)st.st_size;

	if (size < sizeof(uint32_t))
		return (errno = EBADMACHO), -1;
	if ((st.st_mode & S_IFDIR))
		return (errno = EISDIR), -1;

	/* Then map the whole file into a buffer,
	 * map file read only */
	void *const buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED || close(fd))
		return -1;

	/* Start loading,
	 * then unmap file and break */
	int const err = load(arch_info, false, buf, size, collector, user);
	munmap(buf, size);

	return err;
}
