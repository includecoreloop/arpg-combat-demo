#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"

typedef struct {
    Vec3 target_position;
    float stop_distance;
    bool has_intent;

    Entity target_entity;
    bool has_target_entity;
} MoveIntent;

COMPONENT_DEFINE(MoveIntent, move_intent);
