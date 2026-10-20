#pragma once

#include "../../ecs/ecs.h"
#include "../../core/math.h"

Entity mutant_prefab_create(Vec3 position, bool player);

void mutant_prefab_destroy(Entity entity);
