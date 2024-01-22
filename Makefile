
build_platform=native-pthread
# build_platform=web-wasm-pthread
# build_platform=web-wasm-webworker

build_mode=release
# build_mode=debug

include Makefile.env

#

# ifeq ($(build_platform),native-pthread)
# # $(info build_platform is valid, value=$(build_platform))
# else ifeq ($(build_platform),web-wasm-pthread)
# # $(info build_platform is valid, value=$(build_platform))
# else ifeq ($(build_platform),web-wasm-webworker)
# # $(info build_platform is valid, value=$(build_platform))
# else
# $(error unsupported value for "build_platform", value=$(build_platform))
# endif

# #

# ifeq ($(build_mode),release)
# # $(info build_mode is valid, value=$(build_mode))
# else ifeq ($(build_mode),debug)
# # $(info build_mode is valid, value=$(build_mode))
# else
# $(error unsupported value for "build_mode", value=$(build_mode))
# endif

# LOG_INFO= '[$(build_mode)] [$(build_platform)]'

#

ifeq ($(build_platform),native-pthread)

DIR_TARGET=					./bin
NAME_APPLICATION=		$(DIR_TARGET)/exec

else ifeq ($(build_platform),web-wasm-pthread)

DIR_TARGET=					./dist/wasm/pthread
NAME_APPLICATION=		$(DIR_TARGET)/index.js

else ifeq ($(build_platform),web-wasm-webworker)

DIR_TARGET=					./dist/wasm/webworker
NAME_APPLICATION=		$(DIR_TARGET)/index.js
NAME_WEB_WASM_WORKER=		$(DIR_TARGET)/worker.js

endif

#

DIR_LIB_BASIC_GENETIC_ALGORITHM=	./thirdparties/dependencies/basic-genetic-algorithm
DIR_LIB_GERONIMO=									./thirdparties/dependencies/geronimo

ifeq ($(build_platform),native-pthread)

NAME_LIB_BASIC_GENTIC_ALGORITHM=	$(DIR_LIB_BASIC_GENETIC_ALGORITHM)/lib/native/lib-basic-genetic-algorithm.a

#

NAME_LIB_GERONIMO_SYSTEM=		$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-system.a
NAME_LIB_GERONIMO_GRAPHIC=	$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-graphics.a
NAME_LIB_GERONIMO_PHYSICS=	$(DIR_LIB_GERONIMO)/lib/native/lib-geronimo-physics.a
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/native/lib-bullet-physics-dynamics.a
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/native/lib-bullet-physics-collision.a
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/native/lib-bullet-physics-linearmath.a

else

NAME_LIB_BASIC_GENTIC_ALGORITHM=	$(DIR_LIB_BASIC_GENETIC_ALGORITHM)/lib/web-wasm/lib-basic-genetic-algorithm.bc

#

NAME_LIB_GERONIMO_SYSTEM=		$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-system.bc
NAME_LIB_GERONIMO_GRAPHIC=	$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-graphics.bc
NAME_LIB_GERONIMO_PHYSICS=	$(DIR_LIB_GERONIMO)/lib/web-wasm/lib-geronimo-physics.bc
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/web-wasm/lib-bullet-physics-linearmath.bc
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/web-wasm/lib-bullet-physics-collision.bc
NAME_LIB_BULLET_PHYSICS+=		$(DIR_LIB_GERONIMO)/thirdparties/lib/web-wasm/lib-bullet-physics-dynamics.bc

endif



#### DIRS

DIR_SRC=														./src

#### /DIRS



ifeq ($(build_platform),native-pthread)

DIR_OBJ=									./obj/native

else

ifeq ($(build_platform),web-wasm-pthread)

DIR_OBJ=									./obj/web-wasm/pthread

else ifeq ($(build_platform),web-wasm-webworker)

DIR_OBJ=									./obj/web-wasm/webworker/main
DIR_OBJ_WEB_WASM_WORKER=	./obj/web-wasm/webworker/worker

endif

endif

#### SRC

# pthread (native + web-pthread)

SRC+=	\
	$(wildcard \
		$(DIR_SRC)/*.cpp \
		$(DIR_SRC)/application/*.cpp \
		$(DIR_SRC)/application/states/*.cpp \
		$(DIR_SRC)/application/context/*.cpp \
		\
		$(DIR_SRC)/application/context/graphics/*.cpp \
		$(DIR_SRC)/application/context/graphics/camera/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/*.cpp \
		\
		$(DIR_SRC)/application/context/graphics/renderers/hud/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/hud/helpers/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/hud/post-process/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/hud/widgets/*.cpp \
		\
		$(DIR_SRC)/application/context/graphics/renderers/scene/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/animated-circuit/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/background-torus/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/car-tails/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/chessboard-floor/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/flocking/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/flocking/internals/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/models-car/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/particles/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/particles/internals/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/scene-stack-renderers/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/shape-stack-renderer/*.cpp \
		$(DIR_SRC)/application/context/graphics/renderers/scene/shape-stack-renderer/internals/*.cpp \
		\
		$(DIR_SRC)/application/context/logic/*.cpp \
		$(DIR_SRC)/application/context/logic/handlers/*.cpp \
		$(DIR_SRC)/application/context/logic/simulation/*.cpp \
		$(DIR_SRC)/application/context/logic/simulation/logic/*.cpp \
		)

ifneq ($(build_platform),web-wasm-webworker)

SRC+=	\
	$(wildcard \
		$(DIR_SRC)/application/context/logic/simulation/pthread/*.cpp \
		)

else

SRC+=	\
	$(wildcard \
		$(DIR_SRC)/application/context/logic/simulation/webworker/*.cpp \
		$(DIR_SRC)/application/context/logic/simulation/webworker/producer/*.cpp \
		)

SRC_WEB_WASM_WORKER+=	\
	$(wildcard \
		$(DIR_SRC)/application/context/logic/simulation/logic/*.cpp \
		$(DIR_SRC)/application/context/logic/simulation/webworker/consumer/*.cpp \
		)

endif

#

OBJ=	$(patsubst %.cpp, $(DIR_OBJ)/%.o, $(SRC))

ifeq ($(build_platform),web-wasm-webworker)

OBJ_WEB_WASM_WORKER=	$(patsubst %.cpp, $(DIR_OBJ_WEB_WASM_WORKER)/%.o, $(SRC_WEB_WASM_WORKER))

endif

#######


ifeq ($(build_mode),release)

BUILD_FLAG=		-O3 # optimisation flag

else

BUILD_FLAG=		-g3 # debug flag

endif

# DEPENDENCIES += sdl2 glesv2 openal
DEPENDENCIES_LIST += sdl2 glesv2
DEPENDENCIES_CXXFLAGS += $(shell pkg-config $(DEPENDENCIES_LIST) --cflags)
DEPENDENCIES_LDFLAGS += $(shell pkg-config $(DEPENDENCIES_LIST) --libs)

CXXFLAGS += $(BUILD_FLAG)
CXXFLAGS += -std=c++17
CXXFLAGS += -Wall -W -Wextra -Wunused
CXXFLAGS += -I$(DIR_SRC)
CXXFLAGS += -I$(DIR_LIB_BASIC_GENETIC_ALGORITHM)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/src
CXXFLAGS += -I$(DIR_LIB_GERONIMO)/thirdparties

ifeq ($(build_platform),native-pthread)

CXX=clang++
AR=ar

CXXFLAGS += $(DEPENDENCIES_CXXFLAGS)

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_GERONIMO_PHYSICS)
LDFLAGS += $(NAME_LIB_GERONIMO_GRAPHIC)
LDFLAGS += $(NAME_LIB_GERONIMO_SYSTEM)
LDFLAGS += $(NAME_LIB_BASIC_GENTIC_ALGORITHM)
LDFLAGS += $(NAME_LIB_BULLET_PHYSICS)
LDFLAGS += $(DEPENDENCIES_LDFLAGS)
LDFLAGS += -pthread

else

CXX=em++
AR=emar

CXXFLAGS += -s USE_SDL=2

LDFLAGS_COMMON_WEB_WASM += -s USE_SDL=2
LDFLAGS_COMMON_WEB_WASM += -s USE_WEBGL2=1
LDFLAGS_COMMON_WEB_WASM += -s FULL_ES3=1
LDFLAGS_COMMON_WEB_WASM += -s WASM=1
LDFLAGS_COMMON_WEB_WASM += -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1
LDFLAGS_COMMON_WEB_WASM += -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0
LDFLAGS_COMMON_WEB_WASM += -s EXPORTED_RUNTIME_METHODS=cwrap

ifeq ($(build_mode),debug)

# LDFLAGS_COMMON_WEB_WASM += -s DEMANGLE_SUPPORT=1
# LDFLAGS_COMMON_WEB_WASM += -s GL_ASSERTIONS=1
# LDFLAGS_COMMON_WEB_WASM += -s GL_DEBUG=1
LDFLAGS_COMMON_WEB_WASM += -s NO_DISABLE_EXCEPTION_CATCHING
LDFLAGS_COMMON_WEB_WASM += -s ASSERTIONS

endif

ifeq ($(build_platform),web-wasm-pthread)

FLAGS_WEB_WASM_MAIN += -s USE_PTHREADS=1

CXXFLAGS += $(FLAGS_WEB_WASM_MAIN)

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(FLAGS_WEB_WASM_MAIN)
LDFLAGS += $(NAME_LIB_GERONIMO_PHYSICS)
LDFLAGS += $(NAME_LIB_GERONIMO_GRAPHIC)
LDFLAGS += $(NAME_LIB_GERONIMO_SYSTEM)
LDFLAGS += $(NAME_LIB_BASIC_GENTIC_ALGORITHM)
LDFLAGS += $(NAME_LIB_BULLET_PHYSICS)
LDFLAGS += $(LDFLAGS_COMMON_WEB_WASM)
LDFLAGS += --preload-file ./assets/
LDFLAGS += --preload-file $(DIR_LIB_GERONIMO)/src/geronimo/graphics/advanced-concept/textRenderer/assets
LDFLAGS += --preload-file $(DIR_LIB_GERONIMO)/src/geronimo/graphics/advanced-concept/stackRenderers/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/hud/post-process/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/animated-circuit/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/background-torus/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/car-tails/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/chessboard-floor/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/models-car/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/scene-stack-renderers/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/shape-stack-renderer/internals/shaders
LDFLAGS += -s EXPORT_NAME="selfDriving3dCars"
LDFLAGS += -s MODULARIZE=1

# pthread build: the application need more memory
LDFLAGS += -s TOTAL_MEMORY=256Mb # 16Kb, 256Mb, etc.

LDFLAGS += -Wl,--shared-memory,--no-check-features
LDFLAGS += -s PTHREAD_POOL_SIZE=17

else

LDFLAGS += $(BUILD_FLAG)
LDFLAGS += $(NAME_LIB_GERONIMO_GRAPHIC)
LDFLAGS += $(NAME_LIB_GERONIMO_SYSTEM)
LDFLAGS += $(NAME_LIB_BASIC_GENTIC_ALGORITHM)
LDFLAGS += $(LDFLAGS_COMMON_WEB_WASM)
LDFLAGS += --preload-file ./assets/
LDFLAGS += --preload-file $(DIR_LIB_GERONIMO)/src/geronimo/graphics/advanced-concept/textRenderer/assets
LDFLAGS += --preload-file $(DIR_LIB_GERONIMO)/src/geronimo/graphics/advanced-concept/stackRenderers/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/hud/post-process/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/animated-circuit/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/background-torus/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/car-tails/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/chessboard-floor/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/models-car/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/scene-stack-renderers/shaders
LDFLAGS += --preload-file $(DIR_SRC)/application/context/graphics/renderers/scene/shape-stack-renderer/internals/shaders
LDFLAGS += -s EXPORT_NAME="selfDriving3dCars"
LDFLAGS += -s MODULARIZE=1

# webworker build: main script need more memory
LDFLAGS += -s TOTAL_MEMORY=256Mb # 16Kb, 256Mb, etc.

LDFLAGS_WEB_WASM_WORKER += $(BUILD_FLAG)
LDFLAGS_WEB_WASM_WORKER += $(NAME_LIB_GERONIMO_PHYSICS)
LDFLAGS_WEB_WASM_WORKER += $(NAME_LIB_GERONIMO_SYSTEM)
LDFLAGS_WEB_WASM_WORKER += $(NAME_LIB_BASIC_GENTIC_ALGORITHM)
LDFLAGS_WEB_WASM_WORKER += $(NAME_LIB_BULLET_PHYSICS)
LDFLAGS_WEB_WASM_WORKER += $(LDFLAGS_COMMON_WEB_WASM)
LDFLAGS_WEB_WASM_WORKER += -s BUILD_AS_WORKER=1

# webworker build: worker script need less memory
LDFLAGS_WEB_WASM_WORKER += -s TOTAL_MEMORY=128Mb # 16Kb, 256Mb, etc.

endif

endif


RM=			rm -rf


#######

#
## RULE(S)

ifneq ($(build_platform),web-wasm-webworker)

all:	application

else ifeq ($(build_platform),web-wasm-webworker)

all:	application web_wasm_worker

endif

ensurefolders:
	@mkdir -p `dirname $(NAME_APPLICATION)`

application:	ensurefolders $(OBJ)
	@echo ' ---> building $(LOG_INFO): "application"'
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME_APPLICATION) $(LDFLAGS)
	@echo '   --> built $(LOG_INFO): "application"'

ifeq ($(build_platform),web-wasm-webworker)

web_wasm_worker:		ensurefolders $(OBJ_WEB_WASM_WORKER)
	@echo ' ---> building $(LOG_INFO): "web wasm worker application"'
	@$(CXX) $(CXXFLAGS) $(OBJ_WEB_WASM_WORKER) -o $(NAME_WEB_WASM_WORKER) $(LDFLAGS_WEB_WASM_WORKER)
	@echo '   --> built $(LOG_INFO): "web wasm worker application"'

endif

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' ---> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_OBJ) && find $(DIR_OBJ) -name "*.dep")

ifeq ($(build_platform),web-wasm-webworker)

$(DIR_OBJ_WEB_WASM_WORKER)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' ---> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_OBJ_WEB_WASM_WORKER) && find $(DIR_OBJ_WEB_WASM_WORKER) -name "*.dep")

endif

#

ifneq ($(build_platform),web-wasm-webworker)

clean:
	@echo ' -> cleaning $(LOG_INFO): application build file(s)'
	$(RM) $(DIR_OBJ)
	@echo '   -> cleaned $(LOG_INFO): application build file(s)'

fclean:	clean
	@echo ' -> cleaning $(LOG_INFO): application file(s)'
	$(RM) $(NAME_APPLICATION)
	@echo '   -> cleaned $(LOG_INFO): application file(s)'

else ifeq ($(build_platform),web-wasm-webworker)

clean:
	@echo ' -> cleaning $(LOG_INFO): application and webworker build file(s)'
	$(RM) $(DIR_OBJ)
	$(RM) $(DIR_OBJ_WEB_WASM_WORKER)
	@echo '   -> cleaned $(LOG_INFO): application and webworker build file(s)'

fclean:	clean
	@echo ' -> cleaning $(LOG_INFO): application and webworker file(s)'
	$(RM) $(NAME_APPLICATION)
	$(RM) $(NAME_WEB_WASM_WORKER)
	@echo '   -> cleaned $(LOG_INFO): application and webworker file(s)'

endif

re:			fclean all

.PHONY: \
	all \
	application \
	web_wasm_worker \
	clean \
	fclean \
	re

## RULE(S)
#
