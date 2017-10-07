#
# Copyright 2011-2016 Branimir Karadzic. All rights reserved.
# License: http://www.opensource.org/licenses/BSD-2-Clause
#

BGFX_DIR=../../lib/bgfx-cmake/bgfx
RUNTIME_DIR=$(BGFX_DIR)/examples/runtime
BUILD_DIR=../../content/

ADDITIONAL_INCLUDES=-i $(BGFX_DIR)/examples/common

include $(BGFX_DIR)/scripts/shader.mk

.PHONY: rebuild
rebuild:
	@make -s --no-print-directory TARGET=3 clean all
	@make -s --no-print-directory TARGET=4 clean all

.PHONY: cleanall
cleanall:
	@make -s --no-print-directory TARGET=3 clean
	@make -s --no-print-directory TARGET=4 clean
