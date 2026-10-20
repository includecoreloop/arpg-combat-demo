#include "character_animation_system.h"

#include "../ecs/ecs.h"
#include "../core/config.h"
#include "../components/character.h"
#include "../components/character_animation_map.h"
#include "../components/animated_render_3d.h"
#include "../components/combat.h"
#include "../components/action_queue.h"
#include "../components/velocity.h"

void character_animation_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(character, i))
            continue;

        AnimatedRender3D *render = GET_COMPONENT(animated_render_3d, i);
        const CharacterAnimationMap *map = GET_COMPONENT(character_animation_map, i);

        uint32_t target_anim_set = map->idle_animation_set_id;
        float target_anim_speed = render->default_animation_speed;

        if (HAS_COMPONENT(blocking_action_queue, i)) {
            BlockingActionQueue *baq = GET_COMPONENT(blocking_action_queue, i);
            const Action *a = blocking_action_queue_peek(baq);

            if (a) {
                if (a->action_type == CHARACTER_ACTION_DIE) {
                    target_anim_set = map->death_animation_set_id;
                    render->loop = false;
                } if (a->action_type == CHARACTER_ACTION_ATTACK) {
                    if (HAS_COMPONENT(combat_intent, i)) {
                        CombatIntent *ci = GET_COMPONENT(combat_intent, i);
                        const CombatAction *ca = combat_intent_get_processing_attack(ci);

                        if (ca && ci->active_slot_b >= 0) {
                            const AttackAnimationData *anim_data = &map->attack_animations[ci->active_slot_b];
                            target_anim_set = anim_data->animation_set_id;

                            const float clip_duration = (float) anim_data->frame_count / (float) DEFAULT_ANIMATION_FPS;
                            target_anim_speed = clip_duration / ca->duration;
                        }
                    }
                } else if (a->action_type == CHARACTER_ACTION_MOVE_TO_POSITION ||
                           a->action_type == CHARACTER_ACTION_MOVE_TO_TARGET) {
                    if (HAS_COMPONENT(velocity, i)) {
                        const Velocity *v = GET_COMPONENT(velocity, i);
                        if (velocity_is_moving(v)) {
                            target_anim_set = map->run_animation_set_id;
                        }
                    }
                }
            }
        }

        if (render->animation_set_id != target_anim_set) {
            render->animation_set_id = target_anim_set;
            render->animation_speed = target_anim_speed;
            render->current_animation_in_set = 0;
            render->animation_frame_counter = 0.0f;
        }
    }
}
