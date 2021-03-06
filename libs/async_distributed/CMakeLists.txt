# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if(NOT HPX_WITH_DISTRIBUTED_RUNTIME)
  return()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(async_headers
    hpx/async.hpp
    hpx/modules/async_distributed.hpp
    hpx/async_distributed/applier/applier.hpp
    hpx/async_distributed/applier/apply_callback.hpp
    hpx/async_distributed/applier/apply_continue_callback.hpp
    hpx/async_distributed/applier/apply_continue_fwd.hpp
    hpx/async_distributed/applier/apply_continue.hpp
    hpx/async_distributed/applier/apply_helper.hpp
    hpx/async_distributed/applier/apply.hpp
    hpx/async_distributed/applier/bind_naming_wrappers.hpp
    hpx/async_distributed/applier/detail/apply_colocated_callback_fwd.hpp
    hpx/async_distributed/applier/detail/apply_colocated_callback.hpp
    hpx/async_distributed/applier/detail/apply_colocated_fwd.hpp
    hpx/async_distributed/applier/detail/apply_colocated.hpp
    hpx/async_distributed/applier/detail/apply_implementations_fwd.hpp
    hpx/async_distributed/applier/detail/apply_implementations.hpp
    hpx/async_distributed/applier_fwd.hpp
    hpx/async_distributed/applier/register_apply_colocated.hpp
    hpx/async_distributed/applier/trigger.hpp
    hpx/async_distributed/apply.hpp
    hpx/async_distributed/async_callback_fwd.hpp
    hpx/async_distributed/async_callback.hpp
    hpx/async_distributed/async_continue_callback_fwd.hpp
    hpx/async_distributed/async_continue_callback.hpp
    hpx/async_distributed/async_continue_fwd.hpp
    hpx/async_distributed/async_continue.hpp
    hpx/async_distributed/async.hpp
    hpx/async_distributed/dataflow.hpp
    hpx/async_distributed/detail/async_colocated_callback_fwd.hpp
    hpx/async_distributed/detail/async_colocated_callback.hpp
    hpx/async_distributed/detail/async_colocated_fwd.hpp
    hpx/async_distributed/detail/async_colocated.hpp
    hpx/async_distributed/detail/async_implementations_fwd.hpp
    hpx/async_distributed/detail/async_implementations.hpp
    hpx/async_distributed/detail/async_unwrap_result_implementations_fwd.hpp
    hpx/async_distributed/detail/async_unwrap_result_implementations.hpp
    hpx/async_distributed/detail/sync_implementations_fwd.hpp
    hpx/async_distributed/detail/sync_implementations.hpp
    hpx/async_distributed/sync.hpp
)

set(async_compat_headers
    hpx/apply.hpp
    hpx/dataflow.hpp
    hpx/lcos/async_callback_fwd.hpp
    hpx/lcos/async_callback.hpp
    hpx/lcos/async_continue_callback_fwd.hpp
    hpx/lcos/async_continue_callback.hpp
    hpx/lcos/async_continue_fwd.hpp
    hpx/lcos/async_continue.hpp
    hpx/lcos/async_fwd.hpp
    hpx/lcos/async.hpp
    hpx/lcos/dataflow.hpp
    hpx/lcos/sync_fwd.hpp
    hpx/lcos/sync.hpp
    hpx/runtime/applier/applier.hpp
    hpx/runtime/applier/apply_callback.hpp
    hpx/runtime/applier/apply_continue_callback.hpp
    hpx/runtime/applier/apply_continue_fwd.hpp
    hpx/runtime/applier/apply_continue.hpp
    hpx/runtime/applier/apply_helper.hpp
    hpx/runtime/applier/apply.hpp
    hpx/runtime/applier/bind_naming_wrappers.hpp
    hpx/runtime/applier_fwd.hpp
    hpx/runtime/applier/register_apply_colocated.hpp
    hpx/runtime/applier/trigger.hpp
    hpx/sync.hpp
)

include(HPX_AddModule)
add_hpx_module(
  async_distributed
  COMPATIBILITY_HEADERS ON
  DEPRECATION_WARNINGS
  GLOBAL_HEADER_GEN OFF
  HEADERS ${async_headers}
  COMPAT_HEADERS ${async_compat_headers}
  DEPENDENCIES
    hpx_actions_base
    hpx_actions
    hpx_allocator_support
    hpx_assertion
    hpx_config
    hpx_coroutines
    hpx_datastructures
    hpx_errors
    hpx_execution
    hpx_format
    hpx_functional
    hpx_futures
    hpx_async_local
    hpx_lcos_local
    hpx_memory
    hpx_runtime_configuration
    hpx_threading_base
    hpx_threadmanager
    hpx_type_support
    hpx_util
  CMAKE_SUBDIRS examples tests
)
