#pragma once

#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef DEG2RAD
#define DEG2RAD (PI/180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f/PI)
#endif

#define LERP(a, b, t) ((a) + ((b) - (a)) * (t))

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} Quat;

static inline float math_clamp(const float value, const float min, const float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline float vec3_length(const Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Vec3 vec3_normalize(const Vec3 v) {
    const float length = vec3_length(v);
    if (length > 0.00001f) {
        const float i_length = 1.0f / length;
        return (Vec3){.x = v.x * i_length, .y = v.y * i_length, .z = v.z * i_length};
    }

    return (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
}

static inline Vec3 quat_rotate_vec3(const Quat q, const Vec3 v) {
    const Vec3 qv = {.x = q.x, .y = q.y, .z = q.z};
    const Vec3 u = {
        .x = qv.y * v.z - qv.z * v.y,
        .y = qv.z * v.x - qv.x * v.z,
        .z = qv.x * v.y - qv.y * v.x
    };
    const Vec3 u2 = {
        .x = qv.y * u.z - qv.z * u.y,
        .y = qv.z * u.x - qv.x * u.z,
        .z = qv.x * u.y - qv.y * u.x
    };
    return (Vec3){
        .x = v.x + 2.0f * (q.w * u.x + u2.x),
        .y = v.y + 2.0f * (q.w * u.y + u2.y),
        .z = v.z + 2.0f * (q.w * u.z + u2.z)
    };
}

static inline float quat_get_yaw_rad(Quat q) {
    const float siny_cosp = 2.0f * (q.w * q.y + q.z * q.x);
    const float cosy_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    return atan2f(siny_cosp, cosy_cosp);
}
