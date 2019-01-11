/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "nm.h"

#include <ft/opts.h>
#include <ft/stdio.h>

#include <errno.h>
#include <stdlib.h>

static inline char const				*nm_etoa(int const err)
{
	if (err == NM_E_INVAL_SYMTAB)
		return ("Invalid symtab size");
	if (err == NM_E_INVAL_NLIST)
		return ("Invalid nlist table size");
	if (err == NM_E_INVAL_SYMTAB_ORDER)
		return ("Invalid symtab order");
	if (err == NM_E_INVAL_SEGMENT)
		return ("Invalid segment size");
	if (err == NM_E_INVAL_SECTION)
		return ("Invalid section size");
	if (err == NM_E_INVAL_SECTION_COUNT)
		return ("Invalid section count size");
	if (err == NM_E_INVAL_ARCH)
		return ("Architecture miss-match");
	return (ofile_etoa(err));
}

static void								on_load(t_obj const o,
											NXArchInfo const *const arch_info,
											void *user)
{
	struct s_nm_context *const	ctx = user;
	bool						fat;

	fat = o->ofile != OFILE_MH && o->target == OFILE_NX_ALL;
	if (o->name || fat)
	{
		ft_printf("\n%s", ctx->bin);
		if (o->name)
			ft_printf("(%.*s)", (unsigned)o->name_len, o->name);
		if (fat)
			ft_printf(" (for architecture %s)",
				arch_info ? arch_info->name : "none");
		ft_printf(":\n");
	}
	else if (ctx->nfiles > 1)
		ft_printf("\n%s:\n", ctx->bin);
	ctx->nsects = 0;
	ft_memset(ctx->sects, 0, sizeof(ctx->sects));
}

static const struct s_ofile_collector	g_nm_collector = {
	.load = on_load,
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SYMTAB] = symtab_collect,
		[LC_SEGMENT] = segment_collect,
		[LC_SEGMENT_64] = segment_64_collect,
	}
};

static int								nm_parse_arch(int i, char const *exe,
											char const *arch,
											NXArchInfo const **target)
{
	if (arch == NULL)
		*target = OFILE_NX_HOST;
	else if (ft_strcmp("all", arch) != 0
		&& (*target = NXGetArchInfoFromName(arch)) == NULL)
	{
		ft_fprintf(g_stderr, "%s: Unknown architecture named '%s'\n",
			exe, arch);
		return (-1);
	}
	return (i);
}

static int								nm_parse_opts(int ac, char *av[],
											int *flags,
											NXArchInfo const **target)
{
	int			i;
	char const	*arch;
	t_opt const	opts[] = {
		{ FT_OPT_BOOLEAN, 'a', "debug-syms", flags, NULL, NM_OPT_a },
		{ FT_OPT_BOOLEAN, 'g', "extern-only", flags, NULL, NM_OPT_g },
		{ FT_OPT_BOOLEAN, 'n', "numeric-sort", flags, NULL, NM_OPT_n },
		{ FT_OPT_BOOLEAN, 'p', "no-sort", flags, NULL, NM_OPT_p },
		{ FT_OPT_BOOLEAN, 'r', "reverse-sort", flags, NULL, NM_OPT_r },
		{ FT_OPT_BOOLEAN, 'u', "undefined-only", flags, NULL, NM_OPT_u },
		{ FT_OPT_BOOLEAN, 'U', "ext-undefined-only", flags, NULL, NM_OPT_uu },
		{ FT_OPT_STRING, 'A', "arch", &arch, NULL, 0 },
		{ FT_OPT_END, 0, 0, 0, 0, 0 }};

	i = 1;
	*flags = 0;
	return (ft_optparse(opts, &i, ac, av)
		? -1 : nm_parse_arch(i, av[0], arch, target));
}

int										main(int ac, char *av[])
{
	struct s_nm_context	ctx;
	NXArchInfo const	*target;
	int					err;
	int					ret;
	int					i;

	target = NULL;
	if ((i = nm_parse_opts(ac, av, &ctx.flags, &target)) < 0)
		return (EXIT_FAILURE);
	if (i == ac)
		av[ac++] = "a.out";
	ret = EXIT_SUCCESS;
	ctx.nfiles = ac - i;
	while (i < ac)
	{
		ctx.bin = av[i++];
		if ((err = ofile_collect(ctx.bin, target, &g_nm_collector, &ctx)))
		{
			ft_fprintf(g_stderr, "%s: %s: %s\n", av[0], ctx.bin, nm_etoa(err));
			ret = EXIT_FAILURE;
		}
	}
	if (target && target != OFILE_NX_HOST)
		NXFreeArchInfo(target);
	return (ret);
}
