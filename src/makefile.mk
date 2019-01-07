NM_OBJ += src/ofile.o src/nm.o

EBADMACHO := 0x242
EBADARCH  := 0x243
$(call set_config,$(NM_OBJ),EBADMACHO)
$(call set_config,$(NM_OBJ),EBADARCH)

$(eval $(call target_bin,ft_nm,NM_OBJ,NM_BIN))
$(NM_BIN): $(LIBFT_LIB)
$(NM_BIN): CFLAGS  +=  $(LIBFT_CFLAGS)
$(NM_BIN): INCLUDE +=  src include

OTOOL_OBJ += src/ofile.o src/otool.o

EBADMACHO := 0x242
EBADARCH  := 0x243
$(call set_config,$(OTOOL_OBJ),EBADMACHO)
$(call set_config,$(OTOOL_OBJ),EBADARCH)

$(eval $(call target_bin,ft_otool,OTOOL_OBJ,OTOOL_BIN))
$(OTOOL_BIN): $(LIBFT_LIB)
$(OTOOL_BIN): CFLAGS  +=  $(LIBFT_CFLAGS)
$(OTOOL_BIN): INCLUDE +=  src include


