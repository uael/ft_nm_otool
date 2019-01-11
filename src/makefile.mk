OFILE_OBJ = src/ofile.o \
            src/ofile_misc.o \
            src/ofile_mh.o \
            src/ofile_fat.o \
            src/ofile_ar.o

NM_OBJ += $(OFILE_OBJ) src/nm.o

$(eval $(call target_bin,ft_nm,NM_OBJ,NM_BIN))
$(NM_BIN): $(LIBFT_LIB)
$(NM_BIN): CFLAGS  +=  $(LIBFT_CFLAGS)
$(NM_BIN): INCLUDE +=  src include

OTOOL_OBJ += $(OFILE_OBJ) src/otool.o

$(eval $(call target_bin,ft_otool,OTOOL_OBJ,OTOOL_BIN))
$(OTOOL_BIN): $(LIBFT_LIB)
$(OTOOL_BIN): CFLAGS  +=  $(LIBFT_CFLAGS)
$(OTOOL_BIN): INCLUDE +=  src include
