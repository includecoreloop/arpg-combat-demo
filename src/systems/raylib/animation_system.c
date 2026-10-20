#include "animation_system.h"

#include "../../core/config.h"
#include "../../components/animated_render_3d.h"
#include "../../asset/raylib/raylib_model_manager.h"

void animation_system_update(const float dt) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(animated_render_3d, i)) continue;

        AnimatedRender3D *r = GET_COMPONENT(animated_render_3d, i);
        if (!r->is_playing) continue;

        const uint32_t keyframe_count = raylib_model_manager_get_animation_frame_count(r->animation_set_id);

        r->animation_frame_counter += r->animation_speed * DEFAULT_ANIMATION_FPS * dt;

        if (r->animation_frame_counter >= (float)keyframe_count) {
            if (r->loop) {
                r->animation_frame_counter = 0.0f;
            } else {
                r->animation_frame_counter = (float)(keyframe_count - 1);
                r->is_playing = false;
            }
        }
    }
}