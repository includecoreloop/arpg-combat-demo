#pragma once

#include "../ecs/ecs.h"

#define MAX_HIT_TARGETS_PER_ATTACK 16

typedef enum {
    COMBAT_SLOT_PRIMARY = 0,
    COMBAT_SLOT_SECONDARY,
    COMBAT_SLOT_Q,
    COMBAT_SLOT_W,
    COMBAT_SLOT_E,
    COMBAT_SLOT_COUNT,
    COMBAT_SLOT_NONE = -1
} CombatSlot;

typedef struct {
    int power_cost;
    int power_gain;
    int health_points_gain;

    float duration;
    float hit_start_time;
    float hit_end_time;

    float cooldown;
    float cooldown_timer;
    float cast_timer;

    int damage;
    float attack_range;
    float stun_duration;

    float damage_amplifier;
    float damage_amplifier_duration;

    Entity hitbox_collider_entity;
    Entity hit_entities[MAX_HIT_TARGETS_PER_ATTACK];
    int hit_entity_count;
} CombatAction;

typedef struct {
    CombatAction actions[COMBAT_SLOT_COUNT];
    CombatSlot active_slot_b;
    CombatSlot intent_slot_b;

    CombatSlot active_slot_nb;
    CombatSlot intent_slot_nb;

    Entity target_entity;
    bool has_target_entity;
} CombatIntent;

COMPONENT_DEFINE(CombatIntent, combat_intent);

static inline CombatAction *combat_intent_get_processing_attack(CombatIntent *intent) {
    if (intent->active_slot_b >= 0) {
        return &intent->actions[intent->active_slot_b];
    }

    return NULL;
}

static inline void combat_intent_clear_target_entity(CombatIntent *ci) {
    ci->target_entity = (Entity) -1;
    ci->has_target_entity = false;
}
