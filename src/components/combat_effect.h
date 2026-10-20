#pragma once

#include "../ecs/ecs.h"

#define COMBAT_EFFECT_DEFAULT_DAMAGE_AMPLIFIER 1.0f

typedef struct {
    bool stunned;
    bool stun_intent;
    float stun_timer;

    float damage_amplifier;
    float damage_amplifier_timer;
} CombatEffect;

COMPONENT_DEFINE(CombatEffect, combat_effect);
