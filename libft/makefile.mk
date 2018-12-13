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

ifeq (,$(LIBFT_ROOT_DIR))
  $(error Must precise LIBFT_ROOT_DIR)
endif

ifeq (,$(OUTLIB_DIR))
  $(error Must precise OUTLIB_DIR)
endif

ifeq (,$(LIBFT_CONF_FILE))
  LIBFT_CONF_FILE := $(LIBFT_ROOT_DIR)/conf.mk
endif

LIBFT_CFLAGS += -I$(LIBFT_ROOT_DIR)/include
LIBFT_LDFLAGS += -L$(OUTLIB_DIR) -lft

include $(LIBFT_CONF_FILE)
include $(LIBFT_ROOT_DIR)/src/makefile.mk

$(call target_lib,libft,LIBFT_OBJ,LIBFT_LIB)
$(LIBFT_LIB): CFLAGS  += $(LIBFT_CFLAGS)
$(LIBFT_LIB): LDFLAGS += $(LIBFT_LDFLAGS)
