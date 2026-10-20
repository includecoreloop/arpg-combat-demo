#include "render_system.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "../../core/config.h"
#include "../../game/scene.h"
#include "../../asset/raylib/raylib_model_manager.h"

#include "../../components/transform.h"
#include "../../components/animated_render_3d.h"
#include "../../components/character.h"
#include "../../components/static_render_3d.h"
#include "../../components/collider.h"
#include "../../components/combat.h"
#include "../../components/tags.h"

static void render_main_menu_instructions(void) {
    const float screen_w = (float) GetScreenWidth();
    const float screen_h = (float) GetScreenHeight();

    DrawRectangle(0, 0, (int) screen_w, (int) screen_h, ColorAlpha(BLACK, 0.85f));

    const float win_w = screen_w * 0.65f;
    const float win_h = screen_h * 0.75f;
    const float win_x = (screen_w - win_w) * 0.5f;
    const float win_y = (screen_h - win_h) * 0.5f;

    const Rectangle panel = {.x = win_x, .y = win_y, .width = win_w, .height = win_h};
    DrawRectangleRounded(panel, 0.08f, 4, GetColor(0x141414FF));
    DrawRectangleRoundedLines(panel, 0.08f, 4, GetColor(0x606060FF));

    const int title_font = (int) (win_h * 0.08f);
    const char *title = "COMBAT ENGINE CONTROLS";
    const int title_w = MeasureText(title, title_font);
    DrawText(title, (int) (win_x + (win_w - title_w) * 0.5f), (int) (win_y + win_h * 0.06f), title_font, WHITE);

    const char *lines[] = {
        "Left Click: Move / Primary Attack (Generates Power)",
        "Right Click: Secondary Attack (Requires 20 Power)",
        "Q: Use Heal Potion",
        "W: Damage & Stun Surrounding Enemies",
        "E: Increase Movement & Attack Speed",
        "",
        "P: START GAME",
        "R: RESTART GAME"
    };

    const int font_size = (int) (win_h * 0.035f);
    const float start_y = win_y + win_h * 0.22f;
    const float line_spacing = win_h * 0.07f;

    for (int i = 0; i < 8; i++) {
        Color color = WHITE;
        if (i >= 6) {
            color = GetColor(0x2ECC71FF);
        }

        const int text_w = MeasureText(lines[i], font_size);
        const int text_x = (int) (win_x + (win_w - text_w) * 0.5f);
        const int text_y = (int) (start_y + (float) i * line_spacing);

        DrawText(lines[i], text_x, text_y, font_size, color);
    }
}

static void render_floating_text(const Camera3D camera, const Vector3 world_pos, const char *text,
                                 const float life_pct, const Color color, const int base_font_size) {
    if (life_pct <= 0.0f || text == NULL) return;

    const Vector2 screen_pos = GetWorldToScreen(world_pos, camera);

    const float screen_w = (float) GetScreenWidth();
    const float screen_h = (float) GetScreenHeight();
    if (screen_pos.x < -100.0f || screen_pos.x > screen_w + 100.0f ||
        screen_pos.y < -100.0f || screen_pos.y > screen_h + 100.0f) {
        return;
    }

    const float scale_multiplier = 1.0f + 0.35f * sinf((1.0f - life_pct) * PI);
    int current_font_size = (int) (base_font_size * scale_multiplier);
    if (current_font_size < 1) current_font_size = 1;

    const Color final_color = ColorAlpha(color, life_pct);

    const int text_width = MeasureText(text, current_font_size);
    const int draw_x = (int) (screen_pos.x - text_width * 0.5f);
    const int draw_y = (int) (screen_pos.y - current_font_size * 0.5f);

    const Color shadow_color = ColorAlpha(BLACK, life_pct * 0.8f);
    DrawText(text, draw_x + 2, draw_y + 2, current_font_size, shadow_color);

    DrawText(text, draw_x, draw_y, current_font_size, final_color);
}

static void render_hud_draw_globe(const Vector2 center, const float radius, float current_val, float max_val,
                                  const Color fill_color) {
    if (max_val <= 0.0f) max_val = 1.0f;
    if (current_val < 0.0f) current_val = 0.0f;
    if (current_val > max_val) current_val = max_val;

    const float pct = current_val / max_val;
    const float border = 2.0f;

    DrawCircleV(center, radius + border, GetColor(0x141414FF));

    DrawCircleV(center, radius, GetColor(0x2A2A2AFF));

    if (pct > 0.0f) {
        const float fill_height = radius * 2.0f * pct;

        const int scissor_x = (int) (center.x - radius);
        const int scissor_y = (int) (center.y + radius - fill_height);
        const int scissor_w = (int) (radius * 2.0f);
        const int scissor_h = (int) fill_height;

        BeginScissorMode(scissor_x, scissor_y, scissor_w, scissor_h);
        DrawCircleV(center, radius, fill_color);
        EndScissorMode();
    }

    DrawCircleLinesV(center, radius + border, GetColor(0x606060FF));
}

static void render_ui_draw_health_bar(const float current_hp, const float max_hp) {
    const float screen_w = (float) GetScreenWidth();
    const float screen_h = (float) GetScreenHeight();
    const float radius = screen_h * 0.065f;

    const Vector2 center = {
        .x = screen_w * 0.5f - screen_w * 0.2f,
        .y = screen_h - radius - screen_h * 0.025f
    };

    render_hud_draw_globe(center, radius, current_hp, max_hp, GetColor(0x8B0000FF));
}

static void render_hud_draw_power_bar(const float current_power, const float max_power) {
    const float screen_w = (float) GetScreenWidth();
    const float screen_h = (float) GetScreenHeight();
    const float radius = screen_h * 0.065f;

    const Vector2 center = {
        .x = screen_w * 0.5f + screen_w * 0.2f,
        .y = screen_h - radius - screen_h * 0.025f
    };

    render_hud_draw_globe(center, radius, current_power, max_power, GetColor(0x1034A6FF));
}

static void render_hud_draw_target_health_bar(const float current_hp, const float max_hp) {
    if (max_hp <= 0.0f) return;

    const float screen_w = (float) GetScreenWidth();
    const float screen_h = (float) GetScreenHeight();

    const float bar_w = screen_w * 0.35f;
    const float bar_h = screen_h * 0.028f;
    const float pos_x = (screen_w - bar_w) * 0.5f;
    const float pos_y = screen_h * 0.03f;

    const int font_size = (int) (bar_h * 0.6f);
    const int border = 2;

    float pct = current_hp / max_hp;
    if (pct < 0.0f) pct = 0.0f;
    if (pct > 1.0f) pct = 1.0f;

    DrawRectangle((int) pos_x - border, (int) pos_y - border,
                  (int) bar_w + (border * 2), (int) bar_h + (border * 2),
                  GetColor(0x141414FF));
    DrawRectangle((int) pos_x, (int) pos_y, (int) bar_w, (int) bar_h, GetColor(0x2A2A2AFF));

    const float fill_w = bar_w * pct;
    if (fill_w > 0.0f) {
        DrawRectangle((int) pos_x, (int) pos_y, (int) fill_w, (int) bar_h, GetColor(0xA31C1CFF));
    }

    DrawRectangleLines((int) pos_x - border, (int) pos_y - border,
                       (int) bar_w + (border * 2), (int) bar_h + (border * 2),
                       GetColor(0x606060FF));

    const char *hp_text = TextFormat("%d / %d", (int) current_hp, (int) max_hp);
    const int text_w = MeasureText(hp_text, font_size);

    DrawText(hp_text,
             (int) (pos_x + (bar_w - text_w) * 0.5f),
             (int) (pos_y + (bar_h - font_size) * 0.5f),
             font_size, WHITE);
}

static void render_ui(void) {
}

static void render_gui_combat_slot(const Vector2 center, const float size, const char *name,
                                   const float cooldown_timer) {
    const Rectangle rect = {
        .x = center.x - size * 0.5f,
        .y = center.y - size * 0.5f,
        .width = size,
        .height = size
    };

    const float roundness = 0.25f;
    const int segments = 4;
    const float border_thickness = 5.0f;

    const Color border_color = cooldown_timer <= 0.0f ? GetColor(0x2ECC71FF) : GetColor(0x606060FF);

    DrawRectangleRounded(rect, roundness, segments, border_color);

    const Rectangle inner_rect = {
        .x = rect.x + border_thickness,
        .y = rect.y + border_thickness,
        .width = rect.width - border_thickness * 2.0f,
        .height = rect.height - border_thickness * 2.0f
    };
    DrawRectangleRounded(inner_rect, roundness, segments, GetColor(0x2A2A2AFF));

    const int name_font_size = (int) (size * 0.20f);
    if (name && name[0] != '\0') {
        const int name_w = MeasureText(name, name_font_size);
        const int name_x = (int) (rect.x + (rect.width - name_w) * 0.5f);
        const int name_y = (int) (rect.y + rect.height * 0.32f - name_font_size * 0.5f);
        DrawText(name, name_x, name_y, name_font_size, WHITE);
    }

    if (cooldown_timer > 0.0f) {
        const int timer_font_size = (int) (size * 0.22f);
        char timer_str[16];
        snprintf(timer_str, sizeof(timer_str), "%.1fs", cooldown_timer);

        const int timer_w = MeasureText(timer_str, timer_font_size);
        const int timer_x = (int) (rect.x + (rect.width - timer_w) * 0.5f);
        const int timer_y = (int) (rect.y + rect.height * 0.68f - timer_font_size * 0.5f);
        DrawText(timer_str, timer_x, timer_y, timer_font_size, LIGHTGRAY);
    }
}

static void render_hud_combat_actions_hotbar(const float mp_cd, const float ms_cd, const float q_cd, const float w_cd,
                                             const float e_cd) {
    const float screen_w = (float) GetScreenWidth();
    const float screen_h = (float) GetScreenHeight();

    const float slot_size = screen_h * 0.075f;
    const float spacing = slot_size * 1.12f;

    const float center_y = screen_h - slot_size * 0.75f - screen_h * 0.025f;

    const float start_x = screen_w * 0.5f - (2.0f * spacing);

    const char *names[5] = {"MP", "MS", "Q", "W", "E"};

    const float cooldowns[5] = {mp_cd, ms_cd, q_cd, w_cd, e_cd};

    for (int i = 0; i < 5; i++) {
        const Vector2 center = {
            .x = start_x + (float) i * spacing,
            .y = center_y
        };

        render_gui_combat_slot(center, slot_size, names[i], cooldowns[i]);
    }
}

static void render_hud(void) {
    for (Entity i = 0; i < MAX_ENTITIES; ++i) {
        if (!HAS_COMPONENT(character, i)) continue;

        const Character *c = GET_COMPONENT(character, i);

        if (HAS_COMPONENT(player_tag, i)) {
            render_ui_draw_health_bar((float) c->health_points, (float) c->max_health_points);
            render_hud_draw_power_bar((float) c->power_points, (float) c->max_power_points);

            if (HAS_COMPONENT(combat_intent, i)) {
                const CombatIntent *ci = GET_COMPONENT(combat_intent, i);

                render_hud_combat_actions_hotbar(
                    ci->actions[COMBAT_SLOT_PRIMARY].cooldown_timer,
                    ci->actions[COMBAT_SLOT_SECONDARY].cooldown_timer,
                    ci->actions[COMBAT_SLOT_Q].cooldown_timer,
                    ci->actions[COMBAT_SLOT_W].cooldown_timer,
                    ci->actions[COMBAT_SLOT_E].cooldown_timer
                );

                if (ci->has_target_entity) {
                    const Character *tc = GET_COMPONENT(character, ci->target_entity);
                    render_hud_draw_target_health_bar((float) tc->health_points, (float) tc->max_health_points);
                }
            }
        }

        if (c->floating_text.render_intent && (HAS_COMPONENT(player_tag, i) || HAS_COMPONENT(enemy_tag, i))) {
            const float elapsed = c->floating_text.timer_default - c->floating_text.timer;

            const Vector3 pos_vec3 = {
                .x = c->floating_text.position.x,
                .y = c->floating_text.position.y + elapsed,
                .z = c->floating_text.position.z
            };

            render_floating_text(game_camera, pos_vec3, c->floating_text.text,
                                 c->floating_text.timer, WHITE, 30);
        }
    }
}

static Vector3 get_circle_world_center_3d(const Transform3D *t, const CircleShape circle) {
    if (circle.offset.x == 0.0f && circle.offset.z == 0.0f) {
        return (Vector3){.x = t->position.x, .y = t->position.y, .z = t->position.z};
    }

    const float rad = t->rotation.y * DEG2RAD;
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vector3){
        .x = t->position.x + (circle.offset.x * cos_a - circle.offset.z * sin_a),
        .y = t->position.y,
        .z = t->position.z + (circle.offset.x * sin_a + circle.offset.z * cos_a)
    };
}

static Vector3 get_box_world_center_3d(const Transform3D *t, const Collider3D *c) {
    if (c->offset.x == 0.0f && c->offset.z == 0.0f) {
        return (Vector3){.x = t->position.x, .y = t->position.y, .z = t->position.z};
    }

    const float rad = t->rotation.y * DEG2RAD;
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vector3){
        .x = t->position.x + (c->offset.x * cos_a - c->offset.z * sin_a),
        .y = t->position.y,
        .z = t->position.z + (c->offset.x * sin_a + c->offset.z * cos_a)
    };
}

void render_debug_colliders(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(transform, i) || !HAS_COMPONENT(collider, i)) continue;

        const Transform3D *t = GET_COMPONENT(transform, i);
        const Collider3D *c = GET_COMPONENT(collider, i);

        if (!c->is_active) continue;

        const Color color = c->is_trigger ? YELLOW : GREEN;

        if (c->type == COLLIDER_CIRCLE) {
            for (int j = 0; j < c->circle_count; j++) {
                const CircleShape *circle = &c->circles[j];
                const Vector3 world_center = get_circle_world_center_3d(t, *circle);
                const float scaled_radius = circle->radius;

                DrawCircle3D(world_center, scaled_radius, (Vector3){.x = 1.0f, .y = 0.0f, .z = 0.0f}, 90.0f, color);

                const float rad = t->rotation.y * DEG2RAD;
                const Vector3 forward_tip = {
                    .x = world_center.x + sinf(rad) * scaled_radius,
                    .y = world_center.y,
                    .z = world_center.z + cosf(rad) * scaled_radius
                };

                DrawLine3D(world_center, forward_tip, color);
                DrawSphereWires(world_center, 0.05f, 4, 4, color);
            }
        } else if (c->type == COLLIDER_BOX) {
            const Vector3 world_center = get_box_world_center_3d(t, c);

            const float half_w = c->half_extents.x;
            const float half_h = c->half_extents.y;
            const float half_d = c->half_extents.z;

            const Vector3 local_corners[4] = {
                {.x = -half_w, .y = 0.0f, .z = -half_d},
                {.x = half_w, .y = 0.0f, .z = -half_d},
                {.x = half_w, .y = 0.0f, .z = half_d},
                {.x = -half_w, .y = 0.0f, .z = half_d}
            };

            const float rad = t->rotation.y * DEG2RAD;
            const float cos_a = cosf(rad);
            const float sin_a = sinf(rad);

            Vector3 world_corners[4];
            for (int k = 0; k < 4; k++) {
                world_corners[k] = (Vector3){
                    .x = world_center.x + (local_corners[k].x * cos_a - local_corners[k].z * sin_a),
                    .y = world_center.y,
                    .z = world_center.z + (local_corners[k].x * sin_a + local_corners[k].z * cos_a)
                };
            }

            for (int k = 0; k < 4; k++) {
                const Vector3 p1 = world_corners[k];
                const Vector3 p2 = world_corners[(k + 1) % 4];
                DrawLine3D(p1, p2, color);
            }

            if (half_h > 0.0f) {
                const Vector3 size = {.x = half_w * 2.0f, .y = half_h * 2.0f, .z = half_d * 2.0f};
                DrawCubeWires(world_center, size.x, size.y, size.z, color);
            }
        }
    }
}

void render_system_update(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    if (scene_state == SCENE_STATE_MAIN_MENU) {
        render_main_menu_instructions();
    } else {
        BeginMode3D(game_camera);

        DrawGrid(100, 2.0f);

        for (Entity i = 0; i < MAX_ENTITIES; i++) {
            if (!HAS_COMPONENT(transform, i)) continue;

            const Transform3D *t = GET_COMPONENT(transform, i);
            const Vector3 position = (Vector3){.x = t->position.x, .y = t->position.y, .z = t->position.z};
            const Vector3 scale = (Vector3){.x = t->scale.x, .y = t->scale.y, .z = t->scale.z};

            Quaternion rotation_quat = (Quaternion){
                .x = t->rotation.x, .y = t->rotation.y, .z = t->rotation.z, .w = t->rotation.w
            };
            if (rotation_quat.x == 0 && rotation_quat.y == 0 && rotation_quat.z == 0 && rotation_quat.w == 0) {
                rotation_quat.w = 1.0f;
            }

            Vector3 rotation_axis = (Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
            float rotation_angle = 0.0f;
            QuaternionToAxisAngle(rotation_quat, &rotation_axis, &rotation_angle);

            if (HAS_COMPONENT(static_render_3d, i)) {
                const StaticRender3D *r = GET_COMPONENT(static_render_3d, i);
                const Model *model = raylib_model_manager_get(r->model_id);

                if (model) {
                    DrawModelEx(*model, position, rotation_axis, rotation_angle * RAD2DEG, scale, WHITE);
                }
            }

            if (HAS_COMPONENT(animated_render_3d, i)) {
                const AnimatedRender3D *r = GET_COMPONENT(animated_render_3d, i);
                const Model *model = raylib_model_manager_get(r->model_id);
                const ModelAnimation *anim_set = raylib_model_manager_get_animation(r->animation_set_id);

                if (model && anim_set) {
                    const ModelAnimation current_anim = anim_set[r->current_animation_in_set];
                    UpdateModelAnimation(*model, current_anim, r->animation_frame_counter);
                    DrawModelEx(*model, position, rotation_axis, rotation_angle * RAD2DEG, scale, WHITE);
                }
            }
        }

        if (RENDER_COLLIDERS) {
            render_debug_colliders();
        }

        EndMode3D();

        if (RENDER_FPS_COUNTER) {
            DrawFPS(10, 10);
        }

        render_ui();
        render_hud();
    }

    EndDrawing();
}
