#include "input_system.h"

#include "../ecs/ecs.h"
#include "../core/input_bridge.h"
#include "../physics/raycast.h"
#include "../components/combat.h"
#include "../components/move_intent.h"
#include "../components/tags.h"
#include "../components/transform.h"

static void set_move_intent_for_position(MoveIntent *mi, const Vec3 position) {
    mi->target_position = position;
    mi->has_target_entity = false;
    mi->target_entity = (Entity) -1;
    mi->has_intent = true;
}

static void process_primary_action(CombatIntent *ci, MoveIntent *mi) {
    const Entity target_entity = raycast_enemy_target(input_bridge.mouse_position);

    if (target_entity != (Entity) -1) {
        ci->target_entity = target_entity;
        ci->has_target_entity = true;

        mi->target_entity = target_entity;
        mi->has_target_entity = true;
        mi->has_intent = true;
    } else {
        Vec3 ground_hit;
        if (raycast_ground_plane(input_bridge.mouse_position, 0.0f, &ground_hit)) {
            combat_intent_clear_target_entity(ci);

            set_move_intent_for_position(mi, ground_hit);
        }
    }

    input_bridge_consume_action(&input_bridge.action_primary);
}

static void process_secondary_action(CombatIntent *ci, MoveIntent *mi) {
    const Entity target_entity = raycast_enemy_target(input_bridge.mouse_position);

    if (target_entity != (Entity) -1) {
        ci->target_entity = target_entity;
        ci->has_target_entity = true;

        const Transform3D *t = GET_COMPONENT(transform, target_entity);

        set_move_intent_for_position(mi, t->position);
        ci->intent_slot_b = COMBAT_SLOT_SECONDARY;
    }
    else {
        Vec3 ground_hit;
        if (raycast_ground_plane(input_bridge.mouse_position, 0.0f, &ground_hit)) {
            combat_intent_clear_target_entity(ci);
            set_move_intent_for_position(mi, ground_hit);
            ci->intent_slot_b = COMBAT_SLOT_SECONDARY;
        }
    }

    input_bridge_consume_action(&input_bridge.action_secondary);
}

void input_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(player_tag, i))
            continue;

        CombatIntent *ci = GET_COMPONENT(combat_intent, i);
        MoveIntent *mi = GET_COMPONENT(move_intent, i);

        if (input_bridge.action_primary) {
            process_primary_action(ci, mi);
        }

        if (input_bridge.action_secondary) {
            process_secondary_action(ci, mi);
        }

        if (input_bridge.action_q) {
            ci->intent_slot_nb = COMBAT_SLOT_Q;
            input_bridge_consume_action(&input_bridge.action_q);
        }

        if (input_bridge.action_w) {
            ci->intent_slot_b = COMBAT_SLOT_W;
            input_bridge_consume_action(&input_bridge.action_w);
        }

        if (input_bridge.action_e) {
            ci->intent_slot_nb = COMBAT_SLOT_E;
            input_bridge_consume_action(&input_bridge.action_e);
        }
    }
}
