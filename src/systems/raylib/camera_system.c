#include "camera_system.h"

#include "../../core/math.h"
#include "../../core/config.h"

#include "../../components/transform.h"
#include "../../components/tags.h"
#include "../../game/scene.h"

void camera_system_update(const double dt) {
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(player_tag, i))
            continue;

        const Transform3D *player_transform = GET_COMPONENT(transform, i);
        if (!player_transform) return;

        const Vector3 desired_target = {
            .x = player_transform->position.x,
            .y = player_transform->position.y,
            .z = player_transform->position.z
        };

        const Vector3 desired_position = {
            .x = player_transform->position.x + CAMERA_OFFSET.x,
            .y = player_transform->position.y + CAMERA_OFFSET.y,
            .z = player_transform->position.z + CAMERA_OFFSET.z
        };

        const float smoothness = 10.0f * (float) dt;

        game_camera.target.x = LERP(game_camera.target.x, desired_target.x, smoothness);
        game_camera.target.y = LERP(game_camera.target.y, desired_target.y, smoothness);
        game_camera.target.z = LERP(game_camera.target.z, desired_target.z, smoothness);

        game_camera.position.x = LERP(game_camera.position.x, desired_position.x, smoothness);
        game_camera.position.y = LERP(game_camera.position.y, desired_position.y, smoothness);
        game_camera.position.z = LERP(game_camera.position.z, desired_position.z, smoothness);
    }
}
