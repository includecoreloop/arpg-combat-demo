#pragma once

#include "../core/math.h"
#include "../ecs/ecs.h"

typedef struct {
    Vec3 direction;
    float speed;
} Velocity;

COMPONENT_DEFINE(Velocity, velocity);

static inline bool velocity_is_moving(const Velocity *v) {
    const float speed_sq = (v->direction.x * v->direction.x) + (v->direction.z * v->direction.z);

    return speed_sq > 0.0001f && v->speed > 0.0f;
}
