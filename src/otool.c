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
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>

static inline void dump(const char *const text, uint64_t const off,
                        uint64_t const size, unsigned const padd)
{
	ft_printf("Contents of (__TEXT,__text) section\n");

	for (uint64_t i = 0; i < size; i += 0x10) {
		ft_printf("%0*llx\t", padd, off + i);

		for (uint64_t j = 0; j < 0x10 && i + j < size; ++j)
			ft_printf("%02hhx ", text[i + j]);

		ft_printf("\n");
	}
}

static int segment_collect(t_obj const o, size_t off, void *const user)
{
	(void)user;
	struct segment_command const *const seg = obj_peek(o, off, sizeof *seg);

	if (seg == NULL)
		return -1;

	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(o))
		return (errno = EBADARCH), -1;

	/* Only dump section's of __TEXT segment */
	if (ft_strcmp("__TEXT", seg->segname) != 0) return 0;

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(o, seg->nsects); nsects--;) {

		/* Peek the section structure */
		struct section const *const sect = obj_peek(o, off, sizeof *sect);
		if (sect == NULL) return -1;

		if (ft_strcmp("__TEXT", sect->segname) == 0 &&
		    ft_strcmp("__text", sect->sectname) == 0) {

			uint64_t const offset = obj_swap32(o, sect->offset);
			uint64_t const addr   = obj_swap32(o, sect->addr);
			uint64_t const size   = obj_swap32(o, sect->size);

			char const *const text = obj_peek(o, offset, size);
			if (text == NULL) return -1;

			/* It's a valid text section, dump it.. */
			dump(text, addr, size, 8);
			break;
		}

		off += sizeof *sect;
	}

	return 0;
}

static int segment_64_collect(t_obj const o, size_t off, void *const user)
{
	(void)user;
	struct segment_command_64 const *const seg =
		obj_peek(o, off, sizeof *seg);
	if (seg == NULL) return -1;

	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(o))
		return (errno = EBADARCH), -1;

	/* Only dump section's of __TEXT segment */
	if (ft_strcmp("__TEXT", seg->segname) != 0) return 0;

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

			char const *const text = obj_peek(o, offset, size);
			if (text == NULL) return -1;

			/* It's a valid text section, dump it.. */
			dump(text, addr, size, 16);
			break;
		}

		off += sizeof *sect;
	}

	return 0;
}

/* otool collectors */
static struct s_ofile_collector const otool_collector = {
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

	/* Default file if none */
	if (ac < 2) av[ac++] = "a.out";

	/* Loop though argument and dump each one */
	for (int i = 1; i < ac; ++i) {

		/* Indicate filename before hexdump */
		ft_printf("%s:\n", av[i]);

		/* Collect Mach-o object using otool collectors */
		int const err = ofile_collect(av[i], OFILE_NX_HOST,
		                              &otool_collector, NULL);
		if (err) {

			/* Dump error, then continue.. */
			ft_fprintf(g_stderr, "%s: %s: %s\n", exe, av[i], ofile_etoa(err));
			ret = EXIT_FAILURE;
		}
	}

	return ret;
}
