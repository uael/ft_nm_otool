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

# ------------------------------------------------------------------------------
# Configuration
# ------------------------------------------------------------------------------

MAKE_DEPS = Makefile

CC=gcc
LD=gcc

CFLAGS += -fPIC

ifneq ($(DEBUG),)
  CONFIG = debug
  CFLAGS += -g3 -O0
  ifneq ($(ASAN),)
  	CFLAGS += -fsanitize=address
  	LDFLAGS += -fsanitize=address
  	TARGET_SUFFIX = .san
  else
    TARGET_SUFFIX = .dev
  endif
else
  CONFIG = release
  CFLAGS += -flto -O3 -Wall -Werror -Wextra
  LDFLAGS += -flto
endif

BUILD_DIR = build
BUILD_PATH := $(BUILD_DIR)/$(CONFIG)

# ------------------------------------------------------------------------------
# Sources & Target
# ------------------------------------------------------------------------------

TARGET = libft

INCLUDE = include

include src/makefile.mk
MAKE_DEPS += src/makefile.mk

TARGET_LIB := $(TARGET)$(TARGET_SUFFIX).a
all: $(TARGET_LIB)
.DEFAULT_GOAL := $(TARGET_LIB)

# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------

V ?= @

-include $(addprefix $(BUILD_PATH)/,$(SRC:.c=.d))

$(BUILD_PATH)/%.o: %.c $(MAKE_DEPS)
	@echo "  CC      $(notdir $<)"
	@mkdir -p $(dir $@)
	$(V)$(CC) $< -c $(CFLAGS) $(addprefix -I,$(INCLUDE)) \
		$(addprefix -D,$(DEFINE)) -MMD -MF $(@:.o=.d) -o $@

$(TARGET_LIB): $(addprefix $(BUILD_PATH)/,$(SRC:.c=.o))
	@echo "  LD      $(notdir $@)"
	$(V)ar -rc $(TARGET).a $^
	$(V)ranlib $(TARGET).a

clean:
	@$(RM) -rf $(BUILD_DIR)

fclean: clean
	@$(RM) -f $(TARGET_LIB)

re: fclean all

norm:
	@norminette $(shell echo "{include,src}/**/**.[ch]") \
	  $(shell echo "{include,src}/**.[ch]")
