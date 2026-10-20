#pragma once

#include <stdint.h>
#include "../ecs/ecs.h"

typedef struct {
    uint32_t model_id;
} StaticRender3D;

COMPONENT_DEFINE(StaticRender3D, static_render_3d);
