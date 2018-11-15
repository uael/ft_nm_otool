# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#              #
#    Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq (,$(LIBFT_ROOT_DIR))
  $(error Must precise LIBFT_ROOT_DIR)
endif

LIBFT_CFLAGS += -I$(LIBFT_ROOT_DIR)/include
LIBFT_LDFLAGS += -lft

include $(LIBFT_ROOT_DIR)/src/makefile.mk

$(call target_lib,libft,LIBFT_OBJ,LIBFT_LIB)
