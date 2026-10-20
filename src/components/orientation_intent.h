#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"

typedef struct {
    Vec3 look_at_pos;
    Entity target_entity;
    bool has_target_entity;
} OrientationIntent;

COMPONENT_DEFINE(OrientationIntent, orientation_intent);
