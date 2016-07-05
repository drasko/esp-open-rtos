# Component makefile for extras/erpc
#
JSMN_DIR = $(erpc_ROOT)erpc/jsmn

# expected anyone using erpc library includes it as 'erpc/erpc.h'
INC_DIRS += $(erpc_ROOT)erpc/inc

# args for passing into compile rule generation
erpc_INC_DIR = $(erpc_ROOT)erpc/inc $(JSMN_DIR)
erpc_SRC_DIR = $(erpc_ROOT)erpc/src
erpc_EXTRA_SRC_FILES = $(wildcard $(JSMN_DIR)/*.c)

$(eval $(call component_compile_rules,erpc))

# Helpful error if git submodule not initialised
$(JSMN_DIR):
	$(error "jsmn git submodule not installed. Please run 'git submodule update --init --recursive'")
