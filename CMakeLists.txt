cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(eiffel VERSION 0.0.1 LANGUAGES C ASM)

macro (add_target name)
  set(default_opt DEBUG=1)
  add_custom_target(${name}
    COMMAND +make -C ${PROJECT_SOURCE_DIR} ${default_opt} ${ARGN})
endmacro()

add_target(nm bin)
add_target(otool bin)
