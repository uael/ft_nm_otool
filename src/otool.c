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

#include "ofile.h"

#include <ft/ctype.h>
#include <ft/opts.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>

static inline int dump(obj_t const o, uint64_t const off, uint64_t const addr,
                       uint64_t const size)
{
	ft_printf("Contents of (__TEXT,__text) section\n");

	char const *const text = obj_peek(o, off, size);
	if (text == NULL) return -1;

	/* Only dump byte per byte when we known the arch is i386 or x86_64 */
	bool const usual_dump = obj_arch(o) == NULL ||
	                        obj_arch(o)->cputype == CPU_TYPE_I386 ||
	                        obj_arch(o)->cputype == CPU_TYPE_X86_64;

	for (uint64_t i = 0; i < size; i += 0x10) {
		ft_printf("%0*llx\t", obj_ism64(o) ? 16 : 8, addr + i);

		if (usual_dump)
			for (uint64_t j = 0; j < 0x10 && i + j < size; ++j)
				ft_printf("%02hhx ", text[i + j]);
		else
			for (uint64_t j = 0; j < 4 && i + (j * 4) < size; ++j)
				ft_printf("%08x ", obj_swap32(o, ((uint32_t *)(text + i))[j]));

		ft_printf("\n");
	}
	return 0;
}

static int segment_collect(obj_t const o, size_t off, void *const user)
{
	(void)user;
	struct segment_command const *const seg = obj_peek(o, off, sizeof *seg);

	if (seg == NULL)
		return -1;

	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(o))
		return (errno = EBADARCH), -1;

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(o, seg->nsects); nsects--;) {

		/* Peek the section structure */
		struct section const *const sect = obj_peek(o, off, sizeof *sect);
		if (sect == NULL) return -1;

		if (ft_strcmp("__TEXT", sect->segname) == 0 &&
		    ft_strcmp("__text", sect->sectname) == 0) {

			uint32_t const offset = obj_swap32(o, sect->offset);
			uint32_t const addr   = obj_swap32(o, sect->addr);
			uint32_t const size   = obj_swap32(o, sect->size);

			/* It's a valid text section, dump it.. */
			return dump(o, offset, addr, size);
		}

		off += sizeof *sect;
	}

	return 0;
}

static int segment_64_collect(obj_t const o, size_t off, void *const user)
{
	(void)user;
	struct segment_command_64 const *const seg =
		obj_peek(o, off, sizeof *seg);
	if (seg == NULL) return -1;

	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(o))
		return (errno = EBADARCH), -1;

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(o, seg->nsects); nsects--;) {

		/* Peek the section structure */
		struct section_64 const *const sect = obj_peek(o, off, sizeof *sect);
		if (sect == NULL) return -1;

		if (ft_strcmp("__TEXT", sect->segname) == 0 &&
		    ft_strcmp("__text", sect->sectname) == 0) {

			uint64_t const offset = obj_swap64(o, sect->offset);
			uint64_t const addr   = obj_swap64(o, sect->addr);
			uint64_t const size   = obj_swap64(o, sect->size);

			/* It's a valid text section, dump it.. */
			return dump(o, offset, addr, size);
		}

		off += sizeof *sect;
	}

	return 0;
}

static void on_load(obj_t const o, void *user)
{
	char const *ctx = user;

	/* load call-back is called with NULL `o` on archive begin */
	if (obj_ofile(o) == OFILE_AR)
		return (void)ft_printf("Archive : %s\n", ctx);

	size_t name_len;

	/* In case of archive sub object, name is non-null */
	char const *const name = obj_name(o, &name_len);

	/* In case of FAT file and multiple arch, also print arch info name */
	bool const fat = obj_ofile(o) != OFILE_MH && obj_target(o) == OFILE_NX_ALL;

	ft_printf("%s", ctx);
	if (name) ft_printf("(%.*s)", (unsigned) name_len, name);
	if (fat)  ft_printf(" (architecture %s)",
						obj_arch(o) ? obj_arch(o)->name : "none");
	ft_printf(":\n");
}

/* otool collectors */
static struct ofile_collector const otool_collector = {
	.load = on_load,
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SEGMENT]    = segment_collect,
		[LC_SEGMENT_64] = segment_64_collect,
	}
};

int main(int ac, char *av[])
{
	char const *const exe = *av;
	int ret = EXIT_SUCCESS;
	char *ctx;
	char const *arch = NULL;
	t_opt const opts[] = {
		{ FT_OPT_STRING, 'A', "arch", &arch,
			"architecture(s) from a Mach-O file to dump", 0 },
		{ FT_OPT_END, 0, 0, 0, 0, 0 }
	};

	int i = 1;

	/* Parse options */
	if (ft_optparse(opts, &i, ac, av)) {

		/* Got an error while parsing options..
		 * show usage */
		ft_optusage(opts, av[0],
					"[file(s)]",
					"Hexdump [file(s)] (a.out by default).");

		return EXIT_FAILURE;
	}

	NXArchInfo const *target = NULL;

	if (arch == NULL) target = OFILE_NX_HOST;
	else if (ft_strcmp("all", arch) != 0 &&
			 (target = NXGetArchInfoFromName(arch)) == NULL) {

		/* Dump error, then abort.. */
		ft_fprintf(g_stderr, "%s: Unknown architecture named '%s'\n",
				   exe, arch);

		return EXIT_FAILURE;
	}

	/* Default file if none */
	if (ac < 2) av[ac++] = "a.out";

	/* Loop though argument and dump each one */
	for (; i < ac; ++i) {

		/* context is just filename */
		ctx = av[i];

		/* Collect Mach-o object using otool collectors */
		int const err = ofile_collect(ctx, target, &otool_collector, ctx);
		if (err) {

			/* Dump error, then continue.. */
			ft_fprintf(g_stderr, "%s: %s: %s\n", exe, ctx, ofile_etoa(err));
			ret = EXIT_FAILURE;
		}
	}

	return ret;
}
