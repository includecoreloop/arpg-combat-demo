#include "enemy_ai_system.h"

#include "../ecs/ecs.h"

#include "../components/tags.h"
#include "../components/action_queue.h"
#include "../components/character.h"
#include "../components/move_intent.h"

void enemy_ai_system_update(void) {
    if (!ENABLE_ENEMY_AI) return;

    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(enemy_tag, i) || !HAS_COMPONENT(blocking_action_queue, i)) continue;

        MoveIntent *mi = GET_COMPONENT(move_intent, i);

        Entity player_entity = (Entity) -1;

        for (int j = 0; j < MAX_ENTITIES; ++j) {
            if (HAS_COMPONENT(player_tag, j)) {
                player_entity = j;
            }
        }

        if (player_entity == (Entity) -1) continue;

        const Character *c = GET_COMPONENT(character, player_entity);

        if (!character_is_alive(c)) {
            mi->has_intent = false;
            mi->has_target_entity = false;
            mi->target_entity = (Entity) -1;

            continue;
        }

        mi->has_intent = true;
        mi->has_target_entity = true;
        mi->target_entity = player_entity;
    }
}
