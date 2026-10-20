#include "ecs.h"

static Entity free_list[MAX_ENTITIES];
static int free_count = 0;
static bool initialized = false;

void ecs_init_entities(void) {
    if (initialized) return;

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        free_list[i] = MAX_ENTITIES - 1 - i;
    }

    free_count = MAX_ENTITIES;
    initialized = true;
}

Entity create_entity() {
    ecs_init_entities();

    if (free_count == 0)
        return MAX_ENTITIES;

    return free_list[--free_count];
}

void destroy_entity(const Entity e) {
    if (free_count >= MAX_ENTITIES)
        return;

    free_list[free_count++] = e;
}
