/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   otool/main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj.h"

#include <ft/ctype.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>

static inline void dump(const char *text, uint64_t offset, uint64_t size)
{
	ft_printf("Contents of (__TEXT,__text) section\n");

	for (uint64_t i = 0; i < size; i += 0x10) {
		ft_printf("%016llx\t", offset + i);

		for (uint64_t j = 0; j < 0x10 && i + j < size; ++j)
			ft_printf("%02hhx ", text[i + j]);

		ft_printf("\n");
	}
}

static inline int section_dump(obj_t const obj, size_t off)
{
	/* Peek the section structure */
	const struct section *const sect = obj_peek(obj, off, sizeof *sect);

	if (sect == NULL)
		return -1;

	if (ft_strcmp("__TEXT", sect->segname) == 0 &&
		ft_strcmp("__text", sect->sectname) == 0) {

		uint64_t const addr = obj_swap32(obj, sect->addr);
		uint64_t const size = obj_swap32(obj, sect->size);

		const char *const text = obj_peek(obj, addr, size);

		if (text == NULL)
			return -1;

		/* It's a valid text section, dump it.. */
		dump(text, addr, size);
	}

	return 0;
}

static inline int section_64_dump(obj_t const obj, size_t off)
{
	/* Peek the section structure */
	const struct section_64 *const sect = obj_peek(obj, off, sizeof *sect);

	if (sect == NULL)
		return -1;

	if (ft_strcmp("__TEXT", sect->segname) == 0 &&
		ft_strcmp("__text", sect->sectname) == 0) {

		uint64_t const addr = obj_swap64(obj, sect->addr);
		uint64_t const size = obj_swap64(obj, sect->size);

		const char *const text = obj_peek(obj, addr, size);

		if (text == NULL)
			return -1;

		/* It's a valid text section, dump it.. */
		dump(text, addr, size);
	}

	return 0;
}

static int segment_collect(obj_t const obj, size_t off, void *const user)
{
	(void)user;
	const struct segment_command *const seg = obj_peek(obj, off, sizeof *seg);

	if (seg == NULL)
		return -1;

	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(obj))
		return -1;

	/* Only dump section's of __TEXT segment */
	if (ft_strcmp("__TEXT", seg->segname) != 0)
		return 0;

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(obj, seg->nsects); nsects--;) {

		static int (*const sect_dump[])(obj_t, size_t) = {
			[false] = section_dump,
			[true]  = section_64_dump
		};

		if (sect_dump[obj_ism64(obj)](obj, off))
			return -1;

		static const size_t section_size[] = {
			[false] = sizeof(struct section),
			[true]  = sizeof(struct section_64)
		};

		off += section_size[obj_ism64(obj)];
	}

	return 0;
}

/* otool collectors */
static const struct obj_collector nm_collector = {
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SEGMENT]    = segment_collect,
		[LC_SEGMENT_64] = segment_collect,
	}
};

int main(int ac, char *av[])
{
	const char *const exe = *av;
	int ret = EXIT_SUCCESS;

	/* There is no argument, try to dump the bin name `a.out` */
	if (ac < 2) {

		/* Collect Mach-o object using otool collectors */
		if (obj_collect("a.out", &nm_collector, NULL)) {

			/* Dump error, then continue.. */
			ft_fprintf(g_stderr, "%s: %s\n", exe, ft_strerror(errno));
			ret = EXIT_FAILURE;
		}

		return ret;
	}

	/* Loop though argument and dump each one */
	while (*++av) {

		/* Add some indication btw two output */
		if (ac > 2)
			ft_fprintf(g_stdout, "\n%s:\n", *av);

		/* Collect Mach-o object using otool collectors */
		if (obj_collect(*av, &nm_collector, NULL)) {

			/* Dump error, then continue.. */
			ft_fprintf(g_stderr, "%s: %s\n", exe, ft_strerror(errno));
			ret = EXIT_FAILURE;
		}
	}

	return ret;
}
