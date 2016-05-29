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
        grid/water.tga
        skybox/top.tga
        skybox/left.tga
        skybox/right.tga
        skybox/back.tga
        skybox/bottom.tga
        skybox/front.tga
        skybox/stormydays_large.tga)
copy_files_once(${TEXTURES})

# list all the files you want to copy everytime
# you build (ie, you want the shaders there)
file(GLOB SHADERS
        grid/*.glsl
        perlin_quad/*.glsl
        grass/*.glsl
        skybox/*.glsl
        water_grid/*.glsl
        axis/axis_fshader.glsl
        axis/axis_vshader.glsl)

deploy_shaders_to_build_dir(${SHADERS})

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS} terrain/chunk/chunk.h game/game.h misc/observer_subject/observer.h misc/observer_subject/subject.h misc/observer_subject/messages/message.h misc/io/input/handlers/keyboard/keyboard_handler.h misc/observer_subject/messages/keyboard_handler_message.h misc/observer_subject/messages/perlin_noise_prop_changed_message.h misc/io/input/handlers/mouse/mouse_button_handler.h misc/io/input/handlers/handler.h misc/observer_subject/messages/mouse_button_handler_message.h misc/io/input/handlers/mouse/mouse_cursor_handler.h misc/observer_subject/messages/mouse_cursor_handler_message.h misc/io/input/handlers/framebuffer/framebuffer_size_handler.h misc/observer_subject/messages/framebuffer_size_handler_message.h terrain/chunk/chunk_generation/chunk_factory.h axis/axis.h physics/material_point.h)
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
