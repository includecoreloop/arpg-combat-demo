#pragma once

#include <stdarg.h>
#include <stdio.h>
#include "../ecs/ecs.h"
#include "../core/math.h"

typedef struct {
    Vec3 position;
    char text[8];
    float timer;
    float timer_default;
    bool render_intent;
} CharacterFloatingText;

typedef struct {
    int health_points;
    int max_health_points;

    int power_points;
    int max_power_points;

    float movement_speed;
    float target_height;

    CharacterFloatingText floating_text;
} Character;

COMPONENT_DEFINE(Character, character);

static inline void character_take_damage(Character *c, const int damage) {
    c->health_points -= damage;

    if (c->health_points < 0) {
        c->health_points = 0;
    }
}

static inline void character_heal(Character *c, const int hp) {
    c->health_points += hp;

    if (c->health_points > c->max_health_points) {
        c->health_points = c->max_health_points;
    }
}

static inline void character_use_power_points(Character *c, const int power_points) {
    c->power_points -= power_points;

    if (c->power_points < 0) {
        c->power_points = 0;
    }
}

static inline void character_add_power_points(Character *c, const int power_points) {
    c->power_points += power_points;

    if (c->power_points > c->max_power_points) {
        c->power_points = c->max_power_points;
    }
}

static inline bool character_has_enough_power_points(const Character *c, const int power_points) {
    return c->power_points >= power_points;
}

static inline bool character_has_full_health_points(const Character *c) {
    return c->health_points >= c->max_health_points;
}

static inline void character_floating_text_set(Character *c, const Vec3 position, const char *fmt, ...) {
    CharacterFloatingText floating_text = {
        .position = position,
        .timer = c->floating_text.timer_default,
        .render_intent = true,
    };

    va_list args;
    va_start(args, fmt);
    vsnprintf(floating_text.text, sizeof(floating_text.text), fmt, args);
    va_end(args);

    c->floating_text = floating_text;
}

static inline void character_floating_text_reset(Character *c) {
    c->floating_text = (CharacterFloatingText){0};
    c->floating_text.timer_default = 1.0f;
}

static inline bool character_is_alive(const Character *c) {
    return c->health_points > 0;
}
