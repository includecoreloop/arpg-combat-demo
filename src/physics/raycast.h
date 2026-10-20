#pragma once

#include <stdbool.h>
#include "../ecs/ecs.h"
#include "../core/math.h"


bool raycast_ground_plane(Vec3 mouse_screen_pos, float plane_y, Vec3 *out_hit_point);

Entity raycast_enemy_target(Vec3 mouse_screen_pos);