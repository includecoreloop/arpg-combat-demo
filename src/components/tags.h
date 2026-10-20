#pragma once

#include "../ecs/ecs.h"

typedef struct {
    bool _unused;
} PlayerTag;

typedef struct {
    bool _unused;
} EnemyTag;

COMPONENT_DEFINE(PlayerTag, player_tag);
COMPONENT_DEFINE(EnemyTag, enemy_tag);

typedef struct {
    bool _unused;
} BruteClassTag;

typedef struct {
    bool _unused;
} MutantClassTag;

COMPONENT_DEFINE(BruteClassTag, brute_class_tag);
COMPONENT_DEFINE(MutantClassTag, mutant_class_tag);
