#include "hit_box_system.h"

#include "../ecs/ecs.h"
#include "../components/character.h"
#include "../components/collider.h"
#include "../components/combat.h"
#include "../components/combat_effect.h"
#include "../components/transform.h"

static bool has_already_been_hit(const CombatAction *ca, const Entity target) {
    for (int i = 0; i < ca->hit_entity_count; ++i) {
        if (ca->hit_entities[i] == target) {
            return true;
        }
    }

    return false;
}

static void set_floating_text(const int dmg, const Entity target_e, Character *target_c) {
    character_floating_text_reset(target_c);

    const Transform3D *target_t = GET_COMPONENT(transform, target_e);

    const Vec3 target_pos = {
        .x = target_t->position.x, .y = target_t->position.y + target_c->target_height, .z = target_t->position.z
    };

    character_floating_text_set(target_c, target_pos, "%d", dmg);
}

static void process_stun_intent(const CombatAction *ca, const Entity target_e) {
    if (ca->stun_duration > 0.0f) {
        CombatEffect *ce = GET_COMPONENT(combat_effect, target_e);
        ce->stun_timer = ca->stun_duration;
        ce->stun_intent = true;
    }
}

void hit_box_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(character, i) || !HAS_COMPONENT(combat_intent, i))
            continue;

        CombatIntent *ci = GET_COMPONENT(combat_intent, i);

        if (ci->active_slot_b == COMBAT_SLOT_NONE)
            continue;

        CombatAction *ca = &ci->actions[ci->active_slot_b];
        const Entity hit_box_e = ca->hitbox_collider_entity;

        const Collider3D *hit_box_col = GET_COMPONENT(collider, hit_box_e);

        if (!hit_box_col->is_active)
            continue;

        const CombatEffect *ce = GET_COMPONENT(combat_effect, i);

        for (int k = 0; k < hit_box_col->trigger_contact_count; ++k) {
            const Entity target_e = hit_box_col->triggered_entities[k];

            if (target_e == i) continue;

            if (has_already_been_hit(ca, target_e)) continue;

            if (ca->hit_entity_count < MAX_HIT_TARGETS_PER_ATTACK) {
                ca->hit_entities[ca->hit_entity_count++] = target_e;
            }

            Character *target_c = GET_COMPONENT(character, target_e);

            const int dmg = (int) roundf((float) ca->damage * ce->damage_amplifier);

            character_take_damage(target_c, dmg);

            process_stun_intent(ca, target_e);

            set_floating_text(dmg, target_e, target_c);
        }
    }
}
