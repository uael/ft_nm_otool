/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofile_swap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ofile.h"

uint16_t			obj_swap16(struct s_obj const *const obj, uint16_t u)
{
	return (obj->le ? OSSwapConstInt16(u) : u);
}

uint32_t			obj_swap32(struct s_obj const *const obj, uint32_t const u)
{
	return (obj->le ? OSSwapConstInt32(u) : u);
}

uint64_t			obj_swap64(struct s_obj const *const obj, uint64_t const u)
{
	return (obj->le ? OSSwapConstInt64(u) : u);
}
