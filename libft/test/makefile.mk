# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    makefile.mk                                        :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#              #
#    Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TEST_MALLOC_OBJ = test/malloc.o

$(eval $(call target_bin,test-malloc,TEST_MALLOC_OBJ,TEST_MALLOC_BIN))
$(TEST_MALLOC_BIN): $(LIBFT_LIB)
$(TEST_MALLOC_BIN): CFLAGS += $(LIBFT_CFLAGS)
