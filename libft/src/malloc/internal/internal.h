/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc/internal/internal.h                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_INTERNAL_INTERNAL_H
# define MALLOC_INTERNAL_INTERNAL_H

# include "ft/malloc.h"
# include "ft/string.h"

# include <errno.h>

# define PAGE_SIZE (4096)

extern const struct s_mpool_ops	*g_ops[FT_MALLOC_MAX_MPOOL];
extern t_mpool					g_pools;

#endif
