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

.DEFAULT_GOAL := all

# ------------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------------

CC     := gcc
LD     := gcc
CFLAGS += -Wall -Wextra -Werror

ifeq ($(DEBUG),)
  CONFIG   = release
  CFLAGS  += -flto -O2
  LDFLAGS += -flto
else
  CONFIG         = debug
  TARGET_SUFFIX  = -debug
  CFLAGS        += -g3 -O0
  ifneq ($(SAN),)
    CONFIG         = san
    TARGET_SUFFIX  = -san
  	CFLAGS        += -fsanitize=address
  	LDFLAGS       += -fsanitize=address
  endif
endif

PREFIX     ?= .
BUILD_DIR  ?= build
BUILD_PATH ?= $(BUILD_DIR)/$(CONFIG)
OUTLIB_DIR ?= $(PREFIX)/lib
OUTBIN_DIR ?= $(PREFIX)/bin

# ------------------------------------------------------------------------------
# Sources & Target
# ------------------------------------------------------------------------------

all:

define set_define =
  $(addprefix $(BUILD_PATH)/,$(1)): DEFINE+=$(2)
endef

define set_config =
  $(addprefix $(BUILD_PATH)/,$(1)): DEFINE+=$(2)=$($(2))
endef

define target
  $(eval $(3) := $($(5))/$(1)$(TARGET_SUFFIX)$(6))
  $($(3)): $(addprefix $(BUILD_PATH)/,$($(2)))
  $(eval $(4) += $($(3)))
  $(eval OBJ += $($(2)))
  $(1): $($(3))
endef

define target_lib
  $(eval $(call target,$(1),$(2),$(3),TARGET_LIB,OUTLIB_DIR,.a))
endef

define target_bin
  $(eval $(call target,$(1),$(2),$(3),TARGET_BIN,OUTBIN_DIR,))
endef

LIBFT_ROOT_DIR=libft
include $(LIBFT_ROOT_DIR)/makefile.mk

CFLAGS  += $(LIBFT_CFLAGS)
LDFLAGS += $(LIBFT_LDFLAGS)

INCLUDE += include
LDDIRS  += $(OUTLIB_DIR)

include src/makefile.mk

$(TARGET_BIN): $(LIBFT_LIB)

lib: $(TARGET_LIB)
bin: $(TARGET_BIN)

all: bin

MAKE_DEPS := $(MAKEFILE_LIST) $(BUILD_PATH)

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------

V ?= @


-include $(addprefix $(BUILD_PATH)/,$(OBJ:.o=.d))

$(BUILD_PATH)/%.o: %.c $(MAKE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  CC      $(notdir $<)"
	$(V)$(CC) $< -c $(CFLAGS) $(addprefix -I,$(INCLUDE)) \
	  $(addprefix -D,$(DEFINE)) -MMD -MF $(@:.o=.d) -o $@

$(BUILD_PATH)/%.o: %.s $(MAKE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  AS      $(notdir $<)"
	$(V)$(AS) $< -c $(CFLAGS) $(addprefix -I,$(INCLUDE)) \
	  $(addprefix -D,$(DEFINE)) -MMD -MF $(@:.o=.d) -o $@

$(TARGET_LIB): | $(MAKE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  AR      $(notdir $@)"
	$(V)$(AR) rcs $@ $^

$(TARGET_BIN): | $(MAKE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  LD      $(notdir $@)"
	$(V)$(LD) $^ $(LDFLAGS) $(addprefix -L,$(LDDIRS)) \
	  $(addprefix -l,$(LDLIBS)) -o $@

$(BUILD_PATH):
	@mkdir -p $(BUILD_PATH)

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -rf $(TARGET_BIN)

re: clean all
