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
        skybox/skybox_texture.tga)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
        grid/*.glsl
        perlin_quad/*.glsl
        skybox/*.glsl)

deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS} terrain/chunk/chunk.h game/game.h misc/observer_subject/observer.h misc/observer_subject/subject.h misc/observer_subject/messages/message.h)
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
