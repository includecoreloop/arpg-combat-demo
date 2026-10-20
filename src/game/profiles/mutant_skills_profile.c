#include "mutant_skills_profile.h"

#include "../../core/math.h"

#include "../../components/transform.h"
#include "../../components/collider.h"

static Entity hit_box_primary_collider_create() {
    const Entity entity = create_entity();

    const Transform3D t = {
        .position = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
        .scale = {.x = 1.0f, .y = 1.0f, .z = 1.0f},
        .rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f},
    };

    ADD_COMPONENT(transform, entity, t);

    const Collider3D col = {
        .type = COLLIDER_CIRCLE,
        .is_static = false,
        .is_trigger = true,
        .is_active = false,
        .circles = {
            [0] = {
                .offset = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
                .radius = 1.4f,
            },
        },
        .circle_count = 1,
        .trigger_contact_count = 0,
        .mass = 1.0f,
    };

    ADD_COMPONENT(collider, entity, col);

    return entity;
}

void mutant_combat_profile_apply(CombatIntent *combat_intent_comp) {
    combat_intent_comp->active_slot_b = COMBAT_SLOT_NONE;

    const Entity hit_box_primary = hit_box_primary_collider_create();

    const CombatAction a_primary = {
        .duration = 2.0f,
        .hit_start_time = 0.8f,
        .hit_end_time = 1.2f,
        .cooldown = 0.5f,
        .damage = 20,
        .attack_range = 1.2f,
        .hitbox_collider_entity = hit_box_primary,
    };

    combat_intent_comp->actions[COMBAT_SLOT_PRIMARY] = a_primary;

}
