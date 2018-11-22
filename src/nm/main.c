#include <libft.h>

#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include <mach-o/reloc.h>
#include <mach-o/swap.h>
#include <sys/param.h>
#include <stdbool.h>

#include <sys/stat.h>
#include <sys/mman.h>

#define COUNT_OF(x) (sizeof(x) / sizeof(*(x)))

static const char *errno_str[] = {
	[EPERM] = "Operation not permitted",
	[ENOENT] = "No such file or directory",
	[ESRCH] = "No such process",
	[EINTR] = "Interrupted system call",
	[EIO] = "Input/output error",
	[ENXIO] = "Device not configured",
	[E2BIG] = "Argument list too long",
	[ENOEXEC] = "Exec format error",
	[EBADF] = "Bad file descriptor",
	[ECHILD] = "No child processes",
	[EDEADLK] = "Resource deadlock avoided",
	[ENOMEM] = "Cannot allocate memory",
	[EACCES] = "Permission denied",
	[EFAULT] = "Bad address",
	[ENOTBLK] = "Block device required",
	[EBUSY] = "Device / Resource busy",
	[EEXIST] = "File exists",
	[EXDEV] = "Cross-device link",
	[ENODEV] = "Operation not supported by device",
	[ENOTDIR] = "Not a directory",
	[EISDIR] = "Is a directory",
	[EINVAL] = "Invalid argument",
	[ENFILE] = "Too many open files in system",
	[EMFILE] = "Too many open files",
	[ENOTTY] = "Inappropriate ioctl for device",
	[ETXTBSY] = "Text file busy",
	[EFBIG] = "File too large",
	[ENOSPC] = "No space left on device",
	[ESPIPE] = "Illegal seek",
	[EROFS] = "Read-only file system",
	[EMLINK] = "Too many links",
	[EPIPE] = "Broken pipe",
	[EDOM] = "Numerical argument out of domain",
	[ERANGE] = "Result too large",
	[EAGAIN] = "Resource temporarily unavailable",
	[EINPROGRESS] = "Operation now in progress",
	[EALREADY] = "Operation already in progress",
	[ENOTSOCK] = "Socket operation on non-socket",
	[EDESTADDRREQ] = "Destination address required",
	[EMSGSIZE] = "Message too long",
	[EPROTOTYPE] = "Protocol wrong type for socket",
	[ENOPROTOOPT] = "Protocol not available",
	[EPROTONOSUPPORT] = "Protocol not supported",
	[ESOCKTNOSUPPORT] = "Socket type not supported",
	[ENOTSUP] = "Operation not supported",
	[EPFNOSUPPORT] = "Protocol family not supported",
	[EAFNOSUPPORT] = "Address family not supported by protocol family",
	[EADDRINUSE] = "Address already in use",
	[EADDRNOTAVAIL] = "Can't assign requested address",
	[ENETDOWN] = "Network is down",
	[ENETUNREACH] = "Network is unreachable",
	[ENETRESET] = "Network dropped connection on reset",
	[ECONNABORTED] = "Software caused connection abort",
	[ECONNRESET] = "Connection reset by peer",
	[ENOBUFS] = "No buffer space available",
	[EISCONN] = "Socket is already connected",
	[ENOTCONN] = "Socket is not connected",
	[ESHUTDOWN] = "Can't send after socket shutdown",
	[ETOOMANYREFS] = "Too many references: can't splice",
	[ETIMEDOUT] = "Operation timed out",
	[ECONNREFUSED] = "Connection refused",
	[ELOOP] = "Too many levels of symbolic links",
	[ENAMETOOLONG] = "File name too long",
	[EHOSTDOWN] = "Host is down",
	[EHOSTUNREACH] = "No route to host",
	[ENOTEMPTY] = "Directory not empty",
	[EPROCLIM] = "Too many processes",
	[EUSERS] = "Too many users",
	[EDQUOT] = "Disc quota exceeded",
	[ESTALE] = "Stale NFS file handle",
	[EREMOTE] = "Too many levels of remote in path",
	[EBADRPC] = "RPC struct is bad",
	[ERPCMISMATCH] = "RPC version wrong",
	[EPROGUNAVAIL] = "RPC prog. not avail",
	[EPROGMISMATCH] = "Program version wrong",
	[EPROCUNAVAIL] = "Bad procedure for program",
	[ENOLCK] = "No locks available",
	[ENOSYS] = "Function not implemented",
	[EFTYPE] = "Inappropriate file type or format",
	[EAUTH] = "Authentication error",
	[ENEEDAUTH] = "Need authenticator",
	[EPWROFF] = "Device power is off",
	[EDEVERR] = "Device error, e.g. paper out",
	[EOVERFLOW] = "Value too large to be stored in data type",
	[EBADEXEC] = "Bad executable",
	[EBADARCH] = "Bad CPU type in executable",
	[ESHLIBVERS] = "Shared library version mismatch",
	[EBADMACHO] = "Malformed Macho file",
	[ECANCELED] = "Operation canceled",
	[EIDRM] = "Identifier removed",
	[ENOMSG] = "No message of desired type",
	[EILSEQ] = "Illegal byte sequence",
	[ENOATTR] = "Attribute not found",
	[EBADMSG] = "Bad message",
	[EMULTIHOP] = "Reserved",
	[ENODATA] = "No message available on STREAM",
	[ENOLINK] = "Reserved",
	[ENOSR] = "No STREAM resources",
	[ENOSTR] = "Not a STREAM",
	[EPROTO] = "Protocol error",
	[ETIME] = "STREAM ioctl timeout",
	[EOPNOTSUPP] = "Operation not supported on socket",
	[ENOPOLICY] = "No such policy registered",
	[ENOTRECOVERABLE] = "State not recoverable",
	[EOWNERDEAD] = "Previous owner died",
	[EQFULL] = "Interface output queue is full",
};

static const char *ft_strerror(int eno)
{
	return eno < 0 || eno > (int)COUNT_OF(errno_str)
	       ? "Unknown error" : errno_str[eno];
}

struct reader {
	size_t off, len;
	uint8_t *buf;
	int err;
};

static int reader_rd(struct reader *rd, uint8_t *buf, unsigned sz)
{
	size_t left;
	unsigned start = sz;

	if ((left = rd->len - rd->off) > 0) {
		if (left > sz) left = sz;
		if (buf) ft_memcpy(buf, rd->buf + rd->off, left);
		sz -= left;
		rd->off += left;
	}

	if (sz) rd->err = -1;
	return start - sz;
}

//static int reader_seek(struct reader *rd, size_t off)
//{
//	(void)reader_seek;
//	if (off >= rd->len) return -1;
//	rd->off = off;
//	return 0;
//}

static uint8_t reader_rdbyte(struct reader *rd)
{
	uint8_t byte;
	
	return (uint8_t)(reader_rd(rd, &byte, 1) == 1 ? byte : 0);
}

static uint64_t reader_rdle(struct reader *rd, int z)
{
	uint64_t v = 0;
	
	if (z >= 1) v  = (uint64_t)reader_rdbyte(rd);
	if (z >= 2) v |= (uint64_t)reader_rdbyte(rd) <<  8;
	if (z >= 3) v |= (uint64_t)reader_rdbyte(rd) << 16;
	if (z >= 4) v |= (uint64_t)reader_rdbyte(rd) << 24;
	if (z >= 5) v |= (uint64_t)reader_rdbyte(rd) << 32;
	if (z >= 6) v |= (uint64_t)reader_rdbyte(rd) << 40;
	if (z >= 7) v |= (uint64_t)reader_rdbyte(rd) << 48;
	if (z >= 8) v |= (uint64_t)reader_rdbyte(rd) << 56;
	return v;
}


static uint64_t reader_rdbe(struct reader *rd, int z)
{
	uint64_t v = 0;
	
	if (z >= 1) v = (v     ) | reader_rdbyte(rd);
	if (z >= 2) v = (v << 8) | reader_rdbyte(rd);
	if (z >= 3) v = (v << 8) | reader_rdbyte(rd);
	if (z >= 4) v = (v << 8) | reader_rdbyte(rd);
	if (z >= 5) v = (v << 8) | reader_rdbyte(rd);
	if (z >= 6) v = (v << 8) | reader_rdbyte(rd);
	if (z >= 7) v = (v << 8) | reader_rdbyte(rd);
	if (z >= 8) v = (v << 8) | reader_rdbyte(rd);
	return v;
}

static uint64_t (*reader_rdint)(struct reader *rd, int z) = reader_rdbe;

static void reader_rdstr(struct reader *rd, char *dst, unsigned ns, unsigned nd)
{
	unsigned ncpy = MIN(ns, nd - 1);

	reader_rd(rd, (uint8_t *)dst, ncpy);
	reader_rd(rd, NULL, ns - ncpy);
	dst[nd - 1] = '\0';
}

static void reader_vrdfmt(struct reader *rd, const char *fmt, va_list ap)
{
	uint64_t (*rdxe)(struct reader *, int) = reader_rdint;
	char c;

	while ((c = *(fmt++))) {
		if (c == ' ') continue;
		if (c == '<') { rdxe = reader_rdle; continue; }
		if (c == '>') { rdxe = reader_rdbe; continue; }

		void *p = va_arg(ap, void *);
		int n = *fmt != '+' ? 1 : (fmt++, va_arg(ap, int));

		if (c >= '0' && c <= '8') {
			uint64_t *p_v = (uint64_t *)p;

			for (int i = 0; i < n; i++) {
				uint64_t v = rdxe(rd, c - '0');
				if (p) *(p_v++) = v;
			}
		} else if (c == '[' || c == '(') {
			char c_close = (char)(c == '[' ? ']' : ')');
			unsigned ns, nd = 0; char dig;

			if (*fmt == c_close) ns = (fmt++, va_arg(ap, unsigned));
			else for (ns = 0; (dig = *(fmt++)) != c_close;)
				ns = 10 * ns + (dig - '0');

			if (c == '(') nd = (fmt++, va_arg(ap, unsigned));

			if (!p) reader_rd(rd, NULL, ns);
			else if (c == '[') reader_rd(rd, p, ns);
			else reader_rdstr(rd, p, ns, nd);
		}
	}
}

static void reader_rdfmt(struct reader *rd, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	reader_vrdfmt(rd, fmt, ap);
	va_end(ap);
}

static int mapf(char *filename, uint8_t **pdata, size_t *plen)
{
	int fd;
	struct stat st;

	*pdata = NULL;
	*plen = 0;
	fd = open(filename, O_RDONLY, 0);
	if (fd < 0)
		return -1;
	if (fstat(fd, &st))
		return -1;
	*pdata = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (*pdata == MAP_FAILED)
		return close(fd), -1;
	*plen = (size_t)st.st_size;
	close(fd);
	return 0;

}

static int reader_rdhdr(struct reader *rd, struct mach_header *hdr)
{
	hdr->magic = (uint32_t)reader_rdbe(rd, 4);

	reader_rdint = hdr->magic == MH_CIGAM || hdr->magic == MH_CIGAM_64 ||
	               hdr->magic == FAT_CIGAM ? reader_rdle : reader_rdbe;

	reader_rdfmt(rd, "444444",
	             &hdr->cputype, &hdr->cpusubtype, &hdr->filetype,
	             &hdr->ncmds, &hdr->sizeofcmds, &hdr->flags);

	if (hdr->magic == MH_MAGIC_64 || hdr->magic == MH_CIGAM_64)
		reader_rd(rd, NULL, 4);

	return rd->err;
}

int main(int ac, char *av[])
{
	(void)ac;

	for (const char *av0 = *av++; *av; ++av) {
		struct reader rd;
		struct mach_header hdr;

		rd.err = 0;
		rd.off = 0;

		if (mapf(*av, &rd.buf, &rd.len)) {
			ft_printf("%s: %s\n", av0, ft_strerror(errno));
			continue;
		}

		if (reader_rdhdr(&rd, &hdr)) goto done;

		for (uint32_t i = 0; rd.err == 0 && i < hdr.ncmds; ++i) {
			struct load_command lc = { 0, 0 };
			struct segment_command sc;
			struct segment_command_64 sc_64;

			reader_rdfmt(&rd, "44", &lc.cmd, &lc.cmdsize);

			if (lc.cmd == LC_SEGMENT) {
				reader_rdfmt(&rd, "[16]44444444",
				             sc.segname, &sc.vmaddr, &sc.vmsize,
				             &sc.fileoff, &sc.filesize, &sc.maxprot,
				             &sc.initprot, &sc.nsects, &sc.flags);

				reader_rd(&rd, NULL,
				          lc.cmdsize - sizeof(struct segment_command));

				if (!rd.err) ft_printf("%s\n", sc.segname);

				for (uint32_t j = 0; rd.err == 0 && j < sc_64.nsects; ++j) {
					struct section s;

					reader_rdfmt(&rd, "[16][16]444444444",
					             s.sectname, s.segname, &s.addr,
					             &s.addr, &s.size, &s.offset,
					             &s.align,  &s.reloff, &s.nreloc,
					             &s.flags, NULL, NULL);

					if (!rd.err) ft_printf("  %s\n", s.sectname);
				}
			} else if (lc.cmd == LC_SEGMENT_64) {
				reader_rdfmt(&rd, "[16]88884444",
				             sc_64.segname, &sc_64.vmaddr, &sc_64.vmsize,
				             &sc_64.fileoff, &sc_64.filesize, &sc_64.maxprot,
				             &sc_64.initprot, &sc_64.nsects, &sc_64.flags);

				if (!rd.err) ft_printf("%s\n", sc_64.segname);

				for (uint32_t j = 0; rd.err == 0 && j < sc_64.nsects; ++j) {
					struct section_64 s_64;

					reader_rdfmt(&rd, "[16][16]8844444444",
					             s_64.sectname, s_64.segname, &s_64.addr,
					             &s_64.addr, &s_64.size, &s_64.offset,
					             &s_64.align,  &s_64.reloff, &s_64.nreloc,
					             &s_64.flags, NULL, NULL, NULL);

					if (!rd.err) ft_printf("  %s\n", s_64.sectname);
				}
			}
		}

done:   if (rd.err) ft_printf("%s: %s\n", av0, ft_strerror(errno));
		munmap(rd.buf, rd.len);
	}

	return 0;
}
