#pragma once

#include <stdint.h>
#include "../ecs/ecs.h"

typedef struct {
    uint32_t model_id;
    uint32_t animation_set_id;
    uint32_t current_animation_in_set;
    float animation_frame_counter;
    float animation_speed;
    float default_animation_speed;
    bool loop;
    bool is_playing;
} AnimatedRender3D;

COMPONENT_DEFINE(AnimatedRender3D, animated_render_3d);
