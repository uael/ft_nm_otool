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
AS     := gcc
LD     := gcc
AR     := ar
CFLAGS += -Wall -Wextra -Werror

ifeq ($(DEBUG),)
  CONFIG   = release
  CFLAGS  += -O2
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

BUILD_DIR  ?= build
BUILD_PATH ?= $(BUILD_DIR)/$(CONFIG)
OUTLIB_DIR ?= $(BUILD_DIR)/lib
OUTBIN_DIR ?= $(BUILD_DIR)/bin

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

NM_OTOOL_ROOT_DIR := $(shell pwd)
include makefile.mk

lib: $(TARGET_LIB)
bin: $(TARGET_BIN)

all: lib bin

MAKE_DEPS := $(MAKEFILE_LIST)

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------

V ?= @

-include $(addprefix $(BUILD_PATH)/,$(patsubst %.o,%.d,$(OBJ)))

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

$(TARGET_LIB): | $(DEPS) $(MAKE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  AR      $(notdir $@)"
	$(V)$(AR) rcs $@ $^
	$(V)ln -sf $@ ./$(notdir $@)

$(TARGET_BIN): | $(DEPS) $(MAKE_DEPS)
	@mkdir -p $(dir $@)
	@echo "  LD      $(notdir $@)"
	$(V)$(LD) $^ $(LDFLAGS) $(addprefix -L,$(LDDIRS)) \
	  $(addprefix -l,$(LDLIBS)) -o $@
	$(V)ln -sf $@ ./$(notdir $@)

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean
	@rm -rf $(TARGET_LIB) $(TARGET_BIN)

re: clean all
