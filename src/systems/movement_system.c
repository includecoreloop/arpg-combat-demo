#include "movement_system.h"

#include "../components/transform.h"
#include "../components/velocity.h"

void movement_system_update(const double dt) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (HAS_COMPONENT(transform, i) && HAS_COMPONENT(velocity, i)) {
            Transform3D *t = GET_COMPONENT(transform, i);
            const Velocity *v = GET_COMPONENT(velocity, i);

            t->position.x += v->direction.x * v->speed * dt;
            t->position.y += v->direction.y * v->speed * dt;
            t->position.z += v->direction.z * v->speed * dt;
        }
    }
}
