# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.3.2 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Default location is $HPX_ROOT/libs/static_reinit/include
set(static_reinit_headers
  hpx/static_reinit/reinitializable_static.hpp
  hpx/static_reinit/static_reinit.hpp
  )

# Default location is $HPX_ROOT/libs/static_reinit/include_compatibility
set(static_reinit_compat_headers
  hpx/util/reinitializable_static.hpp
  hpx/util/static_reinit.hpp
  )

set(static_reinit_sources
  static_reinit.cpp
  )

include(HPX_AddModule)
add_hpx_module(static_reinit
  COMPATIBILITY_HEADERS ON
  DEPRECATION_WARNINGS
  FORCE_LINKING_GEN
  GLOBAL_HEADER_GEN OFF
  SOURCES ${static_reinit_sources}
  HEADERS ${static_reinit_headers}
  COMPAT_HEADERS ${static_reinit_compat_headers}
  DEPENDENCIES
    hpx_config
    hpx_concurrency
    hpx_functional
    hpx_type_support
  CMAKE_SUBDIRS examples tests
)