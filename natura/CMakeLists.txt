# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
        grid/grass.tga
        grid/rock.tga
        grid/snow.tga
        grid/sand.tga
        skybox/top.tga
        skybox/left.tga
        skybox/right.tga
        skybox/back.tga
        skybox/bottom.tga
        skybox/front.tga)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
        grid/*.glsl
        perlin_quad/*.glsl
        skybox/*.glsl)

deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
