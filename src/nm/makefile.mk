NM_OBJ += src/nm/main.o

$(eval $(call target_bin,ft_nm,NM_OBJ,NM_BIN))
$(NM_BIN): $(LIBFT_LIB)
$(NM_BIN): CFLAGS  +=  $(LIBFT_CFLAGS)
