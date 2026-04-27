#pragma once

#include "raymath.h"
#include <cmath>

namespace Vec {

// -------------------------
// Basic helpers
// -------------------------

inline Vector2 Add(Vector2 a, Vector2 b) {
    return { a.x + b.x, a.y + b.y };
}

inline Vector2 Sub(Vector2 a, Vector2 b) {
    return { a.x - b.x, a.y - b.y };
}

inline Vector2 Scale(Vector2 v, float s) {
    return { v.x * s, v.y * s };
}

// -------------------------
// Component-wise operations
// -------------------------

inline Vector2 Max(Vector2 a, Vector2 b) {
    return {
        fmaxf(a.x, b.x),
        fmaxf(a.y, b.y)
    };
}

inline Vector2 Min(Vector2 a, Vector2 b) {
    return {
        fminf(a.x, b.x),
        fminf(a.y, b.y)
    };
}

inline Vector2 Clamp(Vector2 v, Vector2 min, Vector2 max) {
    return {
        fminf(fmaxf(v.x, min.x), max.x),
        fminf(fmaxf(v.y, min.y), max.y)
    };
}

// -------------------------
// Movement / physics helpers
// -------------------------

// Move vector toward zero (component-wise) assumes amount is unit vector of v
inline Vector2 ApproachZero(Vector2 v, Vector2 vel_unit_vec) {
    // X
    if (v.x > 0.0f) {
        v.x = fmaxf(0.0f, v.x - vel_unit_vec.x);
    } else if (v.x < 0.0f) {
        v.x = fminf(0.0f, v.x - vel_unit_vec.x);
    }

    // Y
    if (v.y > 0.0f) {
        v.y = fmaxf(0.0f, v.y - vel_unit_vec.y);
    } else if (v.y < 0.0f) {
        v.y = fminf(0.0f, v.y - vel_unit_vec.y);
    }

    return v;
}

// Move vector toward zero (length-based, better feel)
inline Vector2 ApproachZeroSmooth(Vector2 v, float amount) {
    float len = Vector2Length(v);
    if (len <= amount) return {0.0f, 0.0f};

    return Vector2Scale(v, (len - amount) / len);
}

// Clamp vector to max length
inline Vector2 ClampLength(Vector2 v, float maxLen) {
    float len = Vector2Length(v);
    if (len > maxLen) {
        return Vector2Scale(v, maxLen / len);
    }
    return v;
}

// Normalize safely (avoid divide by zero)
inline Vector2 NormalizeSafe(Vector2 v) {
    float len = Vector2Length(v);
    if (len == 0.0f) return {0.0f, 0.0f};
    return Vector2Scale(v, 1.0f / len);
}

// -------------------------
// Utility
// -------------------------

inline float Dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

inline float Distance(Vector2 a, Vector2 b) {
    return Vector2Length(Sub(a, b));
}

} // namespace Vec