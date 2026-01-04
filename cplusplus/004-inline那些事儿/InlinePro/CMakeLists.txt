cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    InlinePro
    VERSION 1
    DESCRIPTION "1"
    HOMEPAGE_URL "1"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    InlinePro_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    InlinePro_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(InlinePro_lib PUBLIC cxx_std_11)

# ---- Declare executable ----

add_executable(InlinePro_exe source/main.cpp)
add_executable(InlinePro::exe ALIAS InlinePro_exe)

set_property(TARGET InlinePro_exe PROPERTY OUTPUT_NAME InlinePro)

target_compile_features(InlinePro_exe PRIVATE cxx_std_11)

target_link_libraries(InlinePro_exe PRIVATE InlinePro_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT InlinePro_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of InlinePro"
  )
endif()

include(cmake/dev-mode.cmake)
