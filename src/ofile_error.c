/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_error.c                                      :+:      :+:    :+:   */
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

char const *ofile_etoa(int const err)
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
