#include "obj.h"

#include <ft/ctype.h>
#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <errno.h>

struct nm_context
{
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

static inline int syms_dump(t_stream *const s, struct sym *head)
{
	for (; head; head = head->next) {

		if (head->off || !(head->type == 'u' || head->type == 'U'))
			ft_fprintf(s, "%0*lx %c %.*s\n",
					   16, head->off, head->type, head->str_max_size,
					   head->string);

		else
			ft_fprintf(s, "  %*c %.*s\n",
					   16, head->type, head->str_max_size, head->string);
	}

	return 0;
}

static inline int symtab_32_collect(obj_t const obj, size_t const off,
									void *const user)
{
	(void)obj;
	(void)off;
	(void)user;
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

static inline int symtab_64_collect(obj_t const obj, size_t const off,
									void *const user)
{
	struct nm_context *const ctx = user;
	const struct symtab_command *const symtab =
		obj_peek(obj, off, sizeof *symtab);

	if (symtab == NULL)
		return -1;

	uint32_t const stroff = obj_swap32(obj, symtab->stroff);
	uint32_t const strsize = obj_swap32(obj, symtab->strsize);

	if (obj_peek(obj, stroff, strsize) == NULL)
		return -1;

	uint32_t const symoff = obj_swap32(obj, symtab->symoff);
	uint32_t const nsyms = obj_swap32(obj, symtab->nsyms);

	const struct nlist_64 *const nlist =
		obj_peek(obj, symoff, sizeof(*nlist) * nsyms);

	if (nlist == NULL)
		return -1;

	struct sym *head = NULL, **it, syms[nsyms];

	for (uint32_t i = 0; i < nsyms; ++i) {

		if (nlist[i].n_sect != NO_SECT &&
			nlist[i].n_sect > COUNT_OF(ctx->sections))
			return -1;

		const uint32_t offset = stroff + obj_swap32(obj, nlist[i].n_un.n_strx);

		syms[i] = (struct sym){
			.str_max_size = stroff + strsize - offset,
			.string = obj_peek(obj, offset, stroff + strsize - offset),
			.type = sym_type(obj_swap64(obj, nlist[i].n_value), nlist[i].n_type,
							 nlist[i].n_sect, obj_swap16(obj, nlist[i].n_desc),
							 ctx),
			.off = obj_swap64(obj, nlist[i].n_value)
		};

		/* Save some instructions by insert sorted
		 * into the symbol linked list */
		for (it = &head; *it; it = &(*it)->next)
			if (ft_strcmp(syms[i].string, (*it)->string) < 0) {
				syms[i].next = *it;
				break;
			}

		*it = syms + i;
	}

	return syms_dump(g_stdout, head);
}

static int symtab_collect(obj_t const obj, const size_t off, void *const user)
{
	static obj_collector_t *const collectors[] = {
		[false] = symtab_32_collect,
		[true]  = symtab_64_collect
	};

	return collectors[obj_ism64(obj)](obj, off, user);
}

static int segment_collect(obj_t const obj, size_t off, void *const user)
{
	struct nm_context *const ctx = user;
	const struct segment_command *const seg = obj_peek(obj, off, sizeof *seg);

	if (seg == NULL)
		return -1;

	off += sizeof *seg;

	/* Loop though section and collect each one
	 * section is next to it's header */
	for (uint32_t nsects = obj_swap32(obj, seg->nsects); nsects--;) {

		if (ctx->nsects == COUNT_OF(ctx->sections))
			return -1;

		/* Peek the section structure */
		const struct section *const sect = obj_peek(obj, off, sizeof *sect);

		if (sect == NULL)
			return -1;

		if (ft_strcmp("__text", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 't';
		else if (ft_strcmp("__data", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 'd';
		else if (ft_strcmp("__bss", sect->sectname) == 0)
			ctx->sections[ctx->nsects++] = 'b';
		else
			ctx->sections[ctx->nsects++] = 's';

		static const size_t section_size[] = {
			[false] = sizeof(struct section),
			[true]  = sizeof(struct section_64)
		};

		off += section_size[obj_ism64(obj)];
	}

	return 0;
}

static const struct obj_collector nm_collector = {
	.ncollector = LC_SEGMENT_64 + 1,
	.collectors = {
		[LC_SYMTAB]     = symtab_collect,
		[LC_SEGMENT]    = segment_collect,
		[LC_SEGMENT_64] = segment_collect,
	}
};

int main(int ac, char *av[])
{
	int err;
	struct nm_context ctx = { };

	err = ac > 1 ? obj_collect(av[1], &nm_collector, &ctx) : 0;
	if (err)
		ft_fprintf(g_stderr, "%s: %s\n", av[0], ft_strerror(errno));
	return (err);
}
