/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OBJ_H
# define OBJ_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ar.h>
#include <mach-o/arch.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <mach-o/swap.h>

#define AR_CIGAM  (0x72613C21)
#define AR_MAGIC  (0x213C6172)

/**
 * Mach-o object type definition
 */
typedef const struct obj *obj_t;

enum
{
	OBJ_E_INVAL_MAGIC  = 1, /**< Invalid magic         */
	OBJ_E_INVAL_FATHDR,     /**< Invalid fat header    */
	OBJ_E_INVAL_FATARCH,    /**< Invalid fat archs     */
	OBJ_E_INVAL_ARCHINFO,   /**< Invalid arch info     */
	OBJ_E_INVAL_MHHDR,      /**< Invalid mach-o header */
	OBJ_E_INVAL_LC,         /**< Invalid load command  */
	OBJ_E_NOTFOUND_ARCH,    /**< Invalid arch match    */
	OBJ_E_FAT_RECURSION,    /**< Fat recursion         */
	OBJ_E_MAX
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


/* --- Architecture --- */

/**
 * Possible value to pass to `obj_collect` `arch_info` argument,
 * means collection target the host architecture
 */
#define OBJ_NX_HOST (NXArchInfo const *)(-1)

/**
 * Retrieve whatever Mach-o object is 64 bits based
 * @param obj  [in] Mach-o object
 * @return          true if object is a 64 bits object, false otherwise
 */
bool obj_ism64(obj_t obj);

/**
 * Retrieve whatever Mach-o object is fat
 * @param obj  [in] Mach-o object
 * @return          Whatever object is fat
 */
bool obj_isfat(struct obj const *obj);

/**
 * Retrieve targeted Mach-o object architecture info
 * @param obj  [in] Mach-o object
 * @return          Architecture info or NULL if all
 */
NXArchInfo const *obj_target(struct obj const *obj);


/* --- Collection --- */

/**
 * Peek sized data at offset on Mach-o object
 * @param obj  [in] Mach-o object
 * @param off  [in] Offset where to peek data
 * @param len  [in] Size of data to peek
 * @return          Data on success, NULL otherwise with `errno` set
 */
const void *obj_peek(obj_t obj, size_t off, size_t len);

/**
 * Object collector call-back type definition
 */
typedef int obj_collector_t(obj_t, NXArchInfo const *, size_t off, void *user);

/**
 * Object collector definition
 */
struct obj_collector
{
	size_t ncollector; /**< Actual max size of `collectors` field */
	obj_collector_t *const collectors[]; /**< Collectors array */
};

/**
 * Collect though a Mach-o object
 * @param filename   [in] Path of the mach-o object in the system
 * @param arch_info  [in] Arch to collect, OBJ_NX_HOST for host and NULL for all
 * @param collector  [in] User collection call-back's
 * @param user       [in] Optional user parameter
 * @return                0 on success, -1 otherwise with `errno` set
 */
int obj_collect(const char *filename, NXArchInfo const *arch_info,
				const struct obj_collector *collector, void *user);


#endif /* !OBJ_H */
