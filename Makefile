SUBMODULES = cql glm stb
SOURCES += lak opengl sdl lak_runtime lak_types lak_utils

ifeq ($(lak_SRC), )
lak_SRC = .
endif

export cql_SRC = $(lak_SRC)/cql
export glm_SRC = $(lak_SRC)/glm
export stb_SRC = $(lak_SRC)/stb

sdl_SRC = /usr/include/SDL2

opengl_SRC = $(lak_SRC)/GL
opengl_OBJ = gl3w.c
opengl_HDR = gl3w.h glcorearb.h
opengl_INC = $(lak_SRC)

lak_utils_SRC = $(lak_SRC)/utils
lak_utils_OBJ = stream.cpp
lak_utils_HDR = crc32_hash.h ldebug.h obj.h pnm.h stream.h type.h
lak_utils_INC = $(lak_SRC)
lak_utils_DEP = lak

lak_types_SRC = $(lak_SRC)/types
lak_types_OBJ = shader.cpp mesh.cpp queue.cpp stride_vector.cpp
lak_types_HDR = shader.h mesh.h json.h queue.h stride_vector.h texture.h type_list.h color.h image.h gltf.h
lak_types_INC = $(lak_SRC)
lak_types_DEP = lak lak_utils sdl glm opengl

lak_runtime_SRC = $(lak_SRC)/runtime
lak_runtime_OBJ = mainloop.cpp window.cpp
lak_runtime_HDR = mainloop.h window.h
lak_runtime_INC = $(lak_SRC)
lak_runtime_DEP = lak lak_types lak_utils glm sdl opengl

define SUBMODULE_TEMPLATE =
$(1)_MKF = $$($(1)_SRC)/Makefile
$$($(1)_SRC):
	( if [ ! -d $$@ ]; then $$(error Submodule $$@ not found, use `git submodule update --init --recursive` to initialise submodules); fi )
$$($(1)_MKF): | $$($(1)_SRC)
	( if [ ! -f $$@ ]; then $$(error Submodule $$($(1)_SRC) not initialised, use `git submodule update --init --recursive` to initialise submodules) ; fi )
include $$($(1)_MKF)
endef

$(foreach sub,$(SUBMODULES),$(eval $(call SUBMODULE_TEMPLATE,$(sub))))