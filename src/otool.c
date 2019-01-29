/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   otool.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "otool.h"

#include <ft/ctype.h>
#include <ft/opts.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>

static void								on_load(t_obj const o,
											NXArchInfo const *const arch_info,
											void *user)
{
	struct s_otool_context *const	ctx = user;
	bool							fat;

	fat = o->ofile != OFILE_MH && o->target == OFILE_NX_ALL;
	ft_printf("%s", ctx->bin);
	if (o->name)
		ft_printf("(%.*s)", (unsigned)o->name_len, o->name);
	else if (fat)
		ft_printf(" (architecture %s)",
			arch_info ? arch_info->name : "none");
	ft_printf(":\n");
}

static struct s_ofile_collector const	g_otool_collector = {
	.load = on_load,
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SEGMENT] = segment_collect,
		[LC_SEGMENT_64] = segment_64_collect,
	}
};

static int								otool_parse_arch(int i, char const *exe,
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

static int								otool_parse_opts(int ac, char *av[],
											NXArchInfo const **target)
{
	int			i;
	char const	*arch;
	t_opt const	opts[] = {
		{ FT_OPT_STRING, 'A', "arch", &arch, NULL, 0 },
		{ FT_OPT_END, 0, 0, 0, 0, 0 }};

	i = 1;
	arch = NULL;
	return (ft_optparse(opts, &i, ac, av)
		? -1 : otool_parse_arch(i, av[0], arch, target));
}

int										main(int ac, char *av[])
{
	struct s_otool_context	ctx;
	NXArchInfo const		*target;
	int						ret;
	int						err;
	int						i;

	target = NULL;
	if ((i = otool_parse_opts(ac, av, &target)) < 0)
		return (EXIT_FAILURE);
	if (i == ac)
		av[ac++] = "a.out";
	ret = EXIT_SUCCESS;
	while (i < ac)
	{
		ctx.bin = av[i++];
		if ((err = ofile_collect(ctx.bin, target, &g_otool_collector, &ctx)))
		{
			ft_fprintf(g_stderr, "%s: %s: %s\n",
				av[0], ctx.bin, ofile_etoa(err));
			ret = EXIT_FAILURE;
		}
	}
	if (target && target != OFILE_NX_HOST)
		NXFreeArchInfo(target);
	return (ret);
}
