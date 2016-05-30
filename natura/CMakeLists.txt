# the exercise name is nothing else than the directory
get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# these files are copied only once
file(GLOB TEXTURES
        grid/grass.tga
        grid/grass2.tga
        grid/rock.tga
        grid/snow.tga
        grid/sand.tga
        grid/water.tga
        skybox/top.tga
        skybox/left.tga
        skybox/right.tga
        skybox/back.tga
        skybox/bottom.tga
        grass/grassPack.dds
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
        axis/axis_vshader.glsl
        physics/ball_vshader.glsl
        physics/ball_fshader.glsl
        camera/bezier/bezier_fshader.glsl
        camera/bezier/bezier_vshader.glsl
        shadows/shadow_map_fshader.glsl
        shadows/shadow_map_vshader.glsl)

deploy_shaders_to_build_dir(${SHADERS})


# these files are copied only once
file(GLOB OBJ_FILES
        physics/sphere.obj)
copy_files_once(${OBJ_FILES})


add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS} )
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
