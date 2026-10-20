#pragma once

#include "raylib.h"

extern Camera3D game_camera;

typedef enum {
    SCENE_STATE_MAIN_MENU,
    SCENE_STATE_GAMEPLAY,
} SceneState;

extern SceneState scene_state;

void scene_init(void);

void scene_update(void);

void scene_destroy(void);
