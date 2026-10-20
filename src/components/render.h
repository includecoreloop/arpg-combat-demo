#pragma once

#include <stdint.h>
#include "../ecs/ecs.h"

typedef struct {
    uint32_t rgba; // e.g., 0xFF0000FF for red
} Render;

COMPONENT_DEFINE(Render, render);
