#include "locomotion_system.h"

#include "../ecs/ecs.h"
#include "../core/math.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/move_intent.h"
#include "../components/orientation_intent.h"
#include "../components/character.h"
#include "../components/tags.h"
#include "../components/collider.h"
#include "../components/action_queue.h"

static bool is_related_action(Action *a) {
    if (!a) return false;

    return a->action_type == CHARACTER_ACTION_MOVE_TO_POSITION ||
           a->action_type == CHARACTER_ACTION_MOVE_TO_TARGET ||
           a->action_type == CHARACTER_ACTION_ROTATE_TO_POSITION;
}

static bool target_is_unavailable(BlockingActionQueue *baq, Velocity *v, const MoveIntent *mi, const bool is_player,
                                  const bool is_enemy) {
    const bool enemy_unavailable = is_player && !HAS_COMPONENT(enemy_tag, mi->target_entity);
    const bool player_unavailable = is_enemy && !HAS_COMPONENT(player_tag, mi->target_entity);

    if (enemy_unavailable || player_unavailable) {
        blocking_action_queue_pop(baq);
        v->direction = (Vec3){0};
        v->speed = 0.0f;

        return true;
    }

    return false;
}


static void set_orientation_to_position(const Entity i, const Vec3 target_pos) {
    OrientationIntent *oi = GET_COMPONENT(orientation_intent, i);
    oi->look_at_pos = target_pos;
    oi->target_entity = (Entity) -1;
    oi->has_target_entity = false;
}

static void set_orientation_to_target(const Entity i, const MoveIntent *mi) {
    OrientationIntent *oi = GET_COMPONENT(orientation_intent, i);
    oi->target_entity = mi->target_entity;
    oi->has_target_entity = true;
}

static void stop_movement(Velocity *v) {
    v->direction = (Vec3){0};
    v->speed = 0.0f;
}

static void process_locomotion(BlockingActionQueue *baq, Velocity *v, const Vec3 target_pos, const float stop_dist,
                               const Transform3D *t, const Character *c) {
    const Vec3 to_target = {
        .x = target_pos.x - t->position.x,
        .y = 0.0f,
        .z = target_pos.z - t->position.z
    };

    const float distance = vec3_length(to_target);

    if (distance <= stop_dist) {
        stop_movement(v);
        blocking_action_queue_pop(baq);
    } else {
        v->direction = vec3_normalize(to_target);
        v->speed = c->movement_speed;
    }
}

static void process_rotate_to_position(const Entity i, Velocity *v, MoveIntent *mi) {
    set_orientation_to_position(i, mi->target_position);
    stop_movement(v);
    mi->has_intent = false;
}

void locomotion_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(character, i))
            continue;

        BlockingActionQueue *baq = GET_COMPONENT(blocking_action_queue, i);
        Action *a = blocking_action_queue_peek(baq);

        if (!is_related_action(a)) {
            continue;
        }

        Velocity *v = GET_COMPONENT(velocity, i);
        MoveIntent *mi = GET_COMPONENT(move_intent, i);

        if (a->action_type == CHARACTER_ACTION_ROTATE_TO_POSITION) {
            process_rotate_to_position(i, v, mi);
            blocking_action_queue_pop(baq);
            continue;
        }

        const bool is_player = HAS_COMPONENT(player_tag, i);
        const bool is_enemy = HAS_COMPONENT(enemy_tag, i);

        Vec3 target_pos = {0};
        float stop_dist = 0.0f;

        if (a->action_type == CHARACTER_ACTION_MOVE_TO_POSITION) {
            target_pos = mi->target_position;
            stop_dist = mi->stop_distance;

            set_orientation_to_position(i, target_pos);
        } else if (a->action_type == CHARACTER_ACTION_MOVE_TO_TARGET) {
            if (target_is_unavailable(baq, v, mi, is_player, is_enemy)) continue;

            const Transform3D *target_t = GET_COMPONENT(transform, mi->target_entity);
            target_pos = target_t->position;

            const Collider3D *character_col = GET_COMPONENT(collider, i);
            const Collider3D *target_col = GET_COMPONENT(collider, mi->target_entity);
            stop_dist = collider_get_base_circle_radius(target_col) +
                        collider_get_base_circle_radius(character_col) + 0.4f;

            set_orientation_to_target(i, mi);
        }

        const Transform3D *t = GET_COMPONENT(transform, i);
        const Character *c = GET_COMPONENT(character, i);

        process_locomotion(baq, v, target_pos, stop_dist, t, c);
    }
}
