#pragma once

#define SCREEN_WIDTH        1920
#define SCREEN_HEIGHT       1200

#define WINDOW_TITLE        "ARPG 3D"
#define TARGET_FPS          120

#define TARGET_TPS          120.0
#define FIXED_DT            (1.0 / TARGET_TPS)

#define RENDER_COLLIDERS    1
#define RENDER_FPS_COUNTER  1

#define MAX_COLLIDER_CIRCLES 5
#define MAX_TRIGGER_CONTACTS 8

#define DEFAULT_ANIMATION_FPS 60.0

#define CAMERA_OFFSET (Vector3){ .x = 0.0f, .y = 22.6f, .z = 17.0f }

#define ACTION_QUEUE_MAX_ACTIONS 10

#define ENABLE_ENEMY_AI 1
