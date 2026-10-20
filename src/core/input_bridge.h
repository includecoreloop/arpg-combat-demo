#pragma once

#include <stdbool.h>
#include "math.h"

typedef struct {
    Vec3 mouse_position;
    bool action_primary;
    bool action_secondary;

    bool action_q;
    bool action_w;
    bool action_e;

    bool action_p;
    bool action_r;
} InputBridge;

extern InputBridge input_bridge;

static inline void input_bridge_consume_action(bool *action) {
    *action = false;
}

static inline void input_bridge_consume_all(InputBridge *ib) {
    *ib = (InputBridge) {0};
}
