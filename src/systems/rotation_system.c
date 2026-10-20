#include "rotation_system.h"

#include <math.h>
#include "../ecs/ecs.h"
#include "../components/transform.h"
#include "../components/orientation_intent.h"
#include "../components/tags.h"

void rotation_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(transform, i) || !HAS_COMPONENT(orientation_intent, i)) continue;

        Transform3D *t = GET_COMPONENT(transform, i);
        OrientationIntent *oi = GET_COMPONENT(orientation_intent, i);

        Vec3 target_pos = oi->look_at_pos;

        const bool is_player = HAS_COMPONENT(player_tag, i);
        const bool is_enemy = HAS_COMPONENT(enemy_tag, i);

        if (oi->has_target_entity) {
            if (is_player && HAS_COMPONENT(enemy_tag, oi->target_entity)) {
                const Transform3D *target_t = GET_COMPONENT(transform, oi->target_entity);
                target_pos = target_t->position;
            } else if (is_enemy && HAS_COMPONENT(player_tag, oi->target_entity)) {
                const Transform3D *target_t = GET_COMPONENT(transform, oi->target_entity);
                target_pos = target_t->position;
            } else {
                oi->has_target_entity = false;
                oi->target_entity = (Entity) -1;
            }
        }

        const float diff_x = target_pos.x - t->position.x;
        const float diff_z = target_pos.z - t->position.z;
        const float dist_sq = diff_x * diff_x + diff_z * diff_z;

        if (dist_sq <= 0.000001f) continue;

        const float length = sqrtf(dist_sq);
        const float dir_x = diff_x / length;
        const float dir_z = diff_z / length;

        const float yaw_angle = atan2f(dir_x, dir_z);
        const float half_angle = yaw_angle * 0.5f;

        t->rotation.x = 0.0f;
        t->rotation.y = sinf(half_angle);
        t->rotation.z = 0.0f;
        t->rotation.w = cosf(half_angle);
    }
}
