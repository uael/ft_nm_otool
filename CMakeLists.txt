cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(eiffel VERSION 0.0.1 LANGUAGES C ASM)

macro (add_target name)
  set(default_opt DEBUG=1 BUILD_DIR=${CMAKE_BINARY_DIR}/${name})
  add_custom_target(${name}_compiledb
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND rm -rf compile_commands.json
    COMMAND compiledb -n make ${default_opt} ${ARGN})
  add_custom_target(${name}
    COMMAND +make -C ${PROJECT_SOURCE_DIR} ${default_opt} ${ARGN})
  add_dependencies(${name} ${name}_compiledb)
endmacro()

add_target(eiffel bin)
