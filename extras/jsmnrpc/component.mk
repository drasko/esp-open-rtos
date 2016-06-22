# Component makefile for extras/jsmnrpc

# expected anyone using onewire driver includes it as 'jsmnrpc/jsmnrpc.h'
INC_DIRS += $(jsmnrpc_ROOT)jsmnrpc/inc

# args for passing into compile rule generation
jsmn_INC_DIR = $(jsmnrpc_ROOT)jsmnrpc/inc
jsmn_SRC_DIR = $(jsmnrpc_ROOT)jsmnrpc/src

$(eval $(call component_compile_rules,jsmnrpc))
