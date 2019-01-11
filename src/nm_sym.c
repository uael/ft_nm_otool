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
#include <ft/stdio.h>
#include <ft/stdlib.h>
#include <ft/string.h>

#include <errno.h>
#include <stdlib.h>

static size_t const	g_nl_sizes[] = {
	[false] = sizeof(struct nlist),
	[true] = sizeof(struct nlist_64)
};

static int			nlist_peek(t_obj const o, size_t const off,
						void const **nlist, uint32_t *idxs)
{
	struct symtab_command const *const	symt = obj_peek(o, off, sizeof(*symt));

	if (symt == NULL)
		return (NM_E_INVAL_SYMTAB);
	idxs[1] = obj_swap32(o, symt->stroff);
	idxs[2] = obj_swap32(o, symt->strsize);
	if (!obj_peek(o, idxs[1], idxs[2]))
		return (NM_E_INVAL_STRTAB);
	idxs[0] = obj_swap32(o, symt->nsyms);
	*nlist = obj_peek(o, obj_swap32(o, symt->symoff),
		g_nl_sizes[o->m64] * idxs[0]);
	return (*nlist == NULL ? NM_E_INVAL_NLIST : 0);
}

static void			nlist_value_peek(t_obj const o, void const *nls,
						uint32_t const *idxs, uint64_t *values)
{
	struct nlist_64 const	*nl_64;
	struct nlist const		*nl;

	if (o->m64)
	{
		nl_64 = (struct nlist_64 *)nls + idxs[3] - 1;
		values[0] = obj_swap64(o, nl_64->n_value);
		values[1] = obj_swap32(o, nl_64->n_un.n_strx);
		values[2] = obj_swap16(o, nl_64->n_desc);
		values[3] = nl_64->n_type;
		values[4] = nl_64->n_sect;
	}
	else
	{
		nl = (struct nlist *)nls + idxs[3] - 1;
		values[0] = obj_swap32(o, nl->n_value);
		values[1] = obj_swap32(o, nl->n_un.n_strx);
		values[2] = obj_swap16(o, (uint16_t)nl->n_desc);
		values[3] = nl->n_type;
		values[4] = nl->n_sect;
	}
}

static struct s_sym	nlist_sym_peek(t_obj const o, uint32_t const *idxs,
						uint64_t const *values, void *user)
{
	struct s_sym	sym;
	uint8_t			v[2];

	v[0] = (uint8_t)values[3];
	v[1] = (uint8_t)values[4];
	sym.str_max_size = idxs[1] + idxs[2] - idxs[5];
	sym.string = obj_peek(o, idxs[5], idxs[1] + idxs[2] - idxs[5]);
	sym.type = sym_type(values[0], v, obj_swap16(o, (uint16_t)values[2]), user);
	sym.off = values[0];
	return (sym);
}

static int			syms_dump(t_obj const o, struct s_sym *syms, uint32_t nsyms,
						struct s_nm_context *const ctx)
{
	int const		padding = o->m64 ? 16 : 8;
	uint32_t		i;
	struct s_sym	*sym;

	if (!(ctx->flags & NM_OPT_p))
		ft_qsort(syms, nsyms, sizeof(*syms),
			(ctx->flags & NM_OPT_n) ? sym_n_cmp : sym_s_cmp);
	i = 0;
	while (i < nsyms)
	{
		sym = syms + ((ctx->flags & NM_OPT_r) ? nsyms - 1 - i : i);
		if (sym->off || !(sym->type == 'u' || sym->type == 'U'))
			ft_printf("%0*llx %c %.*s\n",
				padding, sym->off, sym->type, sym->str_max_size,
				sym->string);
		else
			ft_printf("  %*c %.*s\n",
				padding, sym->type, sym->str_max_size, sym->string);
		++i;
	}
	free(syms);
	return (0);
}

int					symtab_collect(t_obj const o, size_t const off,
						void *const user)
{
	int				err;
	void const		*nls;
	uint32_t		idxs[6];
	uint64_t		values[5];
	struct s_sym	*syms;

	if (((struct s_nm_context *)user)->nsects == 0)
		return ((errno = EBADMACHO) & 0 + NM_E_INVAL_SYMTAB_ORDER);
	ft_memset(idxs, 0, sizeof(idxs));
	if ((err = nlist_peek(o, off, &nls, idxs)))
		return (err);
	if ((syms = malloc(idxs[0] * sizeof(*syms))) == NULL)
		return (-1);
	ft_memset(syms, 0, idxs[0] * sizeof(*syms));
	while (idxs[3]++ < idxs[0])
	{
		nlist_value_peek(o, nls, idxs, values);
		idxs[5] = idxs[1] + (uint32_t)values[1];
		syms[idxs[4]] = nlist_sym_peek(o, idxs, values, user);
		if (sym_insert(user, syms + idxs[4]))
			++idxs[4];
	}
	return (syms_dump(o, syms, idxs[4], user));
}
