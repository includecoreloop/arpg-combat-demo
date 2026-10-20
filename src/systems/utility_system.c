#include "utility_system.h"

#include "../ecs/ecs.h"

#include "../components/action_queue.h"
#include "../components/character.h"
#include "../components/combat_effect.h"
#include "../components/transform.h"

static void set_floating_text(const Entity target_e, Character *target_c, const int num) {
    character_floating_text_reset(target_c);

    const Transform3D *target_t = GET_COMPONENT(transform, target_e);

    const Vec3 target_pos = {
        .x = target_t->position.x, .y = target_t->position.y + target_c->target_height, .z = target_t->position.z
    };

    character_floating_text_set(target_c, target_pos, "+%d HP", num);
}

static void process_heal(const Entity entity, Character *c, const CombatAction *ca) {
    if (ca->health_points_gain > 0) {
        character_heal(c, ca->health_points_gain);

        set_floating_text(entity, c, ca->health_points_gain);
    }
}

static void process_utility(const float dt, const Entity entity, NonBlockingActionQueue *nbaq, const Action *a) {
    if (a->action_type != CHARACTER_ACTION_USE_UTILITY) return;

    CombatIntent *ci = GET_COMPONENT(combat_intent, entity);
    Character *c = GET_COMPONENT(character, entity);

    if (ci->active_slot_nb == COMBAT_SLOT_NONE) {
        ci->active_slot_nb = a->action_payload.combat_slot;

        CombatAction *ca = &ci->actions[ci->active_slot_nb];
        ca->cast_timer = 0.0f;

        process_heal(entity, c, ca);

        if (ca->damage_amplifier > COMBAT_EFFECT_DEFAULT_DAMAGE_AMPLIFIER) {
            CombatEffect *ce = GET_COMPONENT(combat_effect, entity);

            ce->damage_amplifier = ca->damage_amplifier;
            ce->damage_amplifier_timer = ca->damage_amplifier_duration;
        }

    }

    CombatAction *active_utility = &ci->actions[ci->active_slot_nb];
    active_utility->cast_timer += dt;

    if (active_utility->cast_timer >= active_utility->duration) {
        active_utility->cooldown_timer = active_utility->cooldown;
        active_utility->cast_timer = 0.0f;
        active_utility->hit_entity_count = 0;

        ci->active_slot_nb = COMBAT_SLOT_NONE;

        non_blocking_action_queue_pop(nbaq);
    }
}

void utility_system_update(const float dt) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(non_blocking_action_queue, i)) continue;

        NonBlockingActionQueue *nbaq = GET_COMPONENT(non_blocking_action_queue, i);

        Action *a = non_blocking_action_queue_peek(nbaq);

        if (!a) continue;

        process_utility(dt, i, nbaq, a);
    }
}
