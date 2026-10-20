#include "raylib.h"

#include "../core/config.h"

#include "../systems/input_system.h"
#include "../systems/movement_system.h"
#include "../systems/rotation_system.h"
#include "../systems/locomotion_system.h"
#include "../systems/character_animation_system.h"
#include "../systems/collision_system.h"
#include "../systems/combat_system.h"
#include "../systems/action_planner_system.h"
#include "../systems/enemy_ai_system.h"
#include "../systems/hit_box_system.h"
#include "../systems/utility_system.h"
#include "../systems/cooldown_system.h"

#include "../systems/raylib/input_system.h"
#include "../systems/raylib/render_system.h"
#include "../systems/raylib/animation_system.h"
#include "../systems/raylib/camera_system.h"

#include "scene.h"

static void tick(const double dt) {
    input_system_update();

    cooldown_system_update((float) dt);
    action_planner_system_update();
    enemy_ai_system_update();

    locomotion_system_update();
    utility_system_update((float) dt);
    combat_system_update((float) dt);
    hit_box_system_update();

    rotation_system_update();
    movement_system_update(dt);
    collision_system_update();
    character_animation_system_update();
    camera_system_update(dt);
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    scene_init();

    double previous = GetTime();
    double accumulator = 0.0;

    while (!WindowShouldClose()) {
        const double current = GetTime();
        double frameTime = current - previous;

        if (frameTime > 0.25)
            frameTime = 0.25;

        previous = current;

        input_system_poll();

        scene_update();

        if (scene_state == SCENE_STATE_GAMEPLAY) {
            accumulator += frameTime;

            while (accumulator >= FIXED_DT) {
                tick(FIXED_DT);
                accumulator -= FIXED_DT;
            }

            animation_system_update((float) frameTime);
        } else {
            accumulator = 0.0;
        }

        render_system_update();
    }

    CloseWindow();

    return 0;
}
