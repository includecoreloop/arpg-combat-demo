#include "combat_system.h"

#include "../ecs/ecs.h"
#include "../components/character.h"
#include "../components/combat.h"
#include "../components/action_queue.h"
#include "../components/transform.h"
#include "../components/collider.h"

static void deactivate_hitbox(const Entity hb_entity) {
    Collider3D *hc = GET_COMPONENT(collider, hb_entity);
    hc->is_active = false;
}

static void process_attack(const float dt, CombatIntent *ci, BlockingActionQueue *baq, const Action *a, Character *c) {
    if (a->action_type != CHARACTER_ACTION_ATTACK) {
        return;
    }

    if (ci->active_slot_b == COMBAT_SLOT_NONE) {
        ci->active_slot_b = a->action_payload.combat_slot;

        CombatAction *ca = &ci->actions[ci->active_slot_b];
        ca->cast_timer = 0.0f;
        character_use_power_points(c, ca->power_cost);
    }

    CombatAction *active_attack = &ci->actions[ci->active_slot_b];
    active_attack->cast_timer += dt;

    if (active_attack->cast_timer >= active_attack->duration) {
        deactivate_hitbox(active_attack->hitbox_collider_entity);

        active_attack->cooldown_timer = active_attack->cooldown;
        active_attack->cast_timer = 0.0f;
        active_attack->hit_entity_count = 0;

        character_add_power_points(c, active_attack->power_gain);

        ci->active_slot_b = COMBAT_SLOT_NONE;

        blocking_action_queue_pop(baq);
    }
}

static void process_interruption(CombatIntent *ci, Action *a) {
    if (ci->active_slot_b != COMBAT_SLOT_NONE) {
        if (!a || a->action_type != CHARACTER_ACTION_ATTACK || a->action_payload.combat_slot != ci->active_slot_b) {
            CombatAction *interrupted_attack = &ci->actions[ci->active_slot_b];
            deactivate_hitbox(interrupted_attack->hitbox_collider_entity);
            interrupted_attack->cast_timer = 0.0f;
            ci->active_slot_b = COMBAT_SLOT_NONE;
        }
    }
}

static void update_hit_box_position(const Transform3D *ct, const CombatIntent *ci, const float dt) {
    if (ci->active_slot_b == COMBAT_SLOT_NONE) return;

    const Entity hit_box_entity = ci->actions[ci->active_slot_b].hitbox_collider_entity;

    Transform3D *ht = GET_COMPONENT(transform, hit_box_entity);
    Collider3D *hc = GET_COMPONENT(collider, hit_box_entity);

    const CombatAction *active_attack = &ci->actions[ci->active_slot_b];

    const Vec3 local_forward = {.x = 0.0f, .y = 0.0f, .z = active_attack->attack_range};

    const Vec3 rotated_offset = quat_rotate_vec3(ct->rotation, local_forward);

    ht->position.x = ct->position.x + rotated_offset.x;
    ht->position.y = ct->position.y + rotated_offset.y;
    ht->position.z = ct->position.z + rotated_offset.z;
    ht->rotation = ct->rotation;

    const bool in_active_window = (active_attack->cast_timer >= active_attack->hit_start_time) &&
                                  (active_attack->cast_timer <= active_attack->hit_end_time);

    const float prev_timer = active_attack->cast_timer - dt;
    const bool lag_swept_window = (prev_timer < active_attack->hit_end_time) &&
                                  (active_attack->cast_timer > active_attack->hit_start_time) &&
                                  (active_attack->hit_entity_count == 0);

    hc->is_active = in_active_window || lag_swept_window;
}

void combat_system_update(const float dt) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(character, i))
            continue;

        CombatIntent *ci = GET_COMPONENT(combat_intent, i);
        const Transform3D *ct = GET_COMPONENT(transform, i);
        Character *c = GET_COMPONENT(character, i);

        update_hit_box_position(ct, ci, dt);

        BlockingActionQueue *baq = GET_COMPONENT(blocking_action_queue, i);
        Action *a = blocking_action_queue_peek(baq);

        process_interruption(ci, a);

        if (!a) continue;

        process_attack(dt, ci, baq, a, c);
    }
}
