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
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>


char const *ofile_etoa(int const err)
{
	switch (err) {
		case OFILE_E_INVAL_MAGIC:
			return "Invalid magic number";
		case OFILE_E_INVAL_FATHDR:
			return "Invalid FAT header";
		case OFILE_E_INVAL_FATARCH:
			return "Invalid FAt architecture";
		case OFILE_E_INVAL_ARCHINFO:
			return "Invalid FAt architecture info";
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

	NXArchInfo const *target; /**< User target */

	uint8_t const *buf; /**< Object mapped buffer      */
	size_t size;        /**< Object mapped buffer size */

	bool m64; /**< Object is 64 bits       */
	bool le;  /**< Object is little-endian */
};


/* --- Endianness --- */

/**
 * Swap 16 bits unsigned integer according to object endianness
 */
inline uint16_t obj_swap16(struct obj const *const o, uint16_t u)
{
	return o->le ? OSSwapConstInt16(u) : u;
}

/**
 * Swap 32 bits unsigned integer according to object endianness
 */
inline uint32_t obj_swap32(struct obj const *const o, uint32_t const u)
{
	return o->le ? OSSwapConstInt32(u) : u;
}

/**
 * Swap 64 bits unsigned integer according to object endianness
 */
inline uint64_t obj_swap64(struct obj const *const o, uint64_t const u)
{
	return o->le ? OSSwapConstInt64(u) : u;
}


/* --- Architecture --- */

/**
 * Retrieve if this Mach-o object is 64 bits based
 */
inline bool obj_ism64(struct obj const *const o)
{
	return o->m64;
}

/**
 * Retrieve Mach-o object ofile type
 */
inline enum ofile obj_ofile(struct obj const *const o)
{
	return o->ofile;
}

/**
 * Retrieve targeted object architecture info
 */
inline NXArchInfo const *obj_target(struct obj const *const o)
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
static int load(NXArchInfo const *target, enum ofile ofile,
                uint8_t const *buf, size_t size,
                struct ofile_collector const *collector, void *user);

/**
 * Start at `mach_header` and collect each `load_command`
 * @param obj        [in] Mach-o object
 * @param off        [in] Stating offset
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return           0 on success, -1 otherwise with `errno` set
 */
static inline int mh_load(struct obj const *o, size_t off,
                          struct ofile_collector const *collectors, void *user)
{
	/* Didn't use the mach_header_64 struct since we only access fields
	 * which have the same size on both 32 and 64 struct */
	struct mach_header const *const header = obj_peek(o, off, sizeof *header);
	if (header == NULL) return OFILE_E_INVAL_MHHDR;

	/* Validate the Mach-o header arch info */
	NXArchInfo const *const arch_info =
		NXGetArchInfoFromCpuType(
			(cpu_type_t)obj_swap32(o, (uint32_t)header->cputype),
			(cpu_subtype_t)obj_swap32(o, (uint32_t)header->cpusubtype));

	if (arch_info == NULL)
		return (errno = EBADMACHO), OFILE_E_INVAL_ARCHINFO;

	/* Skip load if arch didn't match targeted one */
	if (o->target != NULL && o->target != OFILE_NX_HOST &&
		(o->target->cputype != arch_info->cputype ||
	    o->target->cpusubtype != arch_info->cpusubtype))
		return 0;

	static size_t const header_sizes[] = {
		[false] = sizeof(struct mach_header),
		[true]  = sizeof(struct mach_header_64)
	};

	int err = 0; /* Keep a mutable error slot to set on collection */
	off += header_sizes[o->m64];

	/* Loop though load command and collect each one,
	 * command data is next to it's header */
	for (uint32_t ncmds = obj_swap32(o, header->ncmds); ncmds-- && err == 0;) {

		/* Peek the load command structure */
		struct load_command const *const lc = obj_peek(o, off, sizeof *lc);
		if (lc == NULL) return OFILE_E_INVAL_LC;

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
						   struct ofile_collector const *const collector,
						   void *user)
{
	struct fat_header const *const header = obj_peek(o, off, sizeof *header);
	if (header == NULL) return OFILE_E_INVAL_FATHDR;

	/* Retrieve the target architecture information,
	 * use an horrible hack here to get host arch info, hard-code bitch */
	NXArchInfo const *arch_info = o->target != OFILE_NX_HOST
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
	if (fat_archs == NULL) return OFILE_E_INVAL_FATARCH;

	/* We got an arch info target form user, so find the appropriate one
	 * in the fat arch array, if none exists, target all arch instead */
	if (arch_info != OFILE_NX_ALL) {
		bool find = false;

		for (uint32_t i = 0; i < nfat_arch; ++i) {
			struct fat_arch const *const arch = o->m64
				? (struct fat_arch *)((struct fat_arch_64 *)fat_archs + i)
				: (struct fat_arch *)fat_archs + i;

			NXArchInfo const *const info = NXGetArchInfoFromCpuType(
				(cpu_type_t)obj_swap32(o, (uint32_t) arch->cputype),
				(cpu_subtype_t) obj_swap32(o, (uint32_t)arch->cpusubtype));

			if (info == NULL)
				return (errno = EBADMACHO), OFILE_E_INVAL_ARCHINFO;

			/* Skip load if arch didn't match targeted one */
			if (arch_info->cputype == info->cputype &&
				arch_info->cpusubtype == info->cpusubtype)
				find = true;
		}

		/* In case of no match and user target, fail as not found */
		if (find == false && o->target && o->target != OFILE_NX_HOST)
			return OFILE_E_NOTFOUND_ARCH;

		/* In case of no match, target all arch instead */
		if (find == false) arch_info = OFILE_NX_ALL;
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
		int const err = load(arch_info, OFILE_FAT, o->buf + arch_off, arch_size,
		                     collector, user);
		if (err) return err;
	}

	return 0;
}

struct ar_info
{
	struct ar_hdr const *hdr;
	char const *name;
	size_t name_len;
	size_t size;
	uint8_t const *obj;
	uint8_t const *ranlibs;
	size_t ranlibs_size;
};

static int get_ar_hdr(struct obj const *const obj, size_t *const off,
                      struct ar_info *info)
{
	struct ar_hdr hdr_cpy;

	/* Peek AR header */
	info->hdr = obj_peek(obj, *off, sizeof *info->hdr);
	if (info->hdr == NULL) return OFILE_E_INVAL_ARHDR;
	*off += sizeof *info->hdr;

	/* Check for header consistency (must be "`\n") */
	if (ft_strcmp(ARFMAG, info->hdr->ar_fmag) != 0)
		return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

	/* Keep a local mutable copy of the AR header to safely insert null char */
	ft_memcpy(&hdr_cpy, info->hdr, sizeof hdr_cpy);

	/* We don't care about `ar_fmag` anymore, which is the field next to `size`,
	 * insert null terminated.. */
	hdr_cpy.ar_fmag[0] = '\0';

	/* Retrieve object */
	long long int const olen = ft_atoll(hdr_cpy.ar_size);
	if (olen <= 0) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;
	info->size = (size_t)olen;
	info->obj = obj_peek(obj, *off, info->size);
	if (info->obj == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

	size_t obj_off = *off;
	*off += info->size;

	/* We don't care about the date, which is the field next to `name`,
	 * insert null terminated.. */
	hdr_cpy.ar_date[0] = '\0';

	/* Short name (>= 15) */
	if (ft_strncmp(AR_EFMT1, hdr_cpy.ar_name, sizeof(AR_EFMT1) - 1) != 0) {

		/* When `ar_name` does not begin with "#1/" the name is in the
		 * ar_name filed and NULL terminated */
		info->name = info->hdr->ar_name;
		info->name_len = ft_strnlen(info->name, sizeof info->hdr->ar_name);

		/* Name must be null terminated so size cannot be same of the
		 * `ar_name` field */
		if (info->name_len >= sizeof info->hdr->ar_name)
			return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

	} else { /* Long name (> 15) */

		/* When `ar_name` begin with "#1/" the name is next to the header and
		 * it's length is next to "#1/" in `ar_name` */
		int const nlen = ft_atoi(hdr_cpy.ar_name + sizeof(AR_EFMT1) - 1);
		if (nlen <= 0 || (size_t)nlen >= info->size)
			return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

		info->name_len = (size_t)nlen;
		info->name = obj_peek(obj, obj_off, info->name_len);
		if (info->name == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

		obj_off += info->name_len;
		info->obj += info->name_len;
		info->size -= info->name_len;
	}

	if (obj_ism64(obj)) {
		uint64_t const *const size = obj_peek(obj, obj_off, sizeof *size);
		if (size == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

		info->ranlibs_size = (size_t)obj_swap64(obj, *size);
		obj_off += sizeof *size;

	} else {
		uint32_t const *const size = obj_peek(obj, obj_off, sizeof *size);
		if (size == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;

		info->ranlibs_size = (size_t)obj_swap32(obj, *size);
		obj_off += sizeof *size;
	}

	info->ranlibs = obj_peek(obj, obj_off, info->ranlibs_size);
	if (info->ranlibs == NULL) return (errno = EBADMACHO), OFILE_E_INVAL_ARHDR;
	obj_off += info->ranlibs_size;

	return obj_off >= *off ? ((errno = EBADMACHO), OFILE_E_INVAL_ARHDR) : 0;
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
						  const struct ofile_collector *const collector,
						  void *user)
{
	/* AR begin with a special magic: `!<arch>\n`, check for it */
	char const *const mag = obj_peek(obj, off, SARMAG);
	if (mag == NULL || ft_strncmp(ARMAG, mag, SARMAG) != 0)
		return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;
	off += SARMAG;

	struct ar_info info;
	int err = get_ar_hdr(obj, &off, &info);
	if (err) return err;

	size_t const nranlibs = info.ranlibs_size / sizeof(struct ranlib);

	for (size_t i = 0; i < nranlibs; ++i) {
		struct ar_info ran_info;
		size_t ran_off = ((struct ranlib *)info.ranlibs)[i].ran_off;

		err = get_ar_hdr(obj, &ran_off, &ran_info);
		if (err) return err;

		err = load(obj->target, OFILE_AR, ran_info.obj,
		           ran_info.size, collector, user);
		if (err) return err;
	}

	return 0;
}

/**
 * Different archive load
 * @param target     [in] Targeted arch
 * @param ofile      [in] Mach-o object ofile type
 * @param buf        [in] Mach-o object file buffer
 * @param size       [in] Mach-o object file size
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return                0 on success, error code otherwise with `errno` set
 */
static int load(NXArchInfo const *const target, enum ofile const ofile,
                uint8_t const *const buf, size_t const size,
                struct ofile_collector const *const collector, void *const user)
{
	static struct {
		uint32_t const magic;
		bool const m64, le;

		int (*const load)(struct obj const *, size_t,
						  struct ofile_collector const *, void *);
	} const loaders[] = {
		{ MH_MAGIC,     false, false, mh_load  },
		{ MH_CIGAM,     false, true,  mh_load  },
		{ MH_MAGIC_64,  true,  false, mh_load  },
		{ MH_CIGAM_64,  true,  true,  mh_load  },
		{ FAT_MAGIC,    false, false, fat_load },
		{ FAT_CIGAM,    false, true,  fat_load },
		{ FAT_MAGIC_64, true,  false, fat_load },
		{ FAT_CIGAM_64, true,  true,  fat_load },
		{ AR_MAGIC,     false, false, ar_load  },
		{ AR_CIGAM,     false, true,  ar_load  },
	};

	unsigned i;

	/* Using the Mach-o magic, find the appropriate loader,
	 * then retrieve LE mode, M64 and load function from it */
	for (i = 0; i < COUNT_OF(loaders) &&
				loaders[i].magic != *(uint32_t *)buf; ++i)

		/* When loading from FAT or AR, only expect MH */
		if (ofile > OFILE_MH && i == 3)
			return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;

	/* Unable to find proper loader for this magic */
	if (i == COUNT_OF(loaders))
		return (errno = EBADMACHO), OFILE_E_INVAL_MAGIC;

	struct obj const o = (struct obj){
		.target = target, .ofile = ofile,
		.buf = buf, .size = size,
		.m64 = loaders[i].m64, .le = loaders[i].le
	};

	/* Dispatch collection call-back's to correct loader */
	return loaders[i].load(&o, 0, collector, user);
}

/**
 * Collect though a Mach-o object
 */
int ofile_collect(char const *const filename, NXArchInfo const *const arch_info,
                  struct ofile_collector const *const collector,
                  void *const user)
{
	int const fd = open(filename, O_RDONLY);
	struct stat st;

	/* Open the given file and check for validity */
	if (fd < 0 || fstat(fd, &st) < 0)
		return -1;

	size_t const size = (size_t)st.st_size;

	if (size < sizeof(uint32_t)) return (errno = EBADMACHO), -1;
	if ((st.st_mode & S_IFDIR))  return (errno = EISDIR), -1;

	/* Then map the whole file into a buffer,
	 * mapped file read only */
	void *const buf = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (buf == MAP_FAILED || close(fd))
		return -1;

	/* Start loading,
	 * then unmap file and break */
	int const err = load(arch_info, OFILE_MH, buf, size, collector, user);
	munmap(buf, size);

	return err;
}
