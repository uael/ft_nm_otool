/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm_sym.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

#include <ft/ctype.h>
#include <ft/string.h>

inline char			sym_type(uint64_t const n_value, uint8_t const n_2[2],
						uint16_t n_desc, struct s_nm_context *const ctx)
{
	int const	type_field = N_TYPE & n_2[0];
	char		type;

	if (N_STAB & n_2[0])
		type = '-';
	else if (type_field == N_UNDF)
		type = (char)(n_value ? 'c' : 'u');
	else if (type_field == N_ABS)
		type = 'a';
	else if (type_field == N_SECT && ctx->sects[n_2[1] - 1])
		type = ctx->sects[n_2[1] - 1];
	else if (type_field == N_PBUD)
		type = 'u';
	else if (type_field == N_INDR)
		type = 'i';
	else if (n_desc & N_WEAK_REF)
		type = 'W';
	else
		type = '?';
	return (N_EXT & n_2[0] ? (char)ft_toupper(type) : type);
}

inline bool			sym_insert(struct s_nm_context *const ctx,
						struct s_sym *sym)
{
	if (!(ctx->flags & NM_OPT_a) && sym->type == '-')
		return (false);
	if ((ctx->flags & NM_OPT_g) && !ft_isupper(sym->type))
		return (false);
	if ((ctx->flags & NM_OPT_u) && ft_tolower(sym->type) != 'u')
		return (false);
	if ((ctx->flags & NM_OPT_uu) && sym->type != 'U')
		return (false);
	if (!sym->string)
		sym->string = "bad string index";
	return (true);
}

int					sym_n_cmp(const void *a, const void *b, size_t n)
{
	struct s_sym const *const	sym_a = a;
	struct s_sym const *const	sym_b = b;
	int							cmp;

	(void)n;
	cmp = (sym_a->off > sym_b->off) - (sym_a->off < sym_b->off);
	return (cmp == 0 ? ft_strcmp(sym_a->string, sym_b->string) : cmp);
}

int					sym_s_cmp(const void *a, const void *b, size_t n)
{
	struct s_sym const *const	sym_a = a;
	struct s_sym const *const	sym_b = b;
	int const					cmp = ft_strcmp(sym_a->string, sym_b->string);

	(void)n;
	return (cmp == 0
		? ((sym_a->off > sym_b->off) - (sym_a->off < sym_b->off))
		: cmp);
}
