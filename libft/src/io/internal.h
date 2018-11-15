/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io/internal.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/25 00:42:42 by alucas-           #+#    #+#             */
/*   Updated: 2018/02/25 00:42:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IO_INTERNAL_H
# define IO_INTERNAL_H

# include "libft/io.h"

# define FT_FPERM (1 << 0)
# define FT_FNORD (1 << 1)
# define FT_FERRO (1 << 2)
# define FT_FNOWR (1 << 3)

# define FT_BUFSIZ (4096)

size_t	fwritex(t_stream *f, uint8_t const *s, size_t l);
size_t	stdiowrite(t_stream *f, uint8_t const *s, size_t len);

#endif
