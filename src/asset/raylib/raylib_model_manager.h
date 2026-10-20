#pragma once

#include <stdint.h>
#include "raylib.h"

void raylib_model_manager_init(void);

uint32_t raylib_model_manager_load(const char *file_path);

Model *raylib_model_manager_get(uint32_t id);

void raylib_model_manager_unload_all(void);

uint32_t raylib_model_manager_load_animation(const char *file_path);

ModelAnimation *raylib_model_manager_get_animation(uint32_t animation_set_id);

uint32_t raylib_model_manager_get_animation_frame_count(uint32_t animation_set_id);
