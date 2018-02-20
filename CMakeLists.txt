cmake_minimum_required(VERSION 3.8)
project(cynical_engine_cpp)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DDEV)
add_definitions(-DGLM_COORDINATE_SYSTEM=GLM_RIGHT_HANDED)

set(plataform "x32")
#set(plataform "x64")

### DEPS
set(deps_dir "${CMAKE_SOURCE_DIR}/deps")
set(deps_include_dir "${CMAKE_SOURCE_DIR}/deps/include")
set(deps_bin_dir "${CMAKE_SOURCE_DIR}/deps/bin/${plataform}")
set(deps_lib_dir "${CMAKE_SOURCE_DIR}/deps/lib/${plataform}")

### LIBS
set(libs "-lopengl32 -lglu32 -lgdi32 -lglfw3dll -lglew32 -luser32 -lkernel32 -lOpenAl32")

include_directories("include;${deps_include_dir}")
set(SOURCE_FILES source/main.cpp include/graphics.h include/engine.h include/collections.h source/graphics.cpp include/log.h source/log.cpp source/utilities.cpp include/utilities.h include/entities.h include/maths.h source/maths.cpp source/entities.cpp include/core.h source/core.cpp include/input.h source/input.cpp include/time.h source/time.cpp include/audio.h deps/include/stb_vorbis.c source/audio.cpp source/palm_main.cpp source/editor.cpp include/editor.h)
add_executable(cynical_engine_cpp ${SOURCE_FILES})
target_link_libraries(cynical_engine_cpp -L${deps_lib_dir} ${libs})

file(COPY ${deps_bin_dir}/ DESTINATION .)
file(COPY data DESTINATION .)