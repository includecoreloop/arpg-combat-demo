#include "brute_skills_profile.h"

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
                .radius = 0.8f,
            },
        },
        .circle_count = 1,
        .trigger_contact_count = 0,
        .mass = 1.0f,
    };

    ADD_COMPONENT(collider, entity, col);

    return entity;
}

static Entity hit_box_secondary_collider_create() {
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
                .radius = 1.5f,
            },
        },
        .circle_count = 1,
        .trigger_contact_count = 0,
        .mass = 1.0f,
    };

    ADD_COMPONENT(collider, entity, col);

    return entity;
}

static Entity hit_box_w_collider_create() {
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
                .radius = 4.0f,
            },
        },
        .circle_count = 1,
        .trigger_contact_count = 0,
        .mass = 1.0f,
    };

    ADD_COMPONENT(collider, entity, col);

    return entity;
}

void brute_combat_profile_apply(CombatIntent *combat_intent_comp) {
    combat_intent_comp->active_slot_b = COMBAT_SLOT_NONE;
    combat_intent_comp->active_slot_nb = COMBAT_SLOT_NONE;

    const Entity hit_box_primary = hit_box_primary_collider_create();

    const CombatAction a_primary = {
        .power_gain = 20,
        .duration = 0.6f,
        .hit_start_time = 0.2f,
        .hit_end_time = 0.5f,
        .damage = 20,
        .attack_range = 0.5f,
        .hitbox_collider_entity = hit_box_primary,
    };

    combat_intent_comp->actions[COMBAT_SLOT_PRIMARY] = a_primary;

    //--------------------

    const Entity hit_box_secondary = hit_box_secondary_collider_create();

    const CombatAction a_secondary = {
        .power_cost = 20,
        .duration = 1.0f,
        .hit_start_time = 0.2f,
        .hit_end_time = 0.7f,
        .damage = 30,
        .attack_range = 0.5f,
        .hitbox_collider_entity = hit_box_secondary,
    };

    combat_intent_comp->actions[COMBAT_SLOT_SECONDARY] = a_secondary;

    //--------------------

    const CombatAction a_q = {
        .health_points_gain = 40,
        .cooldown = 8.0f,
    };

    combat_intent_comp->actions[COMBAT_SLOT_Q] = a_q;

    //--------------------

    const Entity hit_box_w = hit_box_w_collider_create();

    const CombatAction a_w = {
        .power_cost = 40,
        .duration = 1.0f,
        .hit_start_time = 0.2f,
        .hit_end_time = 0.6f,
        .cooldown = 12.0f,
        .damage = 30,
        .stun_duration = 5.0f,
        .hitbox_collider_entity = hit_box_w,
    };

    combat_intent_comp->actions[COMBAT_SLOT_W] = a_w;

    //--------------------

    const CombatAction a_e = {
        .power_cost = 40,
        .damage_amplifier = 1.6f,
        .damage_amplifier_duration = 6.0f,
        .cooldown = 10.0f,
    };

    combat_intent_comp->actions[COMBAT_SLOT_E] = a_e;
}
