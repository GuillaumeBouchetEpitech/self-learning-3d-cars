
build_platform=native
# build_platform=web_wasm

build_mode=release
# build_mode=debug

#

ifeq ($(build_platform),native)

# $(info build_platform is valid, value=$(build_platform))

else ifeq ($(build_platform),web_wasm)

# $(info build_platform is valid, value=$(build_platform))

else

$(error unsupported value for "build_platform", value=$(build_platform))

endif

LOG_INFO= '[$(build_mode)] [$(build_platform)]'

#

ifeq ($(build_platform),native)

DIR_LIB=		./lib/native

NAME_GERONIMO_SYSTEM=						$(DIR_LIB)/lib-geronimo-system.a
NAME_GERONIMO_GRAPHIC=					$(DIR_LIB)/lib-geronimo-graphic.a
NAME_GERONIMO_PHYSIC=						$(DIR_LIB)/lib-geronimo-physic.a
NAME_GERONIMO_AUDIO=						$(DIR_LIB)/lib-geronimo-audio.a
NAME_BULLET_PHYSICS_LINEARMATH=	$(DIR_LIB)/lib-bullet-physics-linearmath.a
NAME_BULLET_PHYSICS_COLLISION=	$(DIR_LIB)/lib-bullet-physics-collision.a
NAME_BULLET_PHYSICS_DYNAMICS=		$(DIR_LIB)/lib-bullet-physics-dynamics.a

else ifeq ($(build_platform),web_wasm)

DIR_LIB=		./lib/web-wasm

NAME_GERONIMO_SYSTEM=						$(DIR_LIB)/lib-geronimo-system.bc
NAME_GERONIMO_GRAPHIC=					$(DIR_LIB)/lib-geronimo-graphic.bc
NAME_GERONIMO_PHYSIC=						$(DIR_LIB)/lib-geronimo-physic.bc
NAME_GERONIMO_AUDIO=						$(DIR_LIB)/lib-geronimo-audio.bc
NAME_BULLET_PHYSICS_LINEARMATH=	$(DIR_LIB)/lib-bullet-physics-linearmath.bc
NAME_BULLET_PHYSICS_COLLISION=	$(DIR_LIB)/lib-bullet-physics-collision.bc
NAME_BULLET_PHYSICS_DYNAMICS=		$(DIR_LIB)/lib-bullet-physics-dynamics.bc

endif



#### DIRS

DIR_SRC=									./src
DIR_THIRDPARTY=						$(DIR_SRC)/thirdparty
DIR_BULLET_PHYSICS_SRC=		$(DIR_THIRDPARTY)/bullet3/src

#### /DIRS



ifeq ($(build_platform),native)

DIR_OBJ=			./obj/native

else ifeq ($(build_platform),web_wasm)

DIR_OBJ=			./obj/web-wasm

endif

DIR_OBJ_GERONIMO=						$(DIR_OBJ)/geronimo
DIR_OBJ_BULLET_PHYSICS=		$(DIR_OBJ)/bullet-physics


#### SRC

SRC_GERONIMO_SYSTEM+=	\
	$(wildcard \
		$(DIR_SRC)/geronimo/system/*.cpp \
		$(DIR_SRC)/geronimo/system/compression/*.cpp \
		$(DIR_SRC)/geronimo/system/containers/*.cpp \
		$(DIR_SRC)/geronimo/system/easing/*.cpp \
		$(DIR_SRC)/geronimo/system/file-utils/*.cpp \
		$(DIR_SRC)/geronimo/system/math/*.cpp \
		$(DIR_SRC)/geronimo/system/messaging/*.cpp \
		$(DIR_SRC)/geronimo/system/multithreading/*.cpp \
		$(DIR_SRC)/geronimo/system/multithreading/internals/*.cpp \
		$(DIR_SRC)/geronimo/system/parser-utils/*.cpp \
		$(DIR_SRC)/geronimo/system/rng/*.cpp \
		$(DIR_SRC)/geronimo/system/rng/proceduralGeneration/*.cpp \
		$(DIR_SRC)/geronimo/system/string-utils/*.cpp \
		)

SRC_GERONIMO_GRAPHIC+=	\
	$(wildcard \
		$(DIR_SRC)/geronimo/graphic/*.cpp \
		$(DIR_SRC)/geronimo/graphic/camera/*.cpp \
		$(DIR_SRC)/geronimo/graphic/loaders/*.cpp \
		$(DIR_SRC)/geronimo/graphic/make-geometries/*.cpp \
		$(DIR_SRC)/geronimo/graphic/sdl/*.cpp \
		$(DIR_SRC)/geronimo/graphic/vertexBuffers/*.cpp \
		$(DIR_SRC)/geronimo/graphic/wrappers/*.cpp \
		)

SRC_GERONIMO_PHYSIC+=	\
	$(wildcard \
		$(DIR_SRC)/geronimo/physic/*.cpp \
		$(DIR_SRC)/geronimo/physic/body/*.cpp \
		$(DIR_SRC)/geronimo/physic/helpers/*.cpp \
		$(DIR_SRC)/geronimo/physic/queries/*.cpp \
		$(DIR_SRC)/geronimo/physic/shape/*.cpp \
		$(DIR_SRC)/geronimo/physic/vehicle/*.cpp \
		)

SRC_GERONIMO_AUDIO+=	\
	$(wildcard \
		$(DIR_SRC)/geronimo/audio/*.cpp \
		$(DIR_SRC)/geronimo/audio/decoders/*.cpp \
		)

SRC_BULLET_PHYSICS_LINEARMATH+= \
	$(wildcard \
		$(DIR_BULLET_PHYSICS_SRC)/LinearMath/*.cpp \
		)

SRC_BULLET_PHYSICS_COLLISION+= \
	$(wildcard \
		$(DIR_BULLET_PHYSICS_SRC)/BulletCollision/BroadphaseCollision/*.cpp \
		$(DIR_BULLET_PHYSICS_SRC)/BulletCollision/CollisionShapes/*.cpp \
		$(DIR_BULLET_PHYSICS_SRC)/BulletCollision/CollisionDispatch/*.cpp \
		$(DIR_BULLET_PHYSICS_SRC)/BulletCollision/NarrowPhaseCollision/*.cpp \
		)

# TODO: this will be needed for non-static mesh collider support
# SRC_BULLET_PHYSICS_COLLISION+= $(wildcard $(DIR_BULLET_PHYSICS_SRC)/BulletCollision/Gimpact/*.cpp)

SRC_BULLET_PHYSICS_DYNAMICS+= \
	$(wildcard \
		$(DIR_BULLET_PHYSICS_SRC)/BulletDynamics/Dynamics/*.cpp \
		$(DIR_BULLET_PHYSICS_SRC)/BulletDynamics/Vehicle/*.cpp \
		$(DIR_BULLET_PHYSICS_SRC)/BulletDynamics/ConstraintSolver/*.cpp) # TODO: useful?

#

OBJ_GERONIMO_SYSTEM=		$(patsubst %.cpp, $(DIR_OBJ_GERONIMO)/%.o, $(SRC_GERONIMO_SYSTEM))
OBJ_GERONIMO_GRAPHIC=	$(patsubst %.cpp, $(DIR_OBJ_GERONIMO)/%.o, $(SRC_GERONIMO_GRAPHIC))
OBJ_GERONIMO_PHYSIC=		$(patsubst %.cpp, $(DIR_OBJ_GERONIMO)/%.o, $(SRC_GERONIMO_PHYSIC))
OBJ_GERONIMO_AUDIO=		$(patsubst %.cpp, $(DIR_OBJ_GERONIMO)/%.o, $(SRC_GERONIMO_AUDIO))

OBJ_BULLET_PHYSICS_LINEARMATH=	$(patsubst %.cpp, $(DIR_OBJ_BULLET_PHYSICS)/%.o, $(SRC_BULLET_PHYSICS_LINEARMATH))
OBJ_BULLET_PHYSICS_COLLISION=		$(patsubst %.cpp, $(DIR_OBJ_BULLET_PHYSICS)/%.o, $(SRC_BULLET_PHYSICS_COLLISION))
OBJ_BULLET_PHYSICS_DYNAMICS=		$(patsubst %.cpp, $(DIR_OBJ_BULLET_PHYSICS)/%.o, $(SRC_BULLET_PHYSICS_DYNAMICS))

#

#######


ifeq ($(build_mode),release)

BUILD_FLAG= -O3

else

BUILD_FLAG= -g3

endif


CXXFLAGS_COMMON_BULLET_PHYSICS += -DBT_NO_PROFILE
CXXFLAGS_COMMON_BULLET_PHYSICS += -DSIMD_FORCE_INLINE=inline
# CXXFLAGS_COMMON_BULLET_PHYSICS += -DBT_USE_DOUBLE_PRECISION


CXXFLAGS_BULLET_PHYSICS += $(BUILD_FLAG)
CXXFLAGS_BULLET_PHYSICS += -std=c++17
CXXFLAGS_BULLET_PHYSICS += -I$(DIR_BULLET_PHYSICS_SRC)
CXXFLAGS_BULLET_PHYSICS += $(CXXFLAGS_COMMON_BULLET_PHYSICS)

COMMON_FLAGS += $(CXXFLAGS_BULLET_PHYSICS)
COMMON_FLAGS += -Wall -W -Wextra -Wunused -Wpedantic -Wshadow -Wconversion -Werror
COMMON_FLAGS += -I$(DIR_SRC)
COMMON_FLAGS += $(CXXFLAGS_COMMON_BULLET_PHYSICS)

ifeq ($(build_platform),native)

CXX=clang++
AR=ar

# DEPENDENCIES += sdl2 glesv2 egl openal
DEPENDENCIES_LIST += sdl2 glesv2 egl
DEPENDENCIES_CXXFLAGS += $(shell pkg-config $(DEPENDENCIES_LIST) --cflags)
# DEPENDENCIES_LDFLAGS += $(shell pkg-config $(DEPENDENCIES_LIST) --libs)

CXXFLAGS_GERONIMO += $(COMMON_FLAGS)
CXXFLAGS_GERONIMO += $(DEPENDENCIES_CXXFLAGS)

else ifeq ($(build_platform),web_wasm)

CXX=em++
AR=emar

CXXFLAGS_GERONIMO += $(COMMON_FLAGS)
CXXFLAGS_GERONIMO += -s USE_SDL=2
CXXFLAGS_GERONIMO += -s USE_PTHREADS=0

endif


RM=			rm -rf


#######

#
## RULE(S)

all:	\
	bullet_physics	\
	geronimo

ensure_folders:
	@mkdir -p $(DIR_LIB)

geronimo:	\
	ensure_folders	\
	geronimo_system	\
	geronimo_graphic	\
	geronimo_physic	\
	geronimo_audio

geronimo_system:	ensure_folders $(OBJ_GERONIMO_SYSTEM)
	@echo ' ---> building $(LOG_INFO): "geronimo system library"'
	@$(AR) cr $(NAME_GERONIMO_SYSTEM) $(OBJ_GERONIMO_SYSTEM)
	@echo '   --> built $(LOG_INFO): "geronimo system library"'

geronimo_graphic:	ensure_folders $(OBJ_GERONIMO_GRAPHIC)
	@echo ' ---> building $(LOG_INFO): "geronimo graphic library"'
	@$(AR) cr $(NAME_GERONIMO_GRAPHIC) $(OBJ_GERONIMO_GRAPHIC)
	@echo '   --> built $(LOG_INFO): "geronimo graphic library"'

geronimo_physic:	ensure_folders $(OBJ_GERONIMO_PHYSIC)
	@echo ' ---> building $(LOG_INFO): "geronimo physic library"'
	@$(AR) cr $(NAME_GERONIMO_PHYSIC) $(OBJ_GERONIMO_PHYSIC)
	@echo '   --> built $(LOG_INFO): "geronimo physic library"'

geronimo_audio:	ensure_folders $(OBJ_GERONIMO_AUDIO)
	@echo ' ---> building $(LOG_INFO): "geronimo audio library"'
	@$(AR) cr $(NAME_GERONIMO_AUDIO) $(OBJ_GERONIMO_AUDIO)
	@echo '   --> built $(LOG_INFO): "geronimo audio library"'

bullet_physics: \
	ensure_folders \
	bullet_physics_linearmath \
	bullet_physics_collision \
	bullet_physics_dynamics

bullet_physics_linearmath: ensure_folders $(OBJ_BULLET_PHYSICS_LINEARMATH)
	@echo ' ---> building $(LOG_INFO): "bullet linear math library"'
	@$(AR) cr $(NAME_BULLET_PHYSICS_LINEARMATH) $(OBJ_BULLET_PHYSICS_LINEARMATH)
	@echo '   --> built $(LOG_INFO): "bullet linear math library"'

bullet_physics_collision:	ensure_folders $(OBJ_BULLET_PHYSICS_COLLISION)
	@echo ' ---> building $(LOG_INFO): "bullet collision library"'
	@$(AR) cr $(NAME_BULLET_PHYSICS_COLLISION) $(OBJ_BULLET_PHYSICS_COLLISION)
	@echo '   --> built $(LOG_INFO): "bullet collision library"'

bullet_physics_dynamics:	ensure_folders $(OBJ_BULLET_PHYSICS_DYNAMICS)
	@echo ' ---> building $(LOG_INFO): "bullet dynamics library"'
	@$(AR) cr $(NAME_BULLET_PHYSICS_DYNAMICS) $(OBJ_BULLET_PHYSICS_DYNAMICS)
	@echo '   --> built $(LOG_INFO): "bullet dynamics library"'

#

# for every ".cpp" file
# => ensure the "obj" folder(s)
# => compile in a ".o" file

$(DIR_OBJ_GERONIMO)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) -c $(CXXFLAGS_GERONIMO) -MMD -MT "$@" -MF "$@.dep" -o "$@" $<

include $(shell test -d $(DIR_OBJ_GERONIMO) && find $(DIR_OBJ_GERONIMO) -name "*.dep")

$(DIR_OBJ_BULLET_PHYSICS)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo ' --> processing $(LOG_INFO):' $<
	@$(CXX) $(CXXFLAGS_BULLET_PHYSICS) $< -c -o $@

#

clean:
	@echo ' -> cleaning $(LOG_INFO): geronimo library build file(s)'
	@$(RM) $(DIR_OBJ_GERONIMO)
	@echo '   -> cleaned $(LOG_INFO): geronimo library build file(s)'

fclean: clean
	@echo ' -> cleaning $(LOG_INFO): geronimo library file(s)'
	@$(RM) $(NAME_GERONIMO_SYSTEM)
	@$(RM) $(NAME_GERONIMO_GRAPHIC)
	@$(RM) $(NAME_GERONIMO_PHYSIC)
	@$(RM) $(NAME_GERONIMO_AUDIO)
	@echo '   -> cleaned $(LOG_INFO): geronimo library file(s)'

clean_bullet_physics:
	@echo ' -> cleaning $(LOG_INFO): bullet physics library build file(s)'
	@$(RM) $(DIR_OBJ_BULLET_PHYSICS)
	@echo '   -> cleaned $(LOG_INFO): bullet physics library build file(s)'

fclean_bullet_physics: clean_bullet_physics
	@echo ' -> cleaning $(LOG_INFO): bullet physics library file(s)'
	@$(RM) $(NAME_BULLET_PHYSICS_LINEARMATH)
	@$(RM) $(NAME_BULLET_PHYSICS_COLLISION)
	@$(RM) $(NAME_BULLET_PHYSICS_DYNAMICS)
	@echo '   -> cleaned $(LOG_INFO): bullet physics library file(s)'

re:			fclean all

.PHONY:	\
		all \
		geronimo \
		clean \
		fclean \
		re \
		bullet_physics \
		bullet_physics_linearmath \
		bullet_physics_collision \
		bullet_physics_dynamics \
		clean_bullet_physics \
		fclean_bullet_physics

## RULE(S)
#
