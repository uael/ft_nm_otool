#include <ft/stdio.h>
#include <ft/stdlib.h>

#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <mach-o/swap.h>

struct obj
{
	const char	*filename;
	size_t		off;
	size_t		len;
	uint8_t		*buf;
};

static bool g_is_le;

typedef int t_loader(struct obj *o, bool m,
					   int (*s)(void *, struct obj *, bool), void *);

struct obj_loader
{
	const uint32_t	magic;
	t_loader *const	load;
	const bool		m32;
	const bool		le;
};

void *obj_peek(struct obj *o, size_t off, size_t len)
{
	return (o->off + off + len >= o->len ? NULL : o->buf + o->off + off);
}

void obj_seek(struct obj *o, size_t off)
{
	o->off = off;
}

int fat_load(struct obj *o, bool m32,
	int (*load)(void *, struct obj *, bool), void *user)
{
	return load(user, o, m32);
}

int direct_load(struct obj *o, bool m32,
	int (*load)(void *, struct obj *, bool), void *user)
{
	return load(user, o, m32);
}

int	error_load(struct obj *o, bool m32,
	int (*load)(void *, struct obj *, bool), void *user)
{
	(void)o;
	(void)m32;
	(void)load;
	(void)user;
	return (-(errno = EBADMACHO));
}

static const struct obj_loader loaders[] = {
	{ MH_MAGIC,     direct_load, true,  false },
	{ MH_CIGAM,     direct_load, true,  true },
	{ MH_MAGIC_64,  direct_load, false, false },
	{ MH_CIGAM_64,  direct_load, false, true },
	{ FAT_MAGIC,    fat_load,    true,  false },
	{ FAT_CIGAM,    fat_load,    true,  true },
	{ FAT_MAGIC_64, fat_load,    false, false },
	{ FAT_CIGAM_64, fat_load,    false, true },
	{ 0,            error_load,  false, false },
};

int		obj_load(const char *obj_filename,
		int (*load)(void *, struct obj *, bool), void *user)
{
	int						i;
	struct stat				st;
	struct obj				o;
	const uint32_t			*magic;
	const struct obj_loader	*loader;

	if ((i = open(obj_filename, O_RDONLY)) < 0 || fstat(i, &st) < 0)
		return (-(errno));
	if ((st.st_mode & S_IFDIR))
		return (-(errno = EISDIR));
	o.len = (size_t)st.st_size;
	if ((o.buf = mmap(NULL, o.len, PROT_READ, MAP_PRIVATE, i, 0)) == MAP_FAILED)
		return (-(errno));
	if (close(i))
		return (-(errno));
	o.filename = obj_filename;
	if (!(magic = obj_peek(&o, 0, sizeof(uint32_t))))
		return (-(errno = EBADMACHO));
	loader = loaders;
	while (loader->magic && loader->magic != *magic)
		++loader;
	g_is_le = loader->le;
	i = loader->load(&o, loader->m32, load, user);
	munmap(o.buf, o.len);
	return (i);
}

int nm_load(void *user, struct obj *o, bool m32)
{
	(void)user;
	(void)o;
	(void)m32;
	return (0);
}

int main(int ac, char *av[])
{
	int err;

	err = ac > 1 ? obj_load(av[1], nm_load, NULL) : 0;
	if (err)
		ft_fprintf(g_stderr, "%s: %s\n", av[0], ft_strerror(errno));
	return (err);
}
