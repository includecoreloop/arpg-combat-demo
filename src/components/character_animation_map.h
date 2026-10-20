#pragma once

#include "stdint.h"
#include "../ecs/ecs.h"

#include "../components/combat.h"

typedef struct {
    uint32_t animation_set_id;
    uint32_t frame_count;
} AttackAnimationData;

typedef struct {
    uint32_t idle_animation_set_id;
    uint32_t idle_animation_frame_count;

    uint32_t run_animation_set_id;
    uint32_t run_animation_frame_count;

    uint32_t death_animation_set_id;
    uint32_t death_animation_frame_count;

    AttackAnimationData attack_animations[COMBAT_SLOT_COUNT];
} CharacterAnimationMap;

COMPONENT_DEFINE(CharacterAnimationMap, character_animation_map);
