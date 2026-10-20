#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"

typedef struct {
    Vec3 position;
    Vec3 scale;
    Quat rotation;
} Transform3D;

COMPONENT_DEFINE(Transform3D, transform);
