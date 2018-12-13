#include <ft/stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

struct obj
{
	const char *filename;
	size_t  off;
	size_t  len;
	uint8_t *buf;
};

typedef int t_obj_load(struct obj *o, bool m, int (*r)(struct obj *, bool));

struct obj_loader
{
	uint32_t magic;
	bool m32;
	t_obj_load *load;
};

#define NM_EISDIR  (200)

void *obj_peek(struct obj *o, size_t off, size_t len)
{
	return (o->off + off + len >= o->len ? NULL : o->buf + o->off + off);
}

void obj_seek(struct obj *o, size_t off)
{
	o->off = off;
}

int archive_load(struct obj *o, bool m32, int (*reader)(struct obj *, bool))
{
	return reader(o, m32);
}

int fat_load(struct obj *o, bool m32, int (*reader)(struct obj *, bool))
{
	return reader(o, m32);
}

int direct_load(struct obj *o, bool m32, int (*reader)(struct obj *, bool))
{
	return reader(o, m32);
}

int error_load(struct obj *o, bool m32, int (*reader)(struct obj *, bool))
{
	return 0;
}

static const struct obj_loader loaders[] = {
	{ ARCHIVE_MAGIC, archive_load, false },
	{ ARCHIVE_CIGAM, archive_load, false },
	{ MH_MAGIC,      direct_load,  false },
	{ MH_CIGAM,      direct_load,  false },
	{ MH_MAGIC_64,   direct_load,  true },
	{ MH_CIGAM_64,   direct_load,  true },
	{ FAT_MAGIC,     fat_load,     false },
	{ FAT_CIGAM,     fat_load,     false },
	{ FAT_MAGIC_64,  fat_load,     true },
	{ FAT_CIGAM_64,  fat_load,     true },
	{ 0,             error_load },
};

int obj_load(const char *obj_filename, int (*load)(struct obj *, bool))
{
	int			i;
	struct stat	st;
	struct obj  o;
	uint32_t    *magic;
	const struct obj_loader *loader;

	if ((i = open(obj_filename, O_RDONLY)) < 0 || fstat(i, &st) < 0)
		return (-(errno));
	if ((st.st_mode & S_IFDIR))
		return (-(errno = NM_EISDIR));
	o.len = (size_t)st.st_size;
	if ((o.buf = mmap(NULL, o.len, PROT_READ, MAP_PRIVATE, i, 0)) == MAP_FAILED)
		return (-(errno));
	if (close(i))
		return (-(errno));
	o.len = 0;
	o.filename = obj_filename;
	magic = obj_peek(&o, 0, sizeof(uint32_t));
	loader = loaders;
	while (loader->magic && loader->magic != *magic)
		++loader;
	i = loader->load(&o, true, load);
	munmap(o.buf, o.len);
	return (i);
}

int main(int ac, char *av[])
{
	(void)ac;
	(void)av;
	return 0;
}
