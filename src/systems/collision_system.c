#include "collision_system.h"

#include <math.h>
#include "../core/math.h"
#include "../components/transform.h"
#include "../components/collider.h"

static Vec3 get_circle_world_center(const Transform3D *t, const CircleShape *circle) {
    if (circle->offset.x == 0.0f && circle->offset.z == 0.0f) {
        return t->position;
    }

    const float rad = quat_get_yaw_rad(t->rotation);
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vec3){
        .x = t->position.x + (circle->offset.x * cos_a - circle->offset.z * sin_a),
        .y = t->position.y,
        .z = t->position.z + (circle->offset.x * sin_a + circle->offset.z * cos_a)
    };
}

static Vec3 get_box_world_center(const Transform3D *t, const Collider3D *c) {
    if (c->offset.x == 0.0f && c->offset.z == 0.0f) {
        return t->position;
    }

    const float rad = quat_get_yaw_rad(t->rotation);
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vec3){
        .x = t->position.x + (c->offset.x * cos_a - c->offset.z * sin_a),
        .y = t->position.y,
        .z = t->position.z + (c->offset.x * sin_a + c->offset.z * cos_a)
    };
}

static bool is_circle_vs_box_overlapping(const Transform3D *circle_t, const CircleShape *circle,
                                         const Transform3D *box_t, const Collider3D *box_col) {
    const Vec3 circle_center = get_circle_world_center(circle_t, circle);
    const Vec3 box_center = get_box_world_center(box_t, box_col);

    const Vec3 box_min = {
        .x = box_center.x - box_col->half_extents.x, .y = 0.0f, .z = box_center.z - box_col->half_extents.z
    };
    const Vec3 box_max = {
        .x = box_center.x + box_col->half_extents.x, .y = 0.0f, .z = box_center.z + box_col->half_extents.z
    };

    const float closest_x = math_clamp(circle_center.x, box_min.x, box_max.x);
    const float closest_z = math_clamp(circle_center.z, box_min.z, box_max.z);

    const float diff_x = circle_center.x - closest_x;
    const float diff_z = circle_center.z - closest_z;

    return (diff_x * diff_x + diff_z * diff_z) < (circle->radius * circle->radius);
}

static bool is_circle_vs_circle_overlapping(const Transform3D *t_a, const CircleShape *circle_a,
                                            const Transform3D *t_b, const CircleShape *circle_b) {
    const Vec3 center_a = get_circle_world_center(t_a, circle_a);
    const Vec3 center_b = get_circle_world_center(t_b, circle_b);

    const float diff_x = center_a.x - center_b.x;
    const float diff_z = center_a.z - center_b.z;

    const float sum_radii = circle_a->radius + circle_b->radius;

    return (diff_x * diff_x + diff_z * diff_z) < (sum_radii * sum_radii);
}

static bool is_overlapping(const Transform3D *t_a, const Collider3D *c_a,
                           const Transform3D *t_b, const Collider3D *c_b) {
    if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_BOX) {
        for (int k = 0; k < c_a->circle_count; k++) {
            if (is_circle_vs_box_overlapping(t_a, &c_a->circles[k], t_b, c_b)) return true;
        }
    } else if (c_a->type == COLLIDER_BOX && c_b->type == COLLIDER_CIRCLE) {
        for (int k = 0; k < c_b->circle_count; k++) {
            if (is_circle_vs_box_overlapping(t_b, &c_b->circles[k], t_a, c_a)) return true;
        }
    } else if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_CIRCLE) {
        for (int ka = 0; ka < c_a->circle_count; ka++) {
            for (int kb = 0; kb < c_b->circle_count; kb++) {
                if (is_circle_vs_circle_overlapping(t_a, &c_a->circles[ka], t_b, &c_b->circles[kb])) return true;
            }
        }
    }

    return false;
}

static void apply_position_displacement(Transform3D *t_a, Transform3D *t_b,
                                        const Collider3D *col_a, const Collider3D *col_b,
                                        const Vec3 normal, const float penetration) {
    if (penetration <= 0.0f) return;

    if (col_b->is_static) {
        t_a->position.x += normal.x * penetration;
        t_a->position.z += normal.z * penetration;
        return;
    }

    if (col_a->is_static) {
        t_b->position.x -= normal.x * penetration;
        t_b->position.z -= normal.z * penetration;
        return;
    }

    const float mass_a = col_a->mass > 0.001f ? col_a->mass : 1.0f;
    const float mass_b = col_b->mass > 0.001f ? col_b->mass : 1.0f;
    const float total_mass = mass_a + mass_b;

    const float ratio_a = mass_b / total_mass;
    const float ratio_b = mass_a / total_mass;

    t_a->position.x += normal.x * (penetration * ratio_a);
    t_a->position.z += normal.z * (penetration * ratio_a);

    t_b->position.x -= normal.x * (penetration * ratio_b);
    t_b->position.z -= normal.z * (penetration * ratio_b);
}

static void resolve_deep_circle_in_box(Transform3D *circle_t, const Collider3D *circle_col, const CircleShape *circle,
                                       const Vec3 circle_center, const Vec3 box_min, const Vec3 box_max,
                                       Transform3D *box_t, const Collider3D *box_col) {
    const float dist_left = circle_center.x - box_min.x;
    const float dist_right = box_max.x - circle_center.x;
    const float dist_top = circle_center.z - box_min.z;
    const float dist_bottom = box_max.z - circle_center.z;

    float min_dist = dist_left;
    Vec3 normal = (Vec3){.x = -1.0f, .y = 0.0f, .z = 0.0f};

    if (dist_right < min_dist) {
        min_dist = dist_right;
        normal = (Vec3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
    }
    if (dist_top < min_dist) {
        min_dist = dist_top;
        normal = (Vec3){.x = 0.0f, .y = 0.0f, .z = -1.0f};
    }
    if (dist_bottom < min_dist) {
        min_dist = dist_bottom;
        normal = (Vec3){.x = 0.0f, .y = 0.0f, .z = 1.0f};
    }

    apply_position_displacement(circle_t, box_t, circle_col, box_col, normal, min_dist + circle->radius);
}

static void resolve_single_circle_vs_circle(Transform3D *t_a, const Collider3D *c_a, const CircleShape *circle_a,
                                            Transform3D *t_b, const Collider3D *c_b, const CircleShape *circle_b) {
    const Vec3 center_a = get_circle_world_center(t_a, circle_a);
    const Vec3 center_b = get_circle_world_center(t_b, circle_b);

    const Vec3 diff = {.x = center_a.x - center_b.x, .y = 0.0f, .z = center_a.z - center_b.z};
    const float distance = sqrtf((diff.x * diff.x) + (diff.z * diff.z));

    if (distance == 0.0f) return;

    const float sum_radii = circle_a->radius + circle_b->radius;
    if (distance >= sum_radii) return;

    const float penetration = sum_radii - distance;
    const Vec3 normal = {.x = diff.x / distance, .y = 0.0f, .z = diff.z / distance};

    apply_position_displacement(t_a, t_b, c_a, c_b, normal, penetration);
}

static void resolve_single_circle_vs_box(Transform3D *circle_t, const Collider3D *circle_col, const CircleShape *circle,
                                         Transform3D *box_t, const Collider3D *box_col) {
    const Vec3 circle_center = get_circle_world_center(circle_t, circle);
    const Vec3 box_center = get_box_world_center(box_t, box_col);

    const Vec3 box_min = {
        .x = box_center.x - box_col->half_extents.x, .y = 0.0f, .z = box_center.z - box_col->half_extents.z
    };
    const Vec3 box_max = {
        .x = box_center.x + box_col->half_extents.x, .y = 0.0f, .z = box_center.z + box_col->half_extents.z
    };

    const Vec3 closest = {
        .x = math_clamp(circle_center.x, box_min.x, box_max.x),
        .y = 0.0f,
        .z = math_clamp(circle_center.z, box_min.z, box_max.z)
    };

    const Vec3 diff = {.x = circle_center.x - closest.x, .y = 0.0f, .z = circle_center.z - closest.z};
    const float distance_sq = (diff.x * diff.x) + (diff.z * diff.z);

    if (distance_sq == 0.0f) {
        resolve_deep_circle_in_box(circle_t, circle_col, circle, circle_center, box_min, box_max, box_t, box_col);
        return;
    }

    const float distance = sqrtf(distance_sq);
    if (distance >= circle->radius) return;

    const float penetration = circle->radius - distance;
    const Vec3 normal = {.x = diff.x / distance, .y = 0.0f, .z = diff.z / distance};

    apply_position_displacement(circle_t, box_t, circle_col, box_col, normal, penetration);
}

static void reset_all_trigger_contacts(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(collider, i)) continue;
        Collider3D *c = GET_COMPONENT(collider, i);
        c->trigger_contact_count = 0;
    }
}

static bool can_collide(const Collider3D *c_a, const Collider3D *c_b) {
    if (!c_a->is_active || !c_b->is_active) return false;

    if (c_a->is_trigger && c_b->is_trigger) return false;

    if (c_a->is_static && c_b->is_static) return false;

    return true;
}

static bool resolve_trigger_overlap(const Entity entity_a, const Transform3D *t_a, Collider3D *c_a,
                                    const Entity entity_b, const Transform3D *t_b, Collider3D *c_b) {
    if (!c_a->is_trigger && !c_b->is_trigger) return false;

    if (is_overlapping(t_a, c_a, t_b, c_b)) {
        collider_add_trigger_contact(c_a, entity_b);
        collider_add_trigger_contact(c_b, entity_a);
    }

    return true;
}

static void resolve_physics_collision(Transform3D *t_a, const Collider3D *c_a,
                                      Transform3D *t_b, const Collider3D *c_b) {
    if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_BOX) {
        for (int k = 0; k < c_a->circle_count; k++) {
            resolve_single_circle_vs_box(t_a, c_a, &c_a->circles[k], t_b, c_b);
        }
    } else if (c_a->type == COLLIDER_BOX && c_b->type == COLLIDER_CIRCLE) {
        for (int k = 0; k < c_b->circle_count; k++) {
            resolve_single_circle_vs_box(t_b, c_b, &c_b->circles[k], t_a, c_a);
        }
    } else if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_CIRCLE) {
        for (int ka = 0; ka < c_a->circle_count; ka++) {
            for (int kb = 0; kb < c_b->circle_count; kb++) {
                resolve_single_circle_vs_circle(t_a, c_a, &c_a->circles[ka], t_b, c_b, &c_b->circles[kb]);
            }
        }
    }
}

static void process_entity_pair(const Entity i, const Entity j) {
    if (!HAS_COMPONENT(transform, i) || !HAS_COMPONENT(collider, i)) return;
    if (!HAS_COMPONENT(transform, j) || !HAS_COMPONENT(collider, j)) return;

    Transform3D *t_a = GET_COMPONENT(transform, i);
    Collider3D *c_a = GET_COMPONENT(collider, i);

    Transform3D *t_b = GET_COMPONENT(transform, j);
    Collider3D *c_b = GET_COMPONENT(collider, j);

    if (!can_collide(c_a, c_b)) return;

    if (resolve_trigger_overlap(i, t_a, c_a, j, t_b, c_b)) return;

    resolve_physics_collision(t_a, c_a, t_b, c_b);
}

void collision_system_update(void) {
    reset_all_trigger_contacts();

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        for (Entity j = i + 1; j < MAX_ENTITIES; j++) {
            process_entity_pair(i, j);
        }
    }
}
