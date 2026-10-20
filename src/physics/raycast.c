#include "raycast.h"
#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "../game/scene.h"
#include "../components/transform.h"
#include "../components/tags.h"
#include "../components/collider.h"
#include "../components/character.h"

bool raycast_ground_plane(const Vec3 mouse_screen_pos, const float plane_y, Vec3 *out_hit_point) {
    const Vector2 screen_pos = (Vector2){.x = mouse_screen_pos.x, .y = mouse_screen_pos.y};
    const Ray ray = GetScreenToWorldRay(screen_pos, game_camera);

    if (ray.direction.y == 0.0f)
        return false;

    const float t = (plane_y - ray.position.y) / ray.direction.y;
    if (t < 0.0f)
        return false;

    const Vector3 hit = Vector3Add(ray.position, Vector3Scale(ray.direction, t));

    if (out_hit_point) {
        *out_hit_point = (Vec3){.x = hit.x, .y = hit.y, .z = hit.z};
    }

    return true;
}

static bool ray_intersects_infinite_cylinder(const Ray ray, const Vector3 center, const float radius, float *out_t) {
    const float ox = ray.position.x - center.x;
    const float oz = ray.position.z - center.z;
    const float dx = ray.direction.x;
    const float dz = ray.direction.z;

    const float a = dx * dx + dz * dz;
    const float b = 2.0f * (ox * dx + oz * dz);
    const float c = ox * ox + oz * oz - radius * radius;

    const float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f) return false;

    const float sqrt_disc = sqrtf(discriminant);
    const float t1 = (-b - sqrt_disc) / (2.0f * a);
    const float t2 = (-b + sqrt_disc) / (2.0f * a);

    const float hit_t = t1 > 0.0f ? t1 : t2;

    if (hit_t <= 0.0f) return false;

    if (out_t) *out_t = hit_t;

    return true;
}

static bool ray_intersects_vertical_cylinder(const Ray ray, const Vector3 base_position, const float radius,
                                             const float height, float *out_t) {
    float hit_t = 0.0f;
    if (!ray_intersects_infinite_cylinder(ray, base_position, radius, &hit_t)) {
        return false;
    }

    const float hit_y = ray.position.y + hit_t * ray.direction.y;
    const float min_y = base_position.y;
    const float max_y = base_position.y + height;

    if (hit_y < min_y || hit_y > max_y) {
        return false;
    }

    if (out_t) *out_t = hit_t;

    return true;
}

static float get_entity_target_radius(const Entity entity) {
    const Collider3D *col = GET_COMPONENT(collider, entity);

    return col->circles[0].radius;
}

static float get_entity_target_height(const Entity entity) {
    const Character *c = GET_COMPONENT(character, entity);

    return c->target_height;
}

Entity raycast_enemy_target(const Vec3 mouse_screen_pos) {
    const Vector2 screen_pos = (Vector2){.x = mouse_screen_pos.x, .y = mouse_screen_pos.y};
    const Ray ray = GetScreenToWorldRay(screen_pos, game_camera);

    Entity closest_entity = (Entity) -1;
    float closest_t = 1e9f;

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(enemy_tag, i)) continue;

        const Transform3D *t = GET_COMPONENT(transform, i);
        const Vector3 pos = (Vector3){.x = t->position.x, .y = t->position.y, .z = t->position.z};

        const float radius = get_entity_target_radius(i);
        const float height = get_entity_target_height(i);

        float hit_t = 0.0f;
        if (ray_intersects_vertical_cylinder(ray, pos, radius, height, &hit_t)) {
            if (hit_t < closest_t) {
                closest_t = hit_t;
                closest_entity = i;
            }
        }
    }

    return closest_entity;
}
