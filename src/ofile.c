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

/**
 * Retrieve the string representation of an error code
 */
char const *ofile_etoa(int const err)
{
	switch (err) {
		case OFILE_E_INVAL_MAGIC:
			return "Invalid magic number";
		case OFILE_E_INVAL_FATHDR:
			return "Invalid FAT header";
		case OFILE_E_INVAL_FATARCH:
			return "Invalid FAT architecture";
		case OFILE_E_INVAL_ARCHINFO:
			return "Invalid FAT architecture info";
		case OFILE_E_INVAL_ARCHOBJ:
			return "Invalid FAT architecture object";
		case OFILE_E_INVAL_MHHDR:
			return "Invalid Mach-o header";
		case OFILE_E_INVAL_ARHDR:
			return "Invalid archive header";
		case OFILE_E_INVAL_AROBJHDR:
			return "Invalid archive object header";
		case OFILE_E_INVAL_LC:
			return "Invalid load command size";
		case OFILE_E_NOTFOUND_ARCH:
			return "Architecture miss-match";

		case -1:
		default: return ft_strerror(errno);
	}
}

/**
 * Mach-o object definition
 */
struct obj
{
	enum ofile ofile; /**< Object ofile type */

	NXArchInfo const *target; /**< User target      */
	NXArchInfo const *arch;   /**< Object arch info */

	uint8_t const *buf; /**< Object mapped buffer      */
	size_t size;        /**< Object mapped buffer size */

	char const *name; /**< Object name buffer      */
	size_t name_len;  /**< Object name buffer size */

	bool m64; /**< Object is 64 bits       */
	bool le;  /**< Object is little-endian */
};


/* --- Endianness --- */

/**
 * Swap 16 bits unsigned integer according to object endianness
 */
inline uint16_t obj_swap16(struct obj const *const obj, uint16_t u)
{
	return obj->le ? OSSwapConstInt16(u) : u;
}

/**
 * Swap 32 bits unsigned integer according to object endianness
 */
inline uint32_t obj_swap32(struct obj const *const obj, uint32_t const u)
{
	return obj->le ? OSSwapConstInt32(u) : u;
}

/**
 * Swap 64 bits unsigned integer according to object endianness
 */
inline uint64_t obj_swap64(struct obj const *const obj, uint64_t const u)
{
	return obj->le ? OSSwapConstInt64(u) : u;
}


/* --- Mach-o object getter --- */

/**
 * Retrieve if this Mach-o object is 64 bits based
 */
inline bool obj_ism64(struct obj const *const obj)
{
	return obj->m64;
}

/**
 * Retrieve Mach-o object ofile type
 */
inline enum ofile obj_ofile(struct obj const *const obj)
{
	return obj->ofile;
}

/**
 * Retrieve targeted object architecture info
 */
inline NXArchInfo const *obj_target(struct obj const *const obj)
{
	return obj->target;
}

/**
 * Retrieve object architecture info
 */
inline NXArchInfo const *obj_arch(struct obj const *const obj)
{
	return obj->arch;
}

/**
 * Retrieve Mach-o object name, only for archive
 */
inline char const *obj_name(struct obj const *const obj, size_t *out_len)
{
	if (out_len && obj->name_len) *out_len = obj->name_len;
	return obj->name;
}

/**
 * Peek sized data at offset on Mach-o object
 */
inline const void *obj_peek(struct obj const *const obj, size_t const off,
							size_t const len)
{
	/* `len` argument is only used for bound checking */
	return len == 0 || off + len > obj->size
	       ? ((errno = EBADMACHO), NULL)
	       : obj->buf + off;
}


/* --- Mach-o load --- */

/**
 * Generic load
 * @param obj        [in] Mutable mach-o object
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return                0 on success, -1 or error code otherwise
 */
static int load(struct obj *obj,
                struct ofile_collector const *collector, void *user);

/**
 * Start at `mach_header` and collect each `load_command`
 * @param obj        [in] Mach-o object
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 or error code otherwise
 */
static inline int mh_load(struct obj const *obj,
                          struct ofile_collector const *collector, void *user)
{
	/* Didn't use the mach_header_64 struct since we only access fields
	 * which have the same size on both 32 and 64 struct */
	struct mach_header const *const header = obj_peek(obj, 0, sizeof *header);
	if (header == NULL) return OFILE_E_INVAL_MHHDR;
	struct obj new_obj = *obj;

	/* Validate the Mach-o header arch info */
	new_obj.arch =
		NXGetArchInfoFromCpuType(
			(cpu_type_t)obj_swap32(obj, (uint32_t)header->cputype),
			(cpu_subtype_t)obj_swap32(obj, (uint32_t)header->cpusubtype));

	if (new_obj.arch == NULL)
		return (errno = EBADMACHO), OFILE_E_INVAL_ARCHINFO;

	/* Skip load if arch didn't match targeted one */
	if (obj->target != NULL && obj->target != OFILE_NX_HOST &&
		(obj->target->cputype != new_obj.arch->cputype ||
	    obj->target->cpusubtype != new_obj.arch->cpusubtype))
		return 0;

	/* User call-back on load */
	if (collector->load)
		collector->load(&new_obj, user);

	static size_t const header_sizes[] = {
		[false] = sizeof(struct mach_header),
		[true]  = sizeof(struct mach_header_64)
	};

	int err = 0; /* Keep a mutable error slot to set on collection */
	size_t off = header_sizes[obj->m64];

	/* Loop though load command and collect each one,
	 * command data is next to it's header */
	for (uint32_t ncmds = obj_swap32(obj, header->ncmds);
	     ncmds-- && err == 0;) {

		/* Peek the load command structure */
		struct load_command const *lc = obj_peek(obj, off, sizeof *lc);
		if (lc == NULL)
			return OFILE_E_INVAL_LC;
		/* Check for LC size */
		if (obj_peek(obj, off, obj_swap32(obj, lc->cmdsize)) == NULL)
			return OFILE_E_INVAL_LC;

		uint32_t const cmd = obj_swap32(obj, lc->cmd);

		/* Perform user collection if enabled */
		if (cmd < collector->ncollector && collector->collectors[cmd])
			err = collector->collectors[cmd](&new_obj, off, user);

		off += obj_swap32(obj, lc->cmdsize);
	}

	return err;
}


/* --- FAT load --- */

/**
 * Start at `fat_header` and load each `fat_arch`
 * @param obj        [in] Mach-o object
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 or error code otherwise
 */
static int fat_load(struct obj const *const obj,
	struct ofile_collector const *const collector, void *const user)
{
	struct fat_header const *const header = obj_peek(obj, 0, sizeof *header);
	if (header == NULL) return OFILE_E_INVAL_FATHDR;

	/* Retrieve the target architecture information,
	 * use an horrible hack here to get host arch info, hard-code bitch */
	NXArchInfo const *target = obj->target != OFILE_NX_HOST
	                           ? obj->target : NXGetArchInfoFromName("x86_64");

	static size_t const arch_sizes[] = {
		[false] = sizeof(struct fat_arch),
		[true]  = sizeof(struct fat_arch_64)
	};

	/* Fat arch are consecutive in memory just after the fat header
	 * check for total validity */
	uint32_t const nfat_arch = obj_swap32(obj, header->nfat_arch);
	const void *fat_archs    = obj_peek(obj, sizeof *header,
	                                    arch_sizes[obj->m64] * nfat_arch);
	if (fat_archs == NULL) return OFILE_E_INVAL_FATARCH;

	/* We got an arch info target form user, so find the appropriate one
	 * in the fat arch array, if none exists, target all arch instead */
	if (target != OFILE_NX_ALL) {
		bool find = false;

		for (uint32_t i = 0; i < nfat_arch; ++i) {
			struct fat_arch const *const arch = obj->m64
				? (struct fat_arch *)((struct fat_arch_64 *)fat_archs + i)
				: (struct fat_arch *)fat_archs + i;

			NXArchInfo const *const info = NXGetArchInfoFromCpuType(
				(cpu_type_t)obj_swap32(obj, (uint32_t) arch->cputype),
				(cpu_subtype_t) obj_swap32(obj, (uint32_t)arch->cpusubtype));

			if (info == NULL)
				return (errno = EBADMACHO), OFILE_E_INVAL_ARCHINFO;

			if (target->cputype == info->cputype &&
			    target->cpusubtype == info->cpusubtype)
				find = true;
		}

		/* In case of no match and user target, fail as not found */
		if (find == false && obj->target && obj->target != OFILE_NX_HOST)
			return OFILE_E_NOTFOUND_ARCH;

		/* In case of no match, target all arch instead */
		if (find == false) target = OFILE_NX_ALL;
	}

	/* Loop though FAT arch and load each one.
	 * Mach-o header are accessible at `arch->offset` */
	for (uint32_t i = 0; i < nfat_arch; ++i) {
		uint64_t arch_off, arch_size;
		struct obj new_obj = { .target = target, .ofile = OFILE_FAT };

		/* Peek the FAT arch structure,
		 * then retrieve arch offset and size */
		if (obj->m64) {
			struct fat_arch_64 const *const arch =
				(struct fat_arch_64 *)fat_archs + i;

			arch_off  = obj_swap64(obj, arch->offset);
			arch_size = obj_swap64(obj, arch->size);
		} else {
			struct fat_arch const *const arch =
				(struct fat_arch *)fat_archs + i;

			arch_off  = obj_swap32(obj, arch->offset);
			arch_size = obj_swap32(obj, arch->size);
		}

		new_obj.size = arch_size;
		if ((new_obj.buf = obj_peek(obj, arch_off, arch_size)) == NULL)
			return OFILE_E_INVAL_ARCHINFO;

		/* Load at new offset */
		int const err = load(&new_obj, collector, user);
		if (err) return err;
	}

	return 0;
}


/* --- Archive load --- */

/**
 * AR object information definition
 */
struct ar_info
{
	char const *name; /**< AR object name buffer */
	size_t name_len;  /**< AR object name length */

	uint8_t const *obj; /**< AR object buffer */
	size_t size;        /**< AR object size   */
};

/**
 * Peek AR object information
 * @param obj    [in] Mach-o object
 * @param off    [in] Starting offset
 * @param info  [out] Out AR object information
 * @param root   [in] Specify if we peek the root header
 * @return            0 on success, -1 or error code otherwise
 */
static int ar_info_peek(struct obj const *const obj, size_t *off,
                        struct ar_info *const info)
{
	struct ar_hdr hdr_cpy;

	/* Peek AR header */
	struct ar_hdr const *const hdr = obj_peek(obj, *off, sizeof *hdr);
	if (hdr == NULL) return OFILE_E_NO_ARHDR;
	*off += sizeof *hdr;

	/* Check for header consistency (must be "`\n") */
	if (ft_strncmp(ARFMAG, hdr->ar_fmag, sizeof(ARFMAG) - 1) != 0)
		return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

	/* Keep a local mutable copy of the AR header to safely insert null char */
	ft_memcpy(&hdr_cpy, hdr, sizeof hdr_cpy);

	/* We don't care about `ar_fmag` anymore, which is the field next to `size`,
	 * insert null terminated.. */
	hdr_cpy.ar_fmag[0] = '\0';

	/* Retrieve object */
	long long int const olen = ft_atoll(hdr_cpy.ar_size);
	if (olen <= 0) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;
	info->size = (size_t)olen;
	info->obj  = obj_peek(obj, *off, info->size);
	if (info->obj == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

	/* We don't care about the date, which is the field next to `name`,
	 * insert null terminated.. */
	hdr_cpy.ar_date[0] = '\0';

	/* Short name (>= 15) */
	if (ft_strncmp(AR_EFMT1, hdr_cpy.ar_name, sizeof(AR_EFMT1) - 1) != 0) {

		/* Skip leading spaces */
		char *spaces = ft_strchr(hdr_cpy.ar_name, ' ');
		if (spaces) *spaces = '\0';

		/* When `ar_name` does not begin with "#1/" the name is in the
		 * ar_name filed and NULL terminated */
		info->name     = hdr->ar_name;
		info->name_len = ft_strnlen(hdr_cpy.ar_name, sizeof hdr->ar_name);

	} else { /* Long name (> 15) */

		/* When `ar_name` begin with "#1/" the name is next to the header and
		 * it's length is next to "#1/" in `ar_name` */
		int const nlen = ft_atoi(hdr_cpy.ar_name + sizeof(AR_EFMT1) - 1);
		if (nlen <= 0 || (size_t)nlen >= info->size)
			return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

		info->name_len = (size_t)nlen;
		info->name     = obj_peek(obj, *off, info->name_len);
		if (info->name == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

		*off       += info->name_len;
		info->obj  += info->name_len;
		info->size -= info->name_len;
	}

	return 0;
}

/**
 * Start at `ar_hdr` and load each sub object
 * @param obj        [in] Mach-o object
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 or error code otherwise
 */
static int ar_load(struct obj const *const obj,
	struct ofile_collector const *const collector, void *const user)
{
	/* AR begin with a special magic: `!<arch>\n`, check for it */
	char const *const mag = obj_peek(obj, 0, SARMAG);
	if (mag == NULL || ft_strncmp(ARMAG, mag, SARMAG) != 0)
		return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;
	size_t off = SARMAG;

	struct ar_info info;

	/* Peek the first archive object */
	int err = ar_info_peek(obj, &off, &info);
	if (err) return err;

	/* First object got a magic name, check for it */
	if (ft_strncmp(info.name, SYMDEF, info.name_len) != 0 &&
	    ft_strncmp(info.name, SYMDEF_SORTED, info.name_len) != 0 &&
	    ft_strncmp(info.name, SYMDEF_64, info.name_len) != 0 &&
	    ft_strncmp(info.name, SYMDEF_64_SORTED, info.name_len) != 0)
		return (errno = EBADMACHO), OFILE_E_INVAL_ARCHOBJ;

	/* User call-back on load with NULL object to indicate AR begin */
	if (collector->load)
		collector->load(NULL, user);

	/* Loop though AR object and load each one */
	while (off += info.size, (err = ar_info_peek(obj, &off, &info)) == 0) {
		struct obj new_obj = {
			.ofile = OFILE_AR, .target = obj->target,
			.buf = info.obj, .size = info.size,
			.name = info.name, .name_len = info.name_len
		};

		/* Load new object */
		err = load(&new_obj, collector, user);
		if (err) return err;
	}

	/* OFILE_E_NO_ARHDR from ar_info_peek means their is no more object -> 0 */
	return err == OFILE_E_NO_ARHDR ? 0 : err;
}


/* --- Generic load --- */

/**
 * Generic load
 */
static int load(struct obj *const obj,
                struct ofile_collector const *const collector, void *const user)
{
	static struct {
		uint32_t const magic;
		bool const m64, le;
		int (*const load)(struct obj const *,
						  struct ofile_collector const *, void *);
	} const loaders[] = {
		{ MH_MAGIC,     false, false, mh_load  },
		{ MH_CIGAM,     false, true,  mh_load  },
		{ MH_MAGIC_64,  true,  false, mh_load  },
		{ MH_CIGAM_64,  true,  true,  mh_load  },
		{ AR_MAGIC,     false, false, ar_load  },
		{ AR_CIGAM,     false, true,  ar_load  },
		{ FAT_MAGIC,    false, false, fat_load },
		{ FAT_CIGAM,    false, true,  fat_load },
		{ FAT_MAGIC_64, true,  false, fat_load },
		{ FAT_CIGAM_64, true,  true,  fat_load },
	};

	unsigned i;

	/* Using the Mach-o magic, find the appropriate loader,
	 * then retrieve LE mode, M64 and load function from it */
	for (i = 0; i < COUNT_OF(loaders) &&
				loaders[i].magic != *(uint32_t *)obj->buf; ++i)

		/* When loading from AR, only expect MH */
		if (obj->ofile == OFILE_AR && i == 3)
			return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;

		/* When loading from FAT, only expect MH */
		else if (obj->ofile == OFILE_FAT && i == 5)
			return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;

	/* Unable to find proper loader for this magic */
	if (i == COUNT_OF(loaders))
		return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;

	obj->m64 = loaders[i].m64;
	obj->le  = loaders[i].le;

	/* Dispatch collection call-back's to correct loader */
	return loaders[i].load(obj, collector, user);
}


/* --- Collection --- */

/**
 * Collect though a Mach-o object
 */
int ofile_collect(char const *const filename, NXArchInfo const *const target,
                  struct ofile_collector const *const collector,
                  void *const user)
{
	int const fd = open(filename, O_RDONLY);
	struct stat st;

	/* Open the given file and check for validity */
	if (fd < 0 || fstat(fd, &st) < 0) return -1;

	size_t const size = (size_t)st.st_size;

	if (size < sizeof(uint32_t)) return (errno = EBADMACHO), -1;
	if ((st.st_mode & S_IFDIR))  return (errno = EISDIR), -1;

	/* Then map the whole file into a buffer,
	 * mapped file read only */
	void *const buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED || close(fd)) return -1;

	struct obj obj = {
		.ofile = OFILE_MH, .target = target,
		.buf = buf, .size = size,
	};

	/* Start loading,
	 * then unmap file and break */
	int const err = load(&obj, collector, user);
	munmap(buf, size);

	return err;
}
