#include "raylib_model_manager.h"
#include <string.h>

#define MAX_LOADED_MODELS 16
#define MAX_LOADED_ANIMATIONS 16

typedef struct {
    ModelAnimation *animation;
    uint32_t animation_count;
    char path[128];
} RLAnimationSet;

static Model rl_model_pool[MAX_LOADED_MODELS];
static char rl_path_pool[MAX_LOADED_MODELS][128];
static uint32_t rl_model_count = 0;

static RLAnimationSet rl_animation_pool[MAX_LOADED_ANIMATIONS];
static uint32_t rl_animation_count = 0;

void raylib_model_manager_init(void) {
    rl_model_count = 0;
    rl_animation_count = 0;
}

uint32_t raylib_model_manager_load(const char *file_path) {
    for (uint32_t i = 0; i < rl_model_count; i++) {
        if (strcmp(rl_path_pool[i], file_path) == 0) {
            return i;
        }
    }

    const uint32_t id = rl_model_count;
    rl_model_pool[id] = LoadModel(file_path);
    strcpy(rl_path_pool[id], file_path);
    rl_model_count++;

    return id;
}

Model *raylib_model_manager_get(const uint32_t id) {
    return &rl_model_pool[id];
}

uint32_t raylib_model_manager_load_animation(const char *file_path) {
    for (uint32_t i = 0; i < rl_animation_count; i++) {
        if (strcmp(rl_animation_pool[i].path, file_path) == 0) {
            return i;
        }
    }

    int count = 0;
    ModelAnimation *animation = LoadModelAnimations(file_path, &count);

    const uint32_t id = rl_animation_count;
    rl_animation_pool[id].animation = animation;
    rl_animation_pool[id].animation_count = (uint32_t) count;
    strcpy(rl_animation_pool[id].path, file_path);
    rl_animation_count++;

    return id;
}

ModelAnimation *raylib_model_manager_get_animation(const uint32_t animation_set_id) {
    return rl_animation_pool[animation_set_id].animation;
}

void raylib_model_manager_unload_all(void) {
    for (uint32_t i = 0; i < rl_model_count; i++) {
        UnloadModel(rl_model_pool[i]);
    }

    for (uint32_t i = 0; i < rl_animation_count; i++) {
        UnloadModelAnimations(rl_animation_pool[i].animation, (int) rl_animation_pool[i].animation_count);
    }

    rl_model_count = 0;
    rl_animation_count = 0;
}

uint32_t raylib_model_manager_get_animation_frame_count(const uint32_t animation_set_id) {
    return (uint32_t)rl_animation_pool[animation_set_id].animation[0].keyframeCount;
}
