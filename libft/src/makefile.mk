LIBFT_OBJ := $(LIBFT_ROOT_DIR)/src/ctype/ft_isalnum.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_isalpha.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_isascii.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_iscntrl.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_isdigit.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_islower.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_isprint.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_isspace.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_isupper.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_tolower.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/ctype/ft_toupper.o

LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_abs.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_atoi.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_atol.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_atoll.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_itoa.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_qsort.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_strerror.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/stdlib/ft_wctomb.o

LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memccpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memchr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memcmp.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memcpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memdup.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memmove.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memrcpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_memset.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_stpcpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strbstr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strcat.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strchr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strcmp.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strcpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strcspn.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strdup.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strestr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strlcat.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strlcpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strlen.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strmchr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strncat.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strnchr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strncmp.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strncpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strndup.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strnlen.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strnstr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strrchr.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strscpy.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strspn.o
LIBFT_OBJ += $(LIBFT_ROOT_DIR)/src/string/ft_strstr.o

LIBFT_GLOB_OBJ := $(LIBFT_ROOT_DIR)/src/glob/ft_glob.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/brace.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/climb_tree.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/match.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/dir.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/finder.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/is_magic.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/list.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/path.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/sanitize.o
LIBFT_GLOB_OBJ += $(LIBFT_ROOT_DIR)/src/glob/internal/show.o

ifeq (1,$(FT_GLOB_ENABLE))
  LIBFT_OBJ += $(LIBFT_GLOB_OBJ)
endif

LIBFT_MALLOC_OBJ := $(LIBFT_ROOT_DIR)/src/malloc/ft_mpool.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/ft_palloc.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/ft_pfree.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/internal/heap.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/internal/heap_bin.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/internal/heap_lrg.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/internal/stack.o
LIBFT_MALLOC_OBJ += $(LIBFT_ROOT_DIR)/src/malloc/internal/stack_chunk.o

ifeq (1,$(FT_MALLOC_ENABLE))
  LIBFT_OBJ += $(LIBFT_MALLOC_OBJ)

  $(call set_config,$(LIBFT_MALLOC_OBJ),FT_MALLOC_MAX_MPOOL)
endif

LIBFT_STDIO_OBJ := $(LIBFT_ROOT_DIR)/src/stdio/ft_asprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_dprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_fflush.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_fprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_fwrite.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_printf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_snprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_sprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_vasprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_vdprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_vfprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_vprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_vsnprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/ft_vsprintf.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/eval.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/fmtd.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/fmtm.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/fmtpct.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/fmts.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/fmtxp.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/parse.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/std.o
LIBFT_STDIO_OBJ += $(LIBFT_ROOT_DIR)/src/stdio/internal/type.o

ifeq (1,$(FT_STDIO_ENABLE))
  LIBFT_OBJ += $(LIBFT_STDIO_OBJ)
endif

LIBFT_OPTS_OBJ += $(LIBFT_ROOT_DIR)/src/opts/ft_optparse.o
LIBFT_OPTS_OBJ += $(LIBFT_ROOT_DIR)/src/opts/ft_optusage.o

ifeq (1,$(FT_OPTS_ENABLE))
  LIBFT_OBJ += $(LIBFT_OPTS_OBJ)
endif
