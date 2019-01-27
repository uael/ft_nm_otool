/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OFILE_H
# define OFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ar.h>
#include <mach-o/arch.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/ranlib.h>
#include <mach-o/stab.h>
#include <mach-o/swap.h>


#define AR_CIGAM  (0x72613C21)
#define AR_MAGIC  (0x213C6172)

/**
 * Mach-o object type definition
 */
typedef struct obj const *obj_t;

/**
 * Ofile error codes
 */
enum
{
	OFILE_E_INVAL_MAGIC  = 1, /**< Invalid magic                 */
	OFILE_E_INVAL_FATHDR,     /**< Invalid fat header            */
	OFILE_E_INVAL_FATARCH,    /**< Invalid fat archs             */
	OFILE_E_INVAL_ARCHINFO,   /**< Invalid arch info             */
	OFILE_E_INVAL_ARCHOBJ,    /**< Invalid arch object           */
	OFILE_E_INVAL_MHHDR,      /**< Invalid mach-o header         */
	OFILE_E_INVAL_ARHDR,      /**< Invalid archive header        */
	OFILE_E_NO_ARHDR,         /**< Invalid archive header size   */
	OFILE_E_INVAL_AROBJHDR,   /**< Invalid archive object header */
	OFILE_E_INVAL_LC,         /**< Invalid load command          */
	OFILE_E_NOTFOUND_ARCH,    /**< Invalid arch match            */
	OFILE_E_MAX
};

/**
 * Retrieve the string representation of an error code
 * @param err  [in] Error code
 * @return          string representation `err`
 */
char const *ofile_etoa(int err);

/**
 * Ofile types
 */
enum ofile
{
	OFILE_MH = 0, /**< Mach-o ofile  */
	OFILE_FAT,    /**< FAT ofile     */
	OFILE_AR,     /**< Archive ofile */
};


/* --- Endianness --- */

/**
 * Swap 16 bits unsigned integer according to object endianness
 * @param obj  [in] Mach-o object
 * @param u    [in] 16 bits unsigned integer
 * @return          16 bits unsigned integer, swapped if necessary
 */
uint16_t obj_swap16(obj_t obj, uint16_t u);

/**
 * Swap 32 bits unsigned integer according to object endianness
 * @param obj  [in] Mach-o object
 * @param u    [in] 32 bits unsigned integer
 * @return          32 bits unsigned integer, swapped if necessary
 */
uint32_t obj_swap32(obj_t obj, uint32_t u);

/**
 * Swap 64 bits unsigned integer according to object endianness
 * @param obj  [in] Mach-o object
 * @param u    [in] 64 bits unsigned integer
 * @return          64 bits unsigned integer, swapped if necessary
 */
uint64_t obj_swap64(obj_t obj, uint64_t u);


/* --- Mach-o object getter --- */

/**
 * Possible value to pass to `ofile_collect` `arch_info` argument,
 * means collection target the host architecture
 */
#define OFILE_NX_HOST (NXArchInfo const *)(  -1)
#define OFILE_NX_ALL  (NXArchInfo const *)(NULL)

/**
 * Retrieve whatever Mach-o object is 64 bits based
 * @param obj  [in] Mach-o object
 * @return          true if object is a 64 bits object, false otherwise
 */
bool obj_ism64(obj_t obj);

/**
 * Retrieve Mach-o object ofile type
 * @param obj  [in] Mach-o object
 * @return          object ofile type
 */
enum ofile obj_ofile(obj_t obj);

/**
 * Retrieve targeted Mach-o object architecture info
 * @param obj  [in] Mach-o object
 * @return          Architecture info or NULL if all
 */
NXArchInfo const *obj_target(obj_t obj);

/**
 * Retrieve Mach-o object architecture info
 * @param obj  [in] Mach-o object
 * @return          Architecture info or NULL if all
 */
NXArchInfo const *obj_arch(obj_t obj);

/**
 * Retrieve Mach-o object name, only for archive
 * @param obj       [in] Mach-o object
 * @param out_len  [out] Mach-o object out name length
 * @return               Mach-o object name
 */
char const *obj_name(obj_t obj, size_t *out_len);

/**
 * Peek sized data at offset on Mach-o object
 * @param obj  [in] Mach-o object
 * @param off  [in] Offset where to peek data
 * @param len  [in] Size of data to peek
 * @return          Data on success, NULL otherwise with `errno` set
 */
const void *obj_peek(obj_t obj, size_t off, size_t len);


/* --- Collection --- */

/**
 * Object file collector call-back type definition
 */
typedef int ofile_collector_t(obj_t obj, size_t off, void *user);

/**
 * Object file collector definition
 */
struct ofile_collector
{
	void (*load)(obj_t obj, void *user);

	size_t ncollector; /**< Actual max size of `collectors` field */
	ofile_collector_t *const collectors[]; /**< Collectors array */
};

/**
 * Collect though a Mach-o object
 * @param filename   [in] Path of the mach-o object in the system
 * @param target     [in] Arch to collect, see OFILE_NX_*
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return                0 on success, -1 otherwise with `errno` set
 */
int ofile_collect(const char *filename, NXArchInfo const *target,
                  const struct ofile_collector *collector, void *user);


#endif /* !OFILE_H */
