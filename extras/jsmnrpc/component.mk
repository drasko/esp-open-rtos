# Component makefile for extras/jsmnrpc

# expected anyone using onewire driver includes it as 'jsmnrpc/jsmnrpc.h'
# Note: we also need jsmn.h
INC_DIRS += $(jsmnrpc_ROOT)jsmnrpc/inc $(ROOT)/extras/jsmn/jsmn

# args for passing into compile rule generation
jsmnrpc_INC_DIR = $(jsmnrpc_ROOT)jsmnrpc/inc
jsmnrpc_SRC_DIR = $(jsmnrpc_ROOT)jsmnrpc/src

$(eval $(call component_compile_rules,jsmnrpc))
