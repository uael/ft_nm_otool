# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    CMakeLists.txt                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: alucas- <alucas-@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 1970/01/01 00:00:42 by alucas-           #+#    #+#              #
#    Updated: 1970/01/01 00:00:42 by alucas-          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(libft VERSION 0.0.1 LANGUAGES C)

## ************************************************************************** ##
# Build types
##
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "San")

## ************************************************************************** ##
# Options
##


## ************************************************************************** ##
# Dependencies
##
set(__DEPS)

## ************************************************************************** ##
# Project directories definition
##
set(__INC_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(__SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

## ************************************************************************** ##
# Target
##
add_library(libft
	${__SRC_DIR}/cty/cty.c
	${__SRC_DIR}/cty/cty_2.c
	${__SRC_DIR}/cty/cty_3.c
	${__SRC_DIR}/ds/alloc.c
	${__SRC_DIR}/ds/apd.c
	${__SRC_DIR}/ds/at.c
	${__SRC_DIR}/ds/aver.c
	${__SRC_DIR}/ds/back.c
	${__SRC_DIR}/ds/begin.c
	${__SRC_DIR}/ds/clean.c
	${__SRC_DIR}/ds/clr.c
	${__SRC_DIR}/ds/cpush.c
	${__SRC_DIR}/ds/cput.c
	${__SRC_DIR}/ds/ctor.c
	${__SRC_DIR}/ds/cusht.c
	${__SRC_DIR}/ds/dtor.c
	${__SRC_DIR}/ds/emp.c
	${__SRC_DIR}/ds/end.c
	${__SRC_DIR}/ds/grow.c
	${__SRC_DIR}/ds/len.c
	${__SRC_DIR}/ds/lst.c
	${__SRC_DIR}/ds/map.c
	${__SRC_DIR}/ds/map_2.c
	${__SRC_DIR}/ds/mdtor.c
	${__SRC_DIR}/ds/mpush.c
	${__SRC_DIR}/ds/mput.c
	${__SRC_DIR}/ds/musht.c
	${__SRC_DIR}/ds/nodelink.c
	${__SRC_DIR}/ds/nodelput.c
	${__SRC_DIR}/ds/noderput.c
	${__SRC_DIR}/ds/nodeswap.c
	${__SRC_DIR}/ds/nodeulink.c
	${__SRC_DIR}/ds/npop.c
	${__SRC_DIR}/ds/npush.c
	${__SRC_DIR}/ds/nput.c
	${__SRC_DIR}/ds/nrem.c
	${__SRC_DIR}/ds/nsht.c
	${__SRC_DIR}/ds/nusht.c
	${__SRC_DIR}/ds/pop.c
	${__SRC_DIR}/ds/prd.c
	${__SRC_DIR}/ds/push.c
	${__SRC_DIR}/ds/put.c
	${__SRC_DIR}/ds/rem.c
	${__SRC_DIR}/ds/set.c
	${__SRC_DIR}/ds/set_2.c
	${__SRC_DIR}/ds/sht.c
	${__SRC_DIR}/ds/usht.c
	${__SRC_DIR}/ex/ex.c
	${__SRC_DIR}/ex/ex_2.c
	${__SRC_DIR}/fs/fs.c
	${__SRC_DIR}/fs/fs_2.c
	${__SRC_DIR}/fs/fs_3.c
	${__SRC_DIR}/hash/hash.c
	${__SRC_DIR}/hash/hash_2.c
	${__SRC_DIR}/int/len.c
	${__SRC_DIR}/int/str.c
	${__SRC_DIR}/io/fmt
	${__SRC_DIR}/io/fmt/eval.c
	${__SRC_DIR}/io/fmt/fmtd.c
	${__SRC_DIR}/io/fmt/fmtm.c
	${__SRC_DIR}/io/fmt/fmtpct.c
	${__SRC_DIR}/io/fmt/fmts.c
	${__SRC_DIR}/io/fmt/fmtxp.c
	${__SRC_DIR}/io/fmt/internal.h
	${__SRC_DIR}/io/fmt/parse.c
	${__SRC_DIR}/io/fmt/type.c
	${__SRC_DIR}/io/asprintf.c
	${__SRC_DIR}/io/dprintf.c
	${__SRC_DIR}/io/fcntl.c
	${__SRC_DIR}/io/fflush.c
	${__SRC_DIR}/io/fmt.h
	${__SRC_DIR}/io/fopen.c
	${__SRC_DIR}/io/fprintf.c
	${__SRC_DIR}/io/fputc.c
	${__SRC_DIR}/io/fputs.c
	${__SRC_DIR}/io/fwrite.c
	${__SRC_DIR}/io/ifs.c
	${__SRC_DIR}/io/ifs_2.c
	${__SRC_DIR}/io/internal.h
	${__SRC_DIR}/io/printf.c
	${__SRC_DIR}/io/snprintf.c
	${__SRC_DIR}/io/sprintf.c
	${__SRC_DIR}/io/stderr.c
	${__SRC_DIR}/io/stdin.c
	${__SRC_DIR}/io/stdio.c
	${__SRC_DIR}/io/stdout.c
	${__SRC_DIR}/io/vasprintf.c
	${__SRC_DIR}/io/vdprintf.c
	${__SRC_DIR}/io/vfprintf.c
	${__SRC_DIR}/io/vprintf.c
	${__SRC_DIR}/io/vsnprintf.c
	${__SRC_DIR}/io/vsnprintf.h
	${__SRC_DIR}/io/vsprintf.c
	${__SRC_DIR}/lib/atoi.c
	${__SRC_DIR}/lib/clean.c
	${__SRC_DIR}/lib/dtor.c
	${__SRC_DIR}/lib/error.c
	${__SRC_DIR}/lib/getenv.c
	${__SRC_DIR}/lib/itoa.c
	${__SRC_DIR}/lib/join.c
	${__SRC_DIR}/lib/wctomb.c
	${__SRC_DIR}/math/eq.c
	${__SRC_DIR}/math/eq_2.c
	${__SRC_DIR}/math/imax.c
	${__SRC_DIR}/math/imin.c
	${__SRC_DIR}/math/pow.c
	${__SRC_DIR}/math/pow2_next.c
	${__SRC_DIR}/math/umax.c
	${__SRC_DIR}/math/umin.c
	${__SRC_DIR}/mem/alloc.c
	${__SRC_DIR}/str/bzero.c
	${__SRC_DIR}/str/memccpy.c
	${__SRC_DIR}/str/memchr.c
	${__SRC_DIR}/str/memcmp.c
	${__SRC_DIR}/str/memcpy.c
	${__SRC_DIR}/str/memdup.c
	${__SRC_DIR}/str/memmove.c
	${__SRC_DIR}/str/mempcpy.c
	${__SRC_DIR}/str/memrcpy.c
	${__SRC_DIR}/str/memset.c
	${__SRC_DIR}/str/stpcpy.c
	${__SRC_DIR}/str/strbstr.c
	${__SRC_DIR}/str/strcat.c
	${__SRC_DIR}/str/strchr.c
	${__SRC_DIR}/str/strcmp.c
	${__SRC_DIR}/str/strcpy.c
	${__SRC_DIR}/str/strcspn.c
	${__SRC_DIR}/str/strdup.c
	${__SRC_DIR}/str/strestr.c
	${__SRC_DIR}/str/stris.c
	${__SRC_DIR}/str/strlcat.c
	${__SRC_DIR}/str/strlcpy.c
	${__SRC_DIR}/str/strlen.c
	${__SRC_DIR}/str/strmchr.c
	${__SRC_DIR}/str/strncat.c
	${__SRC_DIR}/str/strnchr.c
	${__SRC_DIR}/str/strncmp.c
	${__SRC_DIR}/str/strncpy.c
	${__SRC_DIR}/str/strndup.c
	${__SRC_DIR}/str/strnlen.c
	${__SRC_DIR}/str/strnstr.c
	${__SRC_DIR}/str/strrchr.c
	${__SRC_DIR}/str/strscpy.c
	${__SRC_DIR}/str/strspn.c
	${__SRC_DIR}/str/strstr.c
	${__SRC_DIR}/getopt.c
	${__SRC_DIR}/gnl.c)

set_target_properties(libft PROPERTIES
	C_STANDARD 90
	OUTPUT_NAME "ft"
	OUTPUT_NAME_DEBUG "ft.dev"
	OUTPUT_NAME_SAN "ft.san"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(libft PRIVATE ${__SRC_DIR} PUBLIC ${__INC_DIR})
if (MSVC)
	target_compile_definitions(libft PRIVATE _CRT_SECURE_NO_WARNINGS)
	target_compile_options(libft PRIVATE /Oy /W3)
else ()
	target_link_libraries(libft
		PUBLIC ${__DEPS}
		PRIVATE
			$<$<CONFIG:San>:-fsanitize=address>
			$<$<CONFIG:San>:-fno-omit-frame-pointer>)
	target_compile_options(libft
		PRIVATE
			-Wall -Werror -Wextra -fPIC
			-ansi -fomit-frame-pointer
			-Wno-missing-field-initializers
			$<$<CONFIG:Debug>:-g3 -O0>
			$<$<CONFIG:San>:-g3 -O0 -fsanitize=address>)
endif ()

if (CMAKE_C_FLAGS_RELEASE MATCHES "-O3")
	string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
endif ()
