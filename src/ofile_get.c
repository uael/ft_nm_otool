/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_get.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

#include <errno.h>

bool				obj_ism64(struct s_obj const *const obj)
{
	return (obj->m64);
}

enum e_ofile		obj_ofile(struct s_obj const *const obj)
{
	return (obj->ofile);
}

NXArchInfo const	*obj_target(struct s_obj const *const obj)
{
	return (obj->target);
}

char const			*obj_name(struct s_obj const *const obj, size_t *out_len)
{
	if (out_len && obj->name_len)
		*out_len = obj->name_len;
	return (obj->name);
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
