/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm/main.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#             */
/*   Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "obj.h"

#include <ft/ctype.h>
#include <ft/opts.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>


enum
{
	NM_OPT_a = (1 << 0),
	NM_OPT_g = (1 << 1),
	NM_OPT_n = (1 << 2),
	NM_OPT_p = (1 << 3),
	NM_OPT_r = (1 << 4),
	NM_OPT_u = (1 << 5),
	NM_OPT_U = (1 << 6),
};

struct nm_context
{
	int flags;
	char sections[UINT8_MAX];
	uint8_t nsects;
};

struct sym
{
	struct sym *next;

	const char *string;
	uint64_t off;
	uint32_t str_max_size;
	char type;
};

static inline int syms_dump(t_stream *const s, struct sym *head, unsigned padd)
{
	for (; head; head = head->next) {

		if (head->off || !(head->type == 'u' || head->type == 'U'))
			ft_fprintf(s, "%0*lx %c %.*s\n",
					   padd, head->off, head->type, head->str_max_size,
					   head->string);

		else
			ft_fprintf(s, "  %*c %.*s\n",
					   padd, head->type, head->str_max_size, head->string);
	}

	return 0;
}

static inline char sym_type(uint64_t const n_value, uint8_t n_type,
							uint8_t n_sect, uint16_t n_desc,
							struct nm_context *const ctx)
{
	int const type_field = N_TYPE & n_type;
	char type;

	if (N_STAB & n_type)
		type = '-';
	else if (type_field == N_UNDF)
		type = (char)(n_value ? 'c' : 'u');
	else if (type_field == N_ABS)
		type = 'a';
	else if (type_field == N_SECT && ctx->sections[n_sect - 1])
		type = ctx->sections[n_sect - 1];
	else if (type_field == N_PBUD)
		type = 'u';
	else if (type_field == N_INDR)
		type = 'i';
	else if (n_desc & N_WEAK_REF)
		type = 'W';
	else
		type = '?';
	return N_EXT & n_type ? (char)ft_toupper(type) : type;

}

static inline void sym_insert(struct nm_context *const ctx,
							  struct sym syms[], uint32_t i, struct sym **head)
{
	struct sym **it;

	/* Skip debug symbol if option `a` isn't active */
	if (!(ctx->flags & NM_OPT_a) && syms[i].type == '-')
		return;

	/* Skip non-external symbol if option `g` is active */
	if ((ctx->flags & NM_OPT_g) && !ft_isupper(syms[i].type))
		return;

	/* Skip non-undefined symbol if option `u` is active */
	if ((ctx->flags & NM_OPT_u) && ft_tolower(syms[i].type) != 'u')
		return;

	/* Skip non-external-undefined symbol if option `u` is active */
	if ((ctx->flags & NM_OPT_U) && syms[i].type != 'U')
		return;

	/* Insert symbol */
	if (!(ctx->flags & NM_OPT_p)) {

		/* Save some instructions by using sorted direct insertion
		 * into the symbol linked list */
		for (it = head; *it; it = &(*it)->next) {

			/* Compare two symbol, alphabetically by default,
			 * numerically by address if the `n` option is active */
			int const cmp = (ctx->flags & NM_OPT_n)
				? (syms[i].off > (*it)->off) - (syms[i].off < (*it)->off)
				: ft_strcmp(syms[i].string, (*it)->string);

			/* Check if we get the right position
			 * inverse comparison if the `r` option is active */
			if ((ctx->flags & NM_OPT_r) ? cmp >= 0 : cmp <= 0) {
				syms[i].next = *it;
				break;
			}
		}

	} else {

		it = head;

		/* No sort (reverse), insert at head */
		if ((ctx->flags & NM_OPT_r)) syms[i].next = *it;

		/* No sort insert at tail */
		else for (; *it; it = &(*it)->next);
	}

	/* Iterator got set, insert symbol */
	*it = syms + i;
}

static inline int symtab_32_collect(obj_t const obj, size_t const off,
									void *const user)
{
	struct nm_context *const ctx = user;

	/* Peek the symtab structure */
	const struct symtab_command *const symtab =
		obj_peek(obj, off, sizeof *symtab);

	if (symtab == NULL)
		return -1;

	uint32_t const stroff = obj_swap32(obj, symtab->stroff);
	uint32_t const strsize = obj_swap32(obj, symtab->strsize);

	/* Check for str table validity */
	if (obj_peek(obj, stroff, strsize) == NULL)
		return -1;

	uint32_t const symoff = obj_swap32(obj, symtab->symoff);
	uint32_t const nsyms = obj_swap32(obj, symtab->nsyms);

	/* Peek the nlist structure array */
	const struct nlist *const nlist =
		obj_peek(obj, symoff, sizeof(*nlist) * nsyms);

	if (nlist == NULL)
		return -1;

	struct sym *head = NULL, syms[nsyms];

	/* For each nlist insert the symbol into `head` singly linked list
	 * Options will affect insertion */
	for (uint32_t i = 0; i < nsyms; ++i) {

		const uint32_t offset = stroff + obj_swap32(obj, nlist[i].n_un.n_strx);

		syms[i] = (struct sym){
			.str_max_size = stroff + strsize - offset,
			.string = obj_peek(obj, offset, stroff + strsize - offset),
			.type = sym_type(obj_swap64(obj, nlist[i].n_value), nlist[i].n_type,
							 nlist[i].n_sect,
							 obj_swap16(obj, (uint16_t)nlist[i].n_desc), ctx),
			.off = obj_swap32(obj, nlist[i].n_value)
		};

		/* Got a valid symbol, insert it */
		sym_insert(ctx, syms, i, &head);
	}

	return syms_dump(g_stdout, head, 8);
}

static inline int symtab_64_collect(obj_t const obj, size_t const off,
									void *const user)
{
	struct nm_context *const ctx = user;

	/* Peek the symtab structure */
	const struct symtab_command *const symtab =
		obj_peek(obj, off, sizeof *symtab);

	if (symtab == NULL)
		return -1;

	uint32_t const stroff = obj_swap32(obj, symtab->stroff);
	uint32_t const strsize = obj_swap32(obj, symtab->strsize);

	/* Check for str table validity */
	if (obj_peek(obj, stroff, strsize) == NULL)
		return -1;

	uint32_t const symoff = obj_swap32(obj, symtab->symoff);
	uint32_t const nsyms = obj_swap32(obj, symtab->nsyms);

	/* Peek the nlist structure array */
	const struct nlist_64 *const nlist =
		obj_peek(obj, symoff, sizeof(*nlist) * nsyms);

	if (nlist == NULL)
		return -1;

	struct sym *head = NULL, syms[nsyms];

	/* For each nlist insert the symbol into `head` singly linked list
	 * Options will affect insertion */
	for (uint32_t i = 0; i < nsyms; ++i) {

		const uint32_t offset = stroff + obj_swap32(obj, nlist[i].n_un.n_strx);

		syms[i] = (struct sym){
			.str_max_size = stroff + strsize - offset,
			.string = obj_peek(obj, offset, stroff + strsize - offset),
			.type = sym_type(obj_swap64(obj, nlist[i].n_value), nlist[i].n_type,
							 nlist[i].n_sect, obj_swap16(obj, nlist[i].n_desc),
							 ctx),
			.off = obj_swap64(obj, nlist[i].n_value)
		};

		/* Got a valid symbol, insert it */
		sym_insert(ctx, syms, i, &head);
	}

	return syms_dump(g_stdout, head, 16);
}

static int symtab_collect(obj_t const obj, const size_t off, void *const user)
{
	struct nm_context *const ctx = user;

	/* Symtab before text section ? */
	if (ctx->nsects == 0)
		return -1;

	static obj_collector_t *const collectors[] = {
		[false] = symtab_32_collect,
		[true]  = symtab_64_collect
	};

	return collectors[obj_ism64(obj)](obj, off, user);
}

static int segment_collect(obj_t const obj, size_t off, void *const user)
{
	struct nm_context *const ctx = user;

	/* Peek the segment structure */
	const struct segment_command *const seg = obj_peek(obj, off, sizeof *seg);

	if (seg == NULL)
		return -1;

	/* Check for architecture miss-match */
	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(obj)) {
		errno = EBADMACHO;
		return -1;
	}

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(obj, seg->nsects); nsects--;) {

		/* No more space to store sections.. */
		if (ctx->nsects == COUNT_OF(ctx->sections)) {
			errno = EBADMACHO;
			return -1;
		}

		/* Peek the section structure */
		const struct section *const sect = obj_peek(obj, off, sizeof *sect);

		if (sect == NULL)
			return -1;

		/* Update section table */
		if (ft_strcmp("__text", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 't';
		else if (ft_strcmp("__data", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 'd';
		else if (ft_strcmp("__bss", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 'b';
		else
			ctx->sections[ctx->nsects++] = 's';

		off += sizeof *sect;
	}

	return 0;
}

static int segment_64_collect(obj_t const obj, size_t off, void *const user)
{
	struct nm_context *const ctx = user;

	/* Peek the segment structure */
	const struct segment_command_64 *const seg = obj_peek(obj, off, sizeof *seg);

	if (seg == NULL)
		return -1;

	/* Check for architecture miss-match */
	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(obj)) {
		errno = EBADMACHO;
		return -1;
	}

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(obj, seg->nsects); nsects--;) {

		/* No more space to store sections.. */
		if (ctx->nsects == COUNT_OF(ctx->sections))
			return -1;

		/* Peek the section structure */
		const struct section_64 *const sect = obj_peek(obj, off, sizeof *sect);

		if (sect == NULL)
			return -1;

		/* Update section table */
		if (ft_strcmp("__text", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 't';
		else if (ft_strcmp("__data", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 'd';
		else if (ft_strcmp("__bss", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 'b';
		else
			ctx->sections[ctx->nsects++] = 's';

		off += sizeof *sect;
	}

	return 0;
}

/* nm collectors */
static const struct obj_collector nm_collector = {
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SYMTAB]     = symtab_collect,
		[LC_SEGMENT]    = segment_collect,
		[LC_SEGMENT_64] = segment_64_collect,
	}
};

int main(int ac, char *av[])
{
	const char *const exe = *av;
	int ret = EXIT_SUCCESS, flags;
	struct nm_context ctx = { };
	t_opt const opts[] = {
		{ FT_OPT_BOOLEAN, 'a', "debug-syms", &flags,
		  "Display debugger-only symbols", NM_OPT_a },
		{ FT_OPT_BOOLEAN, 'g', "extern-only", &flags,
		  "Display only external symbols", NM_OPT_g },
		{ FT_OPT_BOOLEAN, 'n', "numeric-sort", &flags,
		  "Sort symbols numerically by address", NM_OPT_n },
		{ FT_OPT_BOOLEAN, 'p', "no-sort", &flags,
		  "Do not sort the symbols", NM_OPT_p },
		{ FT_OPT_BOOLEAN, 'r', "reverse-sort", &flags,
		  "Reverse the sense of the sort", NM_OPT_r },
		{ FT_OPT_BOOLEAN, 'u', "undefined-only", &flags,
		  "Display only undefined symbols", NM_OPT_u },
		{ FT_OPT_BOOLEAN, 'U', "ext-undefined-only", &flags,
		  "Display only external undefined symbols", NM_OPT_U },
		{ FT_OPT_END, 0, 0, 0, 0, 0 }
	};

	int i = 1;
	bool printed = false;

	/* Parse options */
	if (ft_optparse(opts, &i, ac, av)) {

		/* Got an error while parsing options..
		 * show usage */
		ft_optusage(opts, av[0],
					"[file(s)]",
					"List symbols in [file(s)] (a.out by default).");

		return EXIT_FAILURE;
	}

	/* Default file if none */
	if (i == ac) av[ac++] = "a.out";

	/* Loop though argument and dump each one */
	for (; i < ac; ++i) {

		/* Add some indication btw two output */
		if (printed)               ft_fprintf(g_stdout, "\n");
		if (printed || i + 1 < ac) ft_fprintf(g_stdout, "%s:\n", av[i]);

		/* Retrieve flags from parsed options */
		ctx.flags = flags;

		/* Collect Mach-o object using nm collectors */
		if (obj_collect(av[i], &nm_collector, &ctx)) {

			/* Dump error, then continue.. */
			ft_fprintf(g_stderr, "%s: %s: %s\n",
					   exe, av[i], ft_strerror(errno));
			ret = EXIT_FAILURE;
		}

		/* Reset nm context memory for future use */
		ft_memset(&ctx, 0, sizeof ctx);
		printed = true;
	}

	return ret;
}
