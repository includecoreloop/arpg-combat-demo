#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"
#include "../core/config.h"

typedef enum {
    COLLIDER_CIRCLE,
    COLLIDER_BOX
} ColliderType;

typedef struct {
    Vec3 offset;
    float radius;
} CircleShape;

typedef struct {
    ColliderType type;
    bool is_static;
    bool is_trigger;

    Vec3 offset;
    Vec3 half_extents;

    CircleShape circles[MAX_COLLIDER_CIRCLES];
    int circle_count;

    bool is_active;

    Entity triggered_entities[MAX_TRIGGER_CONTACTS];
    int trigger_contact_count;

    float mass;
} Collider3D;

COMPONENT_DEFINE(Collider3D, collider);

static inline void collider_add_trigger_contact(Collider3D *col, const Entity other_entity) {
    if (col->trigger_contact_count < MAX_TRIGGER_CONTACTS) {
        col->triggered_entities[col->trigger_contact_count] = other_entity;
        col->trigger_contact_count++;
    }
}

static inline float collider_get_base_circle_radius(const Collider3D *col) {
    return col->circles[0].radius;
}
