#include "raylib.h"
#include "../../core/input_bridge.h"

InputBridge input_bridge = {0};

void input_system_poll(void) {
    const Vector2 mouse_pos = GetMousePosition();
    input_bridge.mouse_position = (Vec3){.x = mouse_pos.x, .y = mouse_pos.y, .z = 0.0f};

    input_bridge.action_primary |= IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input_bridge.action_secondary |= IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

    input_bridge.action_q |= IsKeyPressed(KEY_Q);
    input_bridge.action_w |= IsKeyPressed(KEY_W);
    input_bridge.action_e |= IsKeyPressed(KEY_E);

    input_bridge.action_p |= IsKeyPressed(KEY_P);
    input_bridge.action_r |= IsKeyPressed(KEY_R);
}
