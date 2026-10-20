#include "scene.h"

#include "../core/math.h"
#include "../core/config.h"

#include "../ecs/ecs.h"

#include "../components/tags.h"
#include "../core/input_bridge.h"

#include "prefabs/brute_prefab.h"
#include "prefabs/mutant_prefab.h"

Camera3D game_camera = {0};

SceneState scene_state;

void scene_init(void) {
    game_camera.position = CAMERA_OFFSET;
    game_camera.target = (Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    game_camera.up = (Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
    game_camera.fovy = 30.0f;
    game_camera.projection = CAMERA_PERSPECTIVE;

    create_entity();

    brute_prefab_create(true);
    mutant_prefab_create((Vec3){.x = 10.0f, .y = 0.0f, .z = 10.0f}, false);

    scene_state = SCENE_STATE_MAIN_MENU;
}

static void scene_reset(void) {
    input_bridge_consume_all(&input_bridge);

    scene_destroy();
    scene_init();
}

void scene_update(void) {
    if (input_bridge.action_p) {
        scene_state = SCENE_STATE_GAMEPLAY;
        input_bridge_consume_all(&input_bridge);
    }

    if (input_bridge.action_r && scene_state == SCENE_STATE_GAMEPLAY) {
        scene_reset();
    }

    if (scene_state == SCENE_STATE_GAMEPLAY) {
        int enemies = 0;
        int players = 0;
        for (int i = 0; i < MAX_ENTITIES; ++i) {
            if (HAS_COMPONENT(enemy_tag, i)) {
                enemies++;
            }

            if (HAS_COMPONENT(player_tag, i)) {
                players++;
            }
        }

        if (players == 0) {
            scene_reset();
            return;
        }

        if (enemies == 0) {
            mutant_prefab_create((Vec3){.x = 10.0f, .y = 0.0f, .z = 10.0f}, false);
            mutant_prefab_create((Vec3){.x = 10.0f, .y = 0.0f, .z = -10.0f}, false);
        }
    }
}

void scene_destroy(void) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(player_tag, i))
            brute_prefab_destroy(i);
    }

    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(enemy_tag, i))
            mutant_prefab_destroy(i);
    }

    game_camera = (Camera3D){0};
}
