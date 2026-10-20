cmake_minimum_required(VERSION 4.0)
project(arpg C)

set(CMAKE_C_STANDARD 17)

add_executable(arpg src/game/main.c)

target_sources(arpg PRIVATE
        src/core/config.h
        src/core/input_bridge.h
        src/core/math.h
        src/ecs/ecs.c
        src/ecs/ecs.h
)

target_sources(arpg PRIVATE
        src/components/collider.c
        src/components/collider.h
        src/components/orientation_intent.c
        src/components/orientation_intent.h
        src/components/render.c
        src/components/render.h
        src/components/transform.c
        src/components/transform.h
        src/components/velocity.c
        src/components/velocity.h
        src/components/character.c
        src/components/character.h
        src/components/animated_render_3d.c
        src/components/animated_render_3d.h
        src/components/static_render_3d.c
        src/components/static_render_3d.h
        src/components/move_intent.c
        src/components/move_intent.h
        src/components/character_animation_map.c
        src/components/character_animation_map.h
        src/components/tags.c
        src/components/tags.h
        src/components/combat.c
        src/components/combat.h
        src/components/action_queue.c
        src/components/action_queue.h
        src/components/combat_effect.c
        src/components/combat_effect.h
)

target_sources(arpg PRIVATE
        src/systems/input_system.c
        src/systems/input_system.h
        src/systems/movement_system.c
        src/systems/movement_system.h
        src/systems/rotation_system.c
        src/systems/rotation_system.h
        src/systems/raylib/camera_system.c
        src/systems/raylib/camera_system.h
        src/systems/raylib/input_system.c
        src/systems/raylib/input_system.h
        src/systems/raylib/render_system.c
        src/systems/raylib/render_system.h
        src/systems/raylib/animation_system.c
        src/systems/raylib/animation_system.h
        src/systems/locomotion_system.c
        src/systems/locomotion_system.h
        src/systems/character_animation_system.c
        src/systems/character_animation_system.h
        src/systems/collision_system.c
        src/systems/collision_system.h
        src/systems/combat_system.c
        src/systems/combat_system.h
        src/systems/action_planner_system.c
        src/systems/action_planner_system.h
        src/systems/enemy_ai_system.c
        src/systems/enemy_ai_system.h
        src/systems/hit_box_system.c
        src/systems/hit_box_system.h
        src/systems/utility_system.c
        src/systems/utility_system.h
        src/systems/cooldown_system.c
        src/systems/cooldown_system.h
)

target_sources(arpg PRIVATE
        src/game/scene.c
        src/game/scene.h
        src/game/prefabs/brute_prefab.c
        src/game/prefabs/brute_prefab.h
        src/game/prefabs/mutant_prefab.c
        src/game/prefabs/mutant_prefab.h
        src/game/profiles/brute_skills_profile.c
        src/game/profiles/brute_skills_profile.h
        src/game/profiles/mutant_skills_profile.c
        src/game/profiles/mutant_skills_profile.h
)

target_sources(arpg PRIVATE
        src/asset/raylib/raylib_model_manager.c
        src/asset/raylib/raylib_model_manager.h
)

target_sources(arpg PRIVATE
        src/physics/raycast.c
        src/physics/raycast.h
)

if (MSVC)
    set(RAYLIB_SUBDIR "msvc")
    set(RAYLIB_LIB_NAME "raylib.lib")
    set(PLATFORM_LIBS winmm gdi32 user32 shell32)
elseif (MINGW)
    set(RAYLIB_SUBDIR "mingw")
    set(RAYLIB_LIB_NAME "libraylib.a")
    set(PLATFORM_LIBS winmm gdi32 user32 shell32)
elseif (APPLE)
    set(RAYLIB_SUBDIR "macos")
    set(RAYLIB_LIB_NAME "libraylib.a")
    set(PLATFORM_LIBS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
else ()
    message(FATAL_ERROR "Unsupported platform or compiler!")
endif ()

target_include_directories(arpg PRIVATE "${CMAKE_SOURCE_DIR}/external/raylib/${RAYLIB_SUBDIR}/include")

target_link_libraries(arpg PRIVATE
        "${CMAKE_SOURCE_DIR}/external/raylib/${RAYLIB_SUBDIR}/lib/${RAYLIB_LIB_NAME}"
        ${PLATFORM_LIBS}
)

add_custom_command(
        TARGET arpg POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:arpg>/assets"
        COMMENT "Copying assets directory to output folder..."
)