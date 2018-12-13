OTOOL_OBJ += src/otool/main.o

$(eval $(call target_bin,ft_otool,OTOOL_OBJ,OTOOL_BIN))
$(OTOOL_BIN): $(LIBFT_LIB)
$(OTOOL_BIN): CFLAGS  +=  $(LIBFT_CFLAGS)
