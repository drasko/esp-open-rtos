# Component makefile for extras/cbor

# expected anyone using cbor lib includes it as 'cbor/cbor.h'
INC_DIRS += $(cbor_ROOT)inc

# args for passing into compile rule generation
cbor_SRC_DIR =  $(cbor_ROOT)src

$(eval $(call component_compile_rules,cbor))
