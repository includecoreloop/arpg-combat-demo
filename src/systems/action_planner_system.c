#include "action_planner_system.h"

#include "../ecs/ecs.h"
#include "../components/action_queue.h"
#include "../components/animated_render_3d.h"
#include "../components/character.h"
#include "../components/move_intent.h"
#include "../components/combat.h"
#include "../components/combat_effect.h"
#include "../components/tags.h"
#include "../components/transform.h"
#include "../components/collider.h"
#include "../game/prefabs/brute_prefab.h"
#include "../game/prefabs/mutant_prefab.h"

static void try_push_attack_action(BlockingActionQueue *baq, const Character *c, const CombatIntent *ci,
                                   const CombatSlot slot) {
    const CombatAction *action = &ci->actions[slot];

    if (action->power_cost > 0 && !character_has_enough_power_points(c, action->power_cost)) {
        return;
    }

    if (action->cooldown_timer <= 0.0f) {
        const Action attack_action = {
            .action_type = CHARACTER_ACTION_ATTACK,
            .action_payload = {.combat_slot = slot}
        };

        blocking_action_queue_push(baq, attack_action);
    }
}

static void process_combat_slot_q(CombatIntent *ci, const Character *c, NonBlockingActionQueue *nbaq) {
    if (ci->intent_slot_nb == COMBAT_SLOT_Q) {
        const CombatAction *action = &ci->actions[COMBAT_SLOT_Q];

        if (action->power_cost > 0 && !character_has_enough_power_points(c, action->power_cost)) {
            return;
        }

        if (character_has_full_health_points(c)) {
            return;
        }

        if (action->cooldown_timer <= 0.0f) {
            const Action utility_action = {
                .action_type = CHARACTER_ACTION_USE_UTILITY,
                .action_payload = {.combat_slot = COMBAT_SLOT_Q}
            };

            non_blocking_action_queue_push(nbaq, utility_action);
        }

        ci->intent_slot_nb = COMBAT_SLOT_NONE;
    }
}

static void process_combat_slot_e(CombatIntent *ci, const Character *c, NonBlockingActionQueue *nbaq) {
    if (ci->intent_slot_nb == COMBAT_SLOT_E) {
        const CombatAction *action = &ci->actions[COMBAT_SLOT_E];

        if (action->power_cost > 0 && !character_has_enough_power_points(c, action->power_cost)) {
            return;
        }

        if (action->cooldown_timer <= 0.0f) {
            const Action utility_action = {
                .action_type = CHARACTER_ACTION_USE_UTILITY,
                .action_payload = {.combat_slot = COMBAT_SLOT_E}
            };

            non_blocking_action_queue_push(nbaq, utility_action);
        }

        ci->intent_slot_nb = COMBAT_SLOT_NONE;
    }
}

static void process_move_intent(MoveIntent *mi, CombatIntent *ci, BlockingActionQueue *baq, const Character *c) {
    if (mi->has_intent) {
        blocking_action_queue_clear(baq);

        if (mi->has_target_entity) {
            const Action move_target_action = {.action_type = CHARACTER_ACTION_MOVE_TO_TARGET};
            blocking_action_queue_push(baq, move_target_action);
            try_push_attack_action(baq, c, ci, COMBAT_SLOT_PRIMARY);
        } else {
            const Action move_action = {.action_type = CHARACTER_ACTION_MOVE_TO_POSITION};
            blocking_action_queue_push(baq, move_action);
        }

        mi->has_intent = false;
        ci->intent_slot_b = COMBAT_SLOT_NONE;
    }
}

static bool process_combat_action_secondary(CombatIntent *ci, BlockingActionQueue *baq, const Character *c) {
    if (ci->intent_slot_b == COMBAT_SLOT_SECONDARY) {
        blocking_action_queue_clear(baq);
        blocking_action_queue_push(baq, (Action){.action_type = CHARACTER_ACTION_ROTATE_TO_POSITION});
        try_push_attack_action(baq, c, ci, COMBAT_SLOT_SECONDARY);
        ci->intent_slot_b = COMBAT_SLOT_NONE;

        return true;
    }

    return false;
}

static bool process_combat_action_w(CombatIntent *ci, BlockingActionQueue *baq, const Character *c) {
    if (ci->intent_slot_b == COMBAT_SLOT_W) {
        blocking_action_queue_clear(baq);
        blocking_action_queue_push(baq, (Action){.action_type = CHARACTER_ACTION_ROTATE_TO_POSITION});
        try_push_attack_action(baq, c, ci, COMBAT_SLOT_W);
        ci->intent_slot_b = COMBAT_SLOT_NONE;

        return true;
    }

    return false;
}

static bool process_stun(BlockingActionQueue *baq, Character *c, const Transform3D *t, CombatEffect *ce,
                         MoveIntent *mi) {
    if (ce->stun_intent) {
        ce->stunned = true;
        ce->stun_intent = false;

        const Vec3 target_pos = {.x = t->position.x, .y = t->position.y + c->target_height, .z = t->position.z};
        character_floating_text_reset(c);
        character_floating_text_set(c, target_pos, "STUNNED");

        mi->has_intent = true;
        mi->target_position = t->position;

        blocking_action_queue_clear(baq);
        const Action move_action = {.action_type = CHARACTER_ACTION_MOVE_TO_POSITION};
        blocking_action_queue_push(baq, move_action);

        return true;
    }

    if (ce->stunned) {
        return true;
    }

    return false;
}

static bool process_death(BlockingActionQueue *baq, Character *c, const Transform3D *t, CombatEffect *ce,
                          MoveIntent *mi, const Entity entity) {
    if (c->health_points <= 0) {
        const Action *a = blocking_action_queue_peek(baq);
        if (a && a->action_type == CHARACTER_ACTION_DIE) {
            const AnimatedRender3D *render = GET_COMPONENT(animated_render_3d, entity);

            // FIXME when using !render->is_playing, something forces first animation frame.
            if (render->animation_frame_counter > 250.0f) {
                blocking_action_queue_clear(baq);

                if (HAS_COMPONENT(mutant_class_tag, entity)) {
                    mutant_prefab_destroy(entity);
                }

                if (HAS_COMPONENT(brute_class_tag, entity)) {
                    brute_prefab_destroy(entity);
                }
            }

            return true;
        }

        *ce = (CombatEffect){0};

        const Vec3 target_pos = {.x = t->position.x, .y = t->position.y + c->target_height, .z = t->position.z};
        character_floating_text_reset(c);
        character_floating_text_set(c, target_pos, "AAAGGHHHH !!");

        mi->has_intent = true;
        mi->target_position = t->position;

        blocking_action_queue_clear(baq);
        const Action move_action = {.action_type = CHARACTER_ACTION_MOVE_TO_POSITION};
        const Action death_action = {.action_type = CHARACTER_ACTION_DIE};

        REMOVE_COMPONENT(collider, entity);

        blocking_action_queue_push(baq, move_action);
        blocking_action_queue_push(baq, death_action);

        return true;
    }

    return false;
}

void action_planner_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(character, i))
            continue;

        MoveIntent *mi = GET_COMPONENT(move_intent, i);
        CombatIntent *ci = GET_COMPONENT(combat_intent, i);
        BlockingActionQueue *baq = GET_COMPONENT(blocking_action_queue, i);
        NonBlockingActionQueue *nbaq = GET_COMPONENT(non_blocking_action_queue, i);
        Character *c = GET_COMPONENT(character, i);
        CombatEffect *ce = GET_COMPONENT(combat_effect, i);
        const Transform3D *t = GET_COMPONENT(transform, i);

        if (process_death(baq, c, t, ce, mi, i)) continue;

        if (process_stun(baq, c, t, ce, mi)) continue;

        process_combat_slot_q(ci, c, nbaq);
        process_combat_slot_e(ci, c, nbaq);

        const Action *ba = blocking_action_queue_peek(baq);

        if (ba && ba->action_type == CHARACTER_ACTION_ATTACK) {
            continue;
        }

        if (process_combat_action_secondary(ci, baq, c)) continue;

        if (process_combat_action_w(ci, baq, c)) continue;

        process_move_intent(mi, ci, baq, c);
    }
}
