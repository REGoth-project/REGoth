#
# Copyright 2011-2016 Branimir Karadzic. All rights reserved.
# License: http://www.opensource.org/licenses/BSD-2-Clause
#
# Usage:
#     make -f src/shaders/makefile rebuild-all

MAKEFILE_NAME := $(lastword $(MAKEFILE_LIST))
ROOT:=$(dir $(MAKEFILE_NAME))

BGFX_DIR=$(ROOT)../../lib/bgfx-cmake/bgfx
RUNTIME_DIR=$(BGFX_DIR)/examples/runtime
BUILD_DIR=$(ROOT)../../content
SHADERS_DIR=$(ROOT)

ADDITIONAL_INCLUDES=-i $(BGFX_DIR)/examples/common

include $(BGFX_DIR)/scripts/shader.mk

.PHONY: rebuild-all
rebuild-all:
	@make -s --no-print-directory -f $(MAKEFILE_NAME) TARGET=3 rebuild
	@make -s --no-print-directory -f $(MAKEFILE_NAME) TARGET=4 rebuild

.PHONY: clean-all
clean-all:
	@make -s --no-print-directory -f $(MAKEFILE_NAME) TARGET=3 clean
	@make -s --no-print-directory -f $(MAKEFILE_NAME) TARGET=4 clean
