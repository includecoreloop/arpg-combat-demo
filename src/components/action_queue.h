#pragma once

#include "../ecs/ecs.h"
#include "../core/config.h"
#include "combat.h"

typedef enum {
    CHARACTER_ACTION_IDLE,
    CHARACTER_ACTION_MOVE_TO_POSITION,
    CHARACTER_ACTION_MOVE_TO_TARGET,
    CHARACTER_ACTION_ROTATE_TO_POSITION,
    CHARACTER_ACTION_ATTACK,
    CHARACTER_ACTION_USE_UTILITY,
    CHARACTER_ACTION_DIE,
} ActionType;

typedef struct {
    CombatSlot combat_slot;
} ActionPayload;

typedef struct {
    ActionType action_type;
    ActionPayload action_payload;
} Action;

typedef struct {
    Action items[ACTION_QUEUE_MAX_ACTIONS];
    int head;
    int tail;
    int count;
} BlockingActionQueue;

typedef struct {
    Action items[ACTION_QUEUE_MAX_ACTIONS];
    int head;
    int tail;
    int count;
} NonBlockingActionQueue;

COMPONENT_DEFINE(BlockingActionQueue, blocking_action_queue);
COMPONENT_DEFINE(NonBlockingActionQueue, non_blocking_action_queue);

static inline void blocking_action_queue_clear(BlockingActionQueue *aq) {
    aq->head = aq->tail = aq->count = 0;
}

static inline bool blocking_action_queue_push(BlockingActionQueue *aq, const Action action) {
    if (aq->count >= ACTION_QUEUE_MAX_ACTIONS) return false;

    aq->items[aq->tail] = action;
    aq->tail = aq->tail + 1 >= ACTION_QUEUE_MAX_ACTIONS ? 0 : aq->tail + 1;
    aq->count++;

    return true;
}

static inline Action *blocking_action_queue_peek(BlockingActionQueue *aq) {
    return aq->count > 0 ? &aq->items[aq->head] : NULL;
}

static inline Action *blocking_action_queue_peek_next(BlockingActionQueue *aq) {
    if (aq->count >= 1) {
        const int next_head = aq->head + 1 >= ACTION_QUEUE_MAX_ACTIONS ? 0 : aq->head + 1;

        return &aq->items[next_head];
    }

    return NULL;
}

static inline void blocking_action_queue_pop(BlockingActionQueue *aq) {
    if (aq->count == 0) return;
    aq->head = aq->head + 1 >= ACTION_QUEUE_MAX_ACTIONS ? 0 : aq->head + 1;
    aq->count--;
}

//--------------------

static inline void non_blocking_action_queue_clear(NonBlockingActionQueue *aq) {
    aq->head = aq->tail = aq->count = 0;
}

static inline bool non_blocking_action_queue_push(NonBlockingActionQueue *aq, const Action action) {
    if (aq->count >= ACTION_QUEUE_MAX_ACTIONS) return false;

    aq->items[aq->tail] = action;
    aq->tail = aq->tail + 1 >= ACTION_QUEUE_MAX_ACTIONS ? 0 : aq->tail + 1;
    aq->count++;

    return true;
}

static inline Action *non_blocking_action_queue_peek(NonBlockingActionQueue *aq) {
    return aq->count > 0 ? &aq->items[aq->head] : NULL;
}

static inline Action *non_blocking_action_queue_peek_next(NonBlockingActionQueue *aq) {
    if (aq->count >= 1) {
        const int next_head = aq->head + 1 >= ACTION_QUEUE_MAX_ACTIONS ? 0 : aq->head + 1;

        return &aq->items[next_head];
    }

    return NULL;
}

static inline void non_blocking_action_queue_pop(NonBlockingActionQueue *aq) {
    if (aq->count == 0) return;
    aq->head = aq->head + 1 >= ACTION_QUEUE_MAX_ACTIONS ? 0 : aq->head + 1;
    aq->count--;
}
