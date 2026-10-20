#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ENTITIES 64
typedef uint32_t Entity;

Entity create_entity();

void destroy_entity(Entity e);

void ecs_init_entities(void);

typedef struct {
    bool has[MAX_ENTITIES];
} ComponentMask;

#define COMPONENT_DEFINE(Type, Name) \
typedef struct { Type data[MAX_ENTITIES]; ComponentMask mask; } Name##Store; \
extern Name##Store Name

#define COMPONENT_IMPLEMENT(Type, Name) \
Name##Store Name = {0}

#define ADD_COMPONENT(Store, e, v) do { Store.data[e] = v; Store.mask.has[e] = true; } while(0)
#define REMOVE_COMPONENT(Store, e) do { \
    Store.mask.has[e] = false; \
    memset(&Store.data[e], 0, sizeof(Store.data[e])); \
} while(0)
#define HAS_COMPONENT(Store, e) (Store.mask.has[e])
#define GET_COMPONENT(Store, e) (&Store.data[e])
