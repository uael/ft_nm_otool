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

#include "ofile.h"

#include <ft/ctype.h>
#include <ft/opts.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>
#include <stdlib.h>
#include <mach-o/nlist.h>


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

enum
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

static inline char const *nm_etoa(int const err)
{
	switch (err) {
		case NM_E_INVAL_SYMTAB:
			return "Invalid symtab size";
		case NM_E_INVAL_NLIST:
			return "Invalid nlist table size";
		case NM_E_INVAL_SYMTAB_ORDER:
			return "Invalid symtab order";
		case NM_E_INVAL_SEGMENT:
			return "Invalid segment size";
		case NM_E_INVAL_SECTION:
			return "Invalid section size";
		case NM_E_INVAL_SECTION_COUNT:
			return "Invalid section count size";
		case NM_E_INVAL_ARCH:
			return "Architecture miss-match";

		default: return ofile_etoa(err);
	}
}

struct nm_context
{
	char const *bin;
	int flags;
	char sects[UINT8_MAX];
	uint8_t nsects;
	int nfiles;
};

struct sym
{
	char const *string;
	uint64_t off;
	uint32_t str_max_size;
	char type;
};

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
	else if (type_field == N_SECT && ctx->sects[n_sect - 1])
		type = ctx->sects[n_sect - 1];
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

static inline bool syms_insert(struct nm_context *const ctx,
							   struct sym *sym)
{
	/* Skip debug symbol if option `a` isn't active */
	if (!(ctx->flags & NM_OPT_a) && sym->type == '-')
		return false;

	/* Skip non-external symbol if option `g` is active */
	if ((ctx->flags & NM_OPT_g) && !ft_isupper(sym->type))
		return false;

	/* Skip non-undefined symbol if option `u` is active */
	if ((ctx->flags & NM_OPT_u) && ft_tolower(sym->type) != 'u')
		return false;

	/* Skip non-external-undefined symbol if option `u` is active */
	if ((ctx->flags & NM_OPT_U) && sym->type != 'U')
		return false;

	if (!sym->string)
		sym->string = "bad string index";

	return true;
}

static int syms_n_cmp(const void *a, const void *b, size_t n)
{
	(void)n;
	struct sym const *const sym_a = a;
	struct sym const *const sym_b = b;
	int const cmp = (sym_a->off > sym_b->off) - (sym_a->off < sym_b->off);

	return cmp == 0 ? ft_strcmp(sym_a->string, sym_b->string) : cmp;
}

static int syms_s_cmp(const void *a, const void *b, size_t n)
{
	(void)n;
	struct sym const *const sym_a = a;
	struct sym const *const sym_b = b;

	int const cmp = ft_strcmp(sym_a->string, sym_b->string);

	return cmp == 0
	       ? (sym_a->off > sym_b->off) - (sym_a->off < sym_b->off) : cmp;
}

static int symtab_collect(obj_t const o, size_t const off, void *const user)
{
	struct nm_context *const ctx = user;

	/* Symtab before text section ? */
	if (ctx->nsects == 0) return (errno = EBADMACHO), NM_E_INVAL_SYMTAB_ORDER;

	/* Peek the symtab structure */
	struct symtab_command const *const symt = obj_peek(o, off, sizeof *symt);
	if (symt == NULL) return NM_E_INVAL_SYMTAB;

	uint32_t const stroff  = obj_swap32(o, symt->stroff);
	uint32_t const strsize = obj_swap32(o, symt->strsize);

	/* Check for string table validity */
	if (obj_peek(o, stroff, strsize) == NULL) return NM_E_INVAL_STRTAB;

	uint32_t const soff  = obj_swap32(o, symt->symoff);
	uint32_t const nsyms = obj_swap32(o, symt->nsyms);

	static size_t const nl_sizes[] = {
		[false] = sizeof(struct nlist),
		[true]  = sizeof(struct nlist_64)
	};

	/* Peek the nlist structure array */
	void const *const nls = obj_peek(o, soff, nl_sizes[obj_ism64(o)] * nsyms);
	if (nls == NULL) return NM_E_INVAL_NLIST;

	struct sym syms[nsyms];

	/* For each nlist insert the symbol into `head` singly linked list
	 * Options will affect insertion */
	uint32_t real_nsyms = 0;

	for (uint32_t i = 0; i < nsyms; ++i) {
		uint64_t value;
		uint32_t strx;
		uint16_t desc;
		uint8_t type, sect;

		if (obj_ism64(o)) {
			struct nlist_64 const *const nl = (struct nlist_64 *)nls + i;

			value = obj_swap64(o, nl->n_value);
			strx  = obj_swap32(o, nl->n_un.n_strx);
			desc  = obj_swap16(o, nl->n_desc);
			type  = nl->n_type;
			sect  = nl->n_sect;
		}
		else {
			struct nlist const *const nl = (struct nlist *)nls + i;

			value = obj_swap32(o, nl->n_value);
			strx  = obj_swap32(o, nl->n_un.n_strx);
			desc  = obj_swap16(o, (uint16_t)nl->n_desc);
			type  = nl->n_type;
			sect  = nl->n_sect;
		}

		uint32_t const offset = stroff + strx;

		syms[real_nsyms] = (struct sym){
			.str_max_size = stroff + strsize - offset,
			.string = obj_peek(o, offset, stroff + strsize - offset),
			.type = sym_type(value, type, sect, obj_swap16(o, desc), ctx),
			.off = value
		};

		/* Got a valid symbol, insert it */
		if (syms_insert(ctx, syms + real_nsyms))
			++real_nsyms;
	}

	if (!(ctx->flags & NM_OPT_p))
		ft_qsort(syms, real_nsyms, sizeof *syms,
		         (ctx->flags & NM_OPT_n) ? syms_n_cmp : syms_s_cmp);

	int const padding = obj_ism64(o) ? 16 : 8;

	/* Everything is done here (collect and sort), just dump.. */
	for (uint32_t i = 0; i < real_nsyms; ++i) {

		struct sym const *const head =
			syms + ((ctx->flags & NM_OPT_r) ? real_nsyms - 1 - i : i);

		/* Special output for valued and undefined symbol */
		if (head->off || !(head->type == 'u' || head->type == 'U'))
			ft_printf("%0*llx %c %.*s\n",
			          padding, head->off, head->type, head->str_max_size,
			          head->string);

		/* Standard output */
		else ft_printf("  %*c %.*s\n",
			           padding, head->type, head->str_max_size, head->string);
	}

	return 0;
}

static int sects_update(struct nm_context *const ctx,
                        struct section const *const sect)
{
	if (sect == NULL) return NM_E_INVAL_SECTION;

	/* Update section table */
	if (ft_strcmp("__text", sect->sectname) == 0)
		ctx->sects[ctx->nsects++] = 't';
	else if (ft_strcmp("__data", sect->sectname) == 0)
		ctx->sects[ctx->nsects++] = 'd';
	else if (ft_strcmp("__bss", sect->sectname) == 0)
		ctx->sects[ctx->nsects++] = 'b';
	else
		ctx->sects[ctx->nsects++] = 's';

	return 0;
}

static int segment_collect(obj_t const o, size_t off, void *const user)
{
	struct nm_context *const ctx = user;

	/* Peek the segment structure */
	struct segment_command const *const seg = obj_peek(o, off, sizeof *seg);
	if (seg == NULL) return NM_E_INVAL_SEGMENT;

	/* Check for architecture miss-match */
	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(o))
		return (errno = EBADARCH), NM_E_INVAL_ARCH;

	off += sizeof *seg;
	uint32_t nsects = obj_swap32(o, seg->nsects);

	/* No more space to store sections.. */
	if (ctx->nsects + nsects >= COUNT_OF(ctx->sects))
		return (errno = EBADMACHO), NM_E_INVAL_SECTION_COUNT;

	int err = 0;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (; err == 0 && nsects--; off += sizeof(struct section))
		err = sects_update(ctx, obj_peek(o, off, sizeof(struct section)));

	return err;
}

static int segment_64_collect(obj_t const o, size_t off, void *const user)
{
	struct nm_context *const ctx = user;

	/* Peek the segment structure */
	struct segment_command_64 const *const seg = obj_peek(o, off, sizeof *seg);
	if (seg == NULL) return NM_E_INVAL_SEGMENT;

	/* Check for architecture miss-match */
	if ((seg->cmd == LC_SEGMENT_64) != obj_ism64(o))
		return (errno = EBADARCH), NM_E_INVAL_ARCH;

	off += sizeof *seg;
	uint32_t nsects = obj_swap32(o, seg->nsects);

	/* No more space to store sections.. */
	if (ctx->nsects + nsects >= COUNT_OF(ctx->sects))
		return (errno = EBADMACHO), NM_E_INVAL_SECTION_COUNT;

	int err = 0;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (; err == 0 && nsects--; off += sizeof(struct section_64))
		err = sects_update(ctx, obj_peek(o, off, sizeof(struct section_64)));

	return err;
}

static void on_load(obj_t const o, NXArchInfo const *const arch_info,
                    void *user)
{
	struct nm_context *const ctx = user;
	size_t name_len;

	/* In case of archive sub object, name is non-null */
	char const *const name = obj_name(o, &name_len);

	/* In case of FAT file and multiple arch, also print arch info name */
	bool const fat = obj_ofile(o) != OFILE_MH && obj_target(o) == OFILE_NX_ALL;

	if (name || fat) {
		ft_printf("\n%s", ctx->bin);
		if (name) ft_printf("(%.*s)", (unsigned) name_len, name);
		if (fat)  ft_printf(" (for architecture %s)",
			                arch_info ? arch_info->name : "none");
		ft_printf(":\n");
	}
	else if (ctx->nfiles > 1) ft_printf("\n%s:\n", ctx->bin);

	/* Reset section saves */
	ctx->nsects = 0;
	ft_memset(ctx->sects, 0, sizeof ctx->sects);
}

/* nm collectors */
static const struct ofile_collector nm_collector = {
	.load = on_load,
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SYMTAB]     = symtab_collect,
		[LC_SEGMENT]    = segment_collect,
		[LC_SEGMENT_64] = segment_64_collect,
	}
};

int main(int ac, char *av[])
{
	char const *const exe = *av;
	int ret = EXIT_SUCCESS, flags;
	struct nm_context ctx = { };
	char const *arch = NULL;
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
					"List symbols in [file(s)] (a.out by default).");

		return EXIT_FAILURE;
	}

	NXArchInfo const *arch_info = NULL;

	if (arch == NULL) arch_info = OFILE_NX_HOST;
	else if (ft_strcmp("all", arch) != 0 &&
	         (arch_info = NXGetArchInfoFromName(arch)) == NULL) {

		/* Dump error, then abort.. */
		ft_fprintf(g_stderr, "%s: Unknown architecture named '%s'\n",
		           exe, arch);

		return EXIT_FAILURE;
	}

	/* Default file if none */
	if (i == ac) av[ac++] = "a.out";

	int const nfiles = ac - i;

	/* Loop though argument and dump each one */
	for (; i < ac; ++i) {

		/* Retrieve flags from parsed options
		 * and bin from cmd line argument */
		ctx.flags  = flags;
		ctx.bin    = av[i];
		ctx.nfiles = nfiles;

		/* Collect Mach-o object using nm collectors */
		int const err = ofile_collect(ctx.bin, arch_info, &nm_collector, &ctx);
		if (err) {

			/* Dump error, then continue.. */
			ft_fprintf(g_stderr, "%s: %s: %s\n", exe, ctx.bin, nm_etoa(err));
			ret = EXIT_FAILURE;
		}

		/* Reset nm context memory for future use */
		ft_memset(&ctx, 0, sizeof ctx);
	}

	/* Even if it's not allocated, this proc is safe to call */
	if (arch_info && arch_info != OFILE_NX_HOST) NXFreeArchInfo(arch_info);

	return ret;
}
