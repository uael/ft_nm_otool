/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft/cdefs.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CDEFS_H
# define FT_CDEFS_H

# include <stdint.h>
# include <stddef.h>

# ifndef COUNT_OF
#  define COUNT_OF(x) (sizeof(x) / sizeof(*(x)))
# endif

# ifndef OFFSET_OF
#  define OFFSET_OF(t, f) ((size_t)((uintptr_t)&(((t*)(0))->f)-(uintptr_t)(0)))
# endif

# ifndef CONTAINER_OF
#  define CONTAINER_OF(p, t, f) ((t *)((uintptr_t)(p) - OFFSET_OF(t, f)))
# endif

#endif
