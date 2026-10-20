#include "cooldown_system.h"

#include "../ecs/ecs.h"

#include "../components/combat.h"
#include "../components/character.h"
#include "../components/combat_effect.h"

static void update_combat_cooldowns(CombatIntent *ci, const float dt) {
    for (int slot = 0; slot < COMBAT_SLOT_COUNT; ++slot) {
        CombatAction *action = &ci->actions[slot];
        if (action->cooldown_timer > 0.0f) {
            action->cooldown_timer -= dt;
            if (action->cooldown_timer < 0.0f) {
                action->cooldown_timer = 0.0f;
            }
        }
    }
}

static void update_hit_render_timer(Character *c, const float dt) {
    if (c->floating_text.render_intent) {
        c->floating_text.timer -= dt;

        if (c->floating_text.timer <= 0.0f) {
            character_floating_text_reset(c);
        }
    }
}

static void update_stun_timer(CombatEffect *ce, const float dt) {
    if (ce->stunned) {
        ce->stun_timer -= dt;
        if (ce->stun_timer <= 0.0f) {
            ce->stun_timer = 0.0f;
            ce->stunned = false;
        }
    }
}

static void update_performance_multiplier_timer(CombatEffect *ce, const float dt) {
    if (ce->damage_amplifier > COMBAT_EFFECT_DEFAULT_DAMAGE_AMPLIFIER) {
        ce->damage_amplifier_timer -= dt;
        if (ce->damage_amplifier_timer <= 0.0f) {
            ce->damage_amplifier_timer = 0.0f;
            ce->damage_amplifier = COMBAT_EFFECT_DEFAULT_DAMAGE_AMPLIFIER;
        }
    }
}

void cooldown_system_update(const float dt) {
    for (Entity i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(character, i)) continue;

        Character *c = GET_COMPONENT(character, i);
        CombatIntent *ci = GET_COMPONENT(combat_intent, i);
        CombatEffect *ce = GET_COMPONENT(combat_effect, i);

        update_combat_cooldowns(ci, dt);
        update_hit_render_timer(c, dt);
        update_stun_timer(ce, dt);
        update_performance_multiplier_timer(ce, dt);
    }
}
