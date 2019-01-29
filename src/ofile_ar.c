/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_ar.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <ft/stdlib.h>
#include <ft/string.h>

#include <errno.h>

static int		ar_obj_peek(struct s_obj const *const obj, size_t *off,
					struct ar_hdr const *hdr, struct s_ar_info *const info)
{
	char			data[sizeof(hdr->ar_size) + 1];
	long long int	olen;

	ft_memcpy(&data, hdr->ar_size, sizeof(hdr->ar_size));
	data[sizeof(hdr->ar_size)] = '\0';
	olen = ft_atoll(data);
	if (olen <= 0)
		return (OFILE_E_INVAL_ARHDR);
	info->size = (size_t)olen;
	info->obj = obj_peek(obj, *off, info->size);
	if (info->obj == NULL)
		return (OFILE_E_INVAL_ARHDR);
	return (0);
}

static int		ar_name_peek(struct s_obj const *const obj, size_t *off,
					struct ar_hdr const *hdr, struct s_ar_info *const info)
{
	char	data[sizeof(hdr->ar_name) + 1];
	int		nlen;
	char	*spaces;

	ft_memcpy(&data, hdr->ar_name, sizeof(hdr->ar_name));
	data[sizeof(hdr->ar_name)] = '\0';
	if (ft_strncmp(AR_EFMT1, data, sizeof(AR_EFMT1) - 1) != 0)
	{
		if ((spaces = ft_strchr(data, ' ')))
			*spaces = '\0';
		info->name = hdr->ar_name;
		info->name_len = ft_strnlen(data, sizeof(hdr->ar_name));
		return (0);
	}
	nlen = ft_atoi(data + sizeof(AR_EFMT1) - 1);
	if (nlen <= 0 || (size_t)nlen >= info->size)
		return (OFILE_E_INVAL_ARHDR);
	info->name_len = (size_t)nlen;
	if ((info->name = obj_peek(obj, *off, info->name_len)) == NULL)
		return (OFILE_E_INVAL_ARHDR);
	*off += info->name_len;
	info->obj += info->name_len;
	info->size -= info->name_len;
	return (0);
}

static int		ar_info_peek(struct s_obj const *const obj, size_t *off,
					struct s_ar_info *const info)
{
	struct ar_hdr const *const	hdr = obj_peek(obj, *off, sizeof(*hdr));
	int							err;

	if (hdr == NULL)
		return (OFILE_E_NO_ARHDR);
	*off += sizeof(*hdr);
	if (ft_strncmp(ARFMAG, hdr->ar_fmag, sizeof(ARFMAG) - 1) != 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_ARHDR);
	}
	if ((err = ar_obj_peek(obj, off, hdr, info))
		|| (err = ar_name_peek(obj, off, hdr, info)))
	{
		errno = EBADMACHO;
		return (err);
	}
	return (0);
}

static int		ar_collect(struct s_obj const *const obj, size_t off,
					struct s_ofile_collector const *const collector,
					void *const user)
{
	struct s_ar_info	info;
	int					err;

	while ((err = ar_info_peek(obj, &off, &info)) == 0)
	{
		if ((err = load(&(struct s_obj){
			.ofile = OFILE_AR, .target = obj->target,
			.buf = info.obj, .size = info.size,
			.name = info.name, .name_len = info.name_len, }, collector, user)))
			return (err);
		off += info.size;
	}
	return (err == OFILE_E_NO_ARHDR ? 0 : err);
}

int				ar_load(struct s_obj const *const obj,
					struct s_ofile_collector const *const collector,
					void *const user)
{
	char const *const	mag = obj_peek(obj, 0, SARMAG);
	struct s_ar_info	info;
	size_t				off;
	int					err;

	if (mag == NULL || ft_strncmp(ARMAG, mag, SARMAG) != 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_MAGIC);
	}
	off = SARMAG;
	if ((err = ar_info_peek(obj, &off, &info)))
		return (err);
	if (ft_strncmp(info.name, SYMDEF, info.name_len) != 0
		&& ft_strncmp(info.name, SYMDEF_SORTED, info.name_len) != 0
		&& ft_strncmp(info.name, SYMDEF_64, info.name_len) != 0
		&& ft_strncmp(info.name, SYMDEF_64_SORTED, info.name_len) != 0)
	{
		errno = EBADMACHO;
		return (OFILE_E_INVAL_MAGIC);
	}
	return (ar_collect(obj, off + info.size, collector, user));
}
