/**
 * Based on the implementation in Klipper [https://github.com/Klipper3d/klipper].
 * Copyright (C) Kevin O'Connor <kevin@koconnor.net>
 *
 * Our implementation takes inspiration from the work of Kevin O'Connor <kevin@koconnor.net> for Klipper
 * in used data structures, and some computations.
 */
#pragma once
#include "common.hpp"
#include "cmath_ext.h"

// #define FAIL_ON_NEGATIVE_STEP_TIME
// #define ISR_DEADLINE_DEBUGGING // Enable audible warnings on step deadline misses
// #define ISR_DEADLINE_TRACKING // Accurate (but expensive) deadline miss tracking
// #define ISR_EVENT_DEBUGGING // Enable audible warnings on event queue misses

constexpr const double EPSILON = 0.000000001;
constexpr const float EPSILON_FLOAT = 0.0000001f;

constexpr const double MAX_PRINT_TIME = 10000000.;

FORCE_INLINE double calc_distance(const double start_v, const double half_accel, const double move_time) {
    return (start_v + half_accel * move_time) * move_time;
}

FORCE_INLINE xyze_double_t calc_position(const double start_v, const double half_accel, const double move_time, const xyze_double_t start_pos, const xyze_double_t axes_r) {
    const double distance = calc_distance(start_v, half_accel, move_time);
    const xyze_double_t end_pos = start_pos + xyze_double_t({ axes_r.x * distance, axes_r.y * distance, axes_r.z * distance, axes_r.e * distance });
    return end_pos;
}

FORCE_INLINE xyze_double_t calc_end_position_move(const move_t *move) {
    return calc_position(move->start_v, move->half_accel, move->move_t, move->start_pos, move->axes_r);
}

FORCE_INLINE float fast_sqrt(float in) {
    float out;
#if defined(__GNUC__) && defined(__VFP_FP__) && !defined(__SOFTFP__)
    asm("vsqrt.f32 %0,%1"
        : "=t"(out)
        : "t"(in));
#else
    out = sqrtf(in);
#endif
    return out;
}

// Calculates time that will take to travel the specified distance.
// Step_dir determines which solution of the quadratic equation we will choose.
FORCE_INLINE float calc_time_for_distance(const float start_velocity, const float acceleration, const float distance, const bool step_dir) {
    if (acceleration == 0.f) {
        if (start_velocity != 0.f)
            return distance / start_velocity;
        else
            return NAN;
    }

    // Preventing computing sqrt from negative numbers caused by numerical issues.
    const float sqr = 2.f * acceleration * distance + SQR(start_velocity);
    if (sqr < 0.f && sqr >= -EPSILON_FLOAT)
        return -(start_velocity / acceleration);

    if (step_dir)
        return (fast_sqrt(sqr) - start_velocity) / acceleration;
    else
        return (-fast_sqrt(sqr) - start_velocity) / acceleration;
}

// Calculates time that will take to travel the specified distance.
FORCE_INLINE float calc_time_for_distance_block(const float start_velocity, const float acceleration, const float distance) {
    if (acceleration == 0.f)
        return distance / start_velocity;

    // Preventing computing sqrt from negative numbers caused by numerical issues.
    if (const float sqr = 2.f * acceleration * distance + SQR(start_velocity); sqr < 0.f)
        return -start_velocity / acceleration;
    else
        return (fast_sqrt(sqr) - start_velocity) / acceleration;
}

FORCE_INLINE double get_move_half_accel(const move_t &move, const int axis) {
    return move.half_accel * move.axes_r[axis];
}

FORCE_INLINE double get_move_start_v(const move_t &move, const int axis) {
    return move.start_v * move.axes_r[axis];
}

FORCE_INLINE double get_move_end_v(const move_t &move, const int axis) {
    return (move.start_v + 2. * move.half_accel * move.move_t) * move.axes_r[axis];
}

FORCE_INLINE double get_move_start_pos(const move_t &move, const int axis) {
    return move.start_pos[axis];
}

// True - Positive direction
// False - Negative direction
FORCE_INLINE int get_move_step_dir(const move_t &move, const int axis) {
    return !(move.flags & (MOVE_FLAG_X_DIR << axis));
}

// Reset all step_event that are equal to std::numeric_limits<float>::max() to zero.
// std::numeric_limits<float>::max() indicates that all step events for the given move and its axis were already generated.
FORCE_INLINE void step_generator_state_restart(step_generator_state_t &step_generator_state) {
    for (step_event_info_t &step_event_info : step_generator_state.step_events)
        if (step_event_info.time == std::numeric_limits<double>::max())
            step_event_info.time = 0;
}

// Find the next the nearest step event index.
// Current implementation is iteration through all axis, so it could be better to have a tiny linked list of sorted step events
// and iterate through this linked list. It could be much faster in most cases.
FORCE_INLINE void step_generator_state_update_nearest_idx(step_generator_state_t &step_generator_state, double next_step_event_time) {
    for (int idx = 0; idx < 4; ++idx)
        if (step_generator_state.step_events[idx].time < next_step_event_time) {
            next_step_event_time = step_generator_state.step_events[idx].time;
            step_generator_state.nearest_step_event_idx = idx;
        }
}

// Returns if all generators reach the end of the move queue and are unable to generate next-step events
// that means that the step queue will start draining.
FORCE_INLINE bool has_all_generators_reached_end_of_move_queue(const step_generator_state_t &step_generator_state) {
    bool all_generators_reached_end_of_move_queue = true;
    for (const basic_step_generator_t *step_generator : step_generator_state.step_generator)
        all_generators_reached_end_of_move_queue &= step_generator->reached_end_of_move_queue;

    return all_generators_reached_end_of_move_queue;
}

// Reset reached_end_of_move_queue flag for all step event generators.
FORCE_INLINE void reset_reached_end_of_move_queue_flag(const step_generator_state_t &step_generator_state) {
    for (basic_step_generator_t *step_generator : step_generator_state.step_generator)
        step_generator->reached_end_of_move_queue = false;
}

FORCE_INLINE constexpr bool is_active_x_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_X_ACTIVE);
}

FORCE_INLINE constexpr bool is_active_y_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_Y_ACTIVE);
}

FORCE_INLINE constexpr bool is_active_z_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_Z_ACTIVE);
}

FORCE_INLINE constexpr bool is_active_e_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_E_ACTIVE);
}

FORCE_INLINE constexpr bool get_dir_x_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_X_DIR);
}

FORCE_INLINE constexpr bool get_dir_y_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_Y_DIR);
}

FORCE_INLINE constexpr bool get_dir_z_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_Z_DIR);
}

FORCE_INLINE constexpr bool get_dir_e_axis(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_E_DIR);
}

FORCE_INLINE constexpr bool is_beginning_empty_move(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_BEGINNING_EMPTY_MOVE);
}

FORCE_INLINE constexpr bool is_ending_empty_move(const move_t &move) {
    return bool(move.flags & MOVE_FLAG_ENDING_EMPTY_MOVE);
}
