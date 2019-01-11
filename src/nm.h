/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H
# define NM_H

# include "ofile.h"

enum			e_nm_opt
{
	NM_OPT_a = (1 << 0),
	NM_OPT_g = (1 << 1),
	NM_OPT_n = (1 << 2),
	NM_OPT_p = (1 << 3),
	NM_OPT_r = (1 << 4),
	NM_OPT_u = (1 << 5),
	NM_OPT_uu = (1 << 6),
};

enum			e_nm_e
{
	NM_E_INVAL_SYMTAB = OFILE_E_MAX,
	NM_E_INVAL_STRTAB,
	NM_E_INVAL_NLIST,
	NM_E_INVAL_SYMTAB_ORDER,
	NM_E_INVAL_SEGMENT,
	NM_E_INVAL_SECTION,
	NM_E_INVAL_SECTION_COUNT,
	NM_E_INVAL_ARCH,
};

struct			s_nm_context
{
	char const	*bin;
	int			flags;
	char		sects[UINT8_MAX];
	uint8_t		nsects;
	int			nfiles;
};

struct			s_sym
{
	char const	*string;
	uint64_t	off;
	uint32_t	str_max_size;
	char		type;
};

int				symtab_collect(t_obj o, size_t off, void *user);
int				segment_collect(t_obj o, size_t off, void *user);
int				segment_64_collect(t_obj o, size_t off, void *user);

int				sym_n_cmp(const void *a, const void *b, size_t n);
int				sym_s_cmp(const void *a, const void *b, size_t n);
bool			sym_insert(struct s_nm_context *ctx, struct s_sym *sym);
char			sym_type(uint64_t n_value, uint8_t const n_2[2],
					uint16_t n_desc, struct s_nm_context *ctx);

#endif
