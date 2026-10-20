#include "mutant_prefab.h"

#include "../../asset/raylib/raylib_model_manager.h"
#include "../../components/animated_render_3d.h"
#include "../../components/character.h"
#include "../../components/collider.h"
#include "../../components/orientation_intent.h"
#include "../../components/render.h"
#include "../../components/transform.h"
#include "../../components/velocity.h"
#include "../../components/move_intent.h"
#include "../../components/character_animation_map.h"
#include "../../components/tags.h"
#include "../../components/combat.h"
#include "../../components/action_queue.h"
#include "../../components/combat_effect.h"
#include "../profiles/mutant_skills_profile.h"

Entity mutant_prefab_create(const Vec3 position, const bool player) {
    const Entity entity = create_entity();

    const Transform3D transform_comp = {
        .position = position,
        .scale = {.x = 0.02f, .y = 0.02f, .z = 0.02f},
        .rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f},
    };

    ADD_COMPONENT(transform, entity, transform_comp);

    const Velocity velocity_comp = {0};
    ADD_COMPONENT(velocity, entity, velocity_comp);

    const Character character_comp = {
        .movement_speed = 2.0f,
        .max_health_points = 150,
        .health_points = 150,
        .power_points = 100,
        .max_power_points = 100,
        .target_height = 5.0f,
    };

    ADD_COMPONENT(character, entity, character_comp);

    const uint32_t model_id = raylib_model_manager_load("assets/models/mutant.glb");
    const uint32_t idle_anim_set_id = raylib_model_manager_load_animation("assets/models/mutant_idle.glb");

    const AnimatedRender3D animated_render_comp = {
        .model_id = model_id,
        .animation_set_id = idle_anim_set_id,
        .current_animation_in_set = 0,
        .animation_frame_counter = 0.0f,
        .animation_speed = 1.0f,
        .default_animation_speed = 0.8f,
        .loop = true,
        .is_playing = true
    };

    ADD_COMPONENT(animated_render_3d, entity, animated_render_comp);

    const uint32_t run_anim_set_id = raylib_model_manager_load_animation("assets/models/mutant_walk.glb");
    const uint32_t death_anim_set_id = raylib_model_manager_load_animation("assets/models/mutant_death.glb");
    const uint32_t primary_attack_anim_set_id = raylib_model_manager_load_animation(
            "assets/models/mutant_attack_primary.glb");

    const uint32_t primary_attack_anim_frame_count = raylib_model_manager_get_animation_frame_count(
        primary_attack_anim_set_id);

    const CharacterAnimationMap anim_map_comp = {
        .idle_animation_set_id = idle_anim_set_id,
        .run_animation_set_id = run_anim_set_id,
        .death_animation_set_id = death_anim_set_id,
        .attack_animations = {
            [COMBAT_SLOT_PRIMARY] = {
                .animation_set_id = primary_attack_anim_set_id,
                .frame_count = primary_attack_anim_frame_count
            },
        }
    };
    ADD_COMPONENT(character_animation_map, entity, anim_map_comp);

    const OrientationIntent orientation_intent_comp = {0};
    ADD_COMPONENT(orientation_intent, entity, orientation_intent_comp);

    const Collider3D collider_comp = {
        .type = COLLIDER_CIRCLE,
        .is_static = false,
        .is_trigger = false,
        .is_active = true,
        .offset = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
        .circles = {
            [0] = {
                .offset = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
                .radius = 1.5f
            }
        },
        .circle_count = 1,
        .trigger_contact_count = 0,
        .mass = 500.0f,
    };

    ADD_COMPONENT(collider, entity, collider_comp);

    const Render render_comp = {0};
    ADD_COMPONENT(render, entity, render_comp);

    const MoveIntent move_intent_comp = {
        .target_position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
        .stop_distance = 0.1f,
        .has_intent = false,
        .target_entity = -1,
        .has_target_entity = false,
    };

    ADD_COMPONENT(move_intent, entity, move_intent_comp);

    if (player) {
        const PlayerTag player_tag_component = {0};
        ADD_COMPONENT(player_tag, entity, player_tag_component);
    } else {
        const EnemyTag enemy_tag_component = {0};
        ADD_COMPONENT(enemy_tag, entity, enemy_tag_component);
    }

    CombatIntent combat_intent_comp = {0};

    mutant_combat_profile_apply(&combat_intent_comp);

    ADD_COMPONENT(combat_intent, entity, combat_intent_comp);

    const MutantClassTag mutant_class_tag_component = {0};
    ADD_COMPONENT(mutant_class_tag, entity, mutant_class_tag_component);

    const BlockingActionQueue action_queue_comp = {0};

    ADD_COMPONENT(blocking_action_queue, entity, action_queue_comp);

    const CombatEffect combat_effect_comp = {
        .damage_amplifier = COMBAT_EFFECT_DEFAULT_DAMAGE_AMPLIFIER,
    };

    ADD_COMPONENT(combat_effect, entity, combat_effect_comp);

    return entity;
}

void mutant_prefab_destroy(const Entity entity) {
    REMOVE_COMPONENT(transform, entity);
    REMOVE_COMPONENT(velocity, entity);
    REMOVE_COMPONENT(character, entity);
    REMOVE_COMPONENT(animated_render_3d, entity);
    REMOVE_COMPONENT(character_animation_map, entity);
    REMOVE_COMPONENT(orientation_intent, entity);
    REMOVE_COMPONENT(collider, entity);
    REMOVE_COMPONENT(render, entity);
    REMOVE_COMPONENT(move_intent, entity);

    if (HAS_COMPONENT(player_tag, entity)) {
        REMOVE_COMPONENT(player_tag, entity);
    } else {
        REMOVE_COMPONENT(enemy_tag, entity);
    }

    REMOVE_COMPONENT(combat_intent, entity);
    REMOVE_COMPONENT(mutant_class_tag, entity);
    REMOVE_COMPONENT(blocking_action_queue, entity);
    REMOVE_COMPONENT(combat_effect, entity);

    destroy_entity(entity);
}
