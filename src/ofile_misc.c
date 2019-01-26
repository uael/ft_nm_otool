/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_misc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <ft/stdlib.h>

#include <errno.h>

char const			*ofile_etoa(int const err)
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

uint16_t			obj_swap16(struct s_obj const *const obj, uint16_t u)
{
	return (obj->le ? ((uint16_t)
		((((uint16_t)(u) & 0xff00) >> 8)
		| (((uint16_t)(u) & 0x00ff) << 8))) : u);
}

uint32_t			obj_swap32(struct s_obj const *const obj, uint32_t const u)
{
	return (obj->le ? ((uint32_t)
		((((uint32_t)(u) & 0xff000000) >> 24)
		| (((uint32_t)(u) & 0x00ff0000) >> 8)
		| (((uint32_t)(u) & 0x0000ff00) << 8)
		| (((uint32_t)(u) & 0x000000ff) << 24))) : u);
}

uint64_t			obj_swap64(struct s_obj const *const obj, uint64_t const u)
{
	return (obj->le ? ((uint64_t)
		((((uint64_t)(u) & 0xff00000000000000ULL) >> 56)
		| (((uint64_t)(u) & 0x00ff000000000000ULL) >> 40)
		| (((uint64_t)(u) & 0x0000ff0000000000ULL) >> 24)
		| (((uint64_t)(u) & 0x000000ff00000000ULL) >> 8)
		| (((uint64_t)(u) & 0x00000000ff000000ULL) << 8)
		| (((uint64_t)(u) & 0x0000000000ff0000ULL) << 24)
		| (((uint64_t)(u) & 0x000000000000ff00ULL) << 40)
		| (((uint64_t)(u) & 0x00000000000000ffULL) << 56))) : u);
}

inline const void	*obj_peek(struct s_obj const *const obj, size_t const off,
						size_t const len)
{
	if (len == 0 || off + len > obj->size)
	{
		errno = EBADMACHO;
		return (NULL);
	}
	return (obj->buf + off);
}
