# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
        grid/*.glsl
        perlin_quad/*.glsl)

deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS} terrain/chunk/chunk.h)
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
