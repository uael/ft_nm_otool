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

ifeq (,$(NM_OTOOL_ROOT_DIR))
  $(error Must precise NM_OTOOL_ROOT_DIR)
endif

ifeq (,$(OUTLIB_DIR))
  $(error Must precise OUTLIB_DIR)
endif

LIBFT_ROOT_DIR := libft
include $(LIBFT_ROOT_DIR)/makefile.mk
include $(NM_OTOOL_ROOT_DIR)/src/makefile.mk
