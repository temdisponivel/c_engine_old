//
// Created by temdisponivel on 17/02/2018.
//

#include "time.h"

static time_state_t *time_state;

void prepare_time() {
    time_state = (time_state_t *) memalloc(sizeof(time_state_t));
    memset(time_state, 0, sizeof(time_state_t));
    time_state->time_scale = 1;

    time_state->_callbacks = create_list<timed_callback_config_t>(5);
}

void release_time() {
    memfree(time_state);
}

time_state_t get_time_state() {
    return *time_state;
}

float get_dt() {
    return time_state->dt;
}

float get_unscaled_dt() {
    return time_state->unscaled_dt;
}

float get_time_scale() {
    return time_state->time_scale;
}

float get_time() {
    return time_state->time;
}

float get_unscaled_time() {
    return time_state->unscaled_time;
}

void time_start_frame() {
    ++time_state->frame_index;
    time_state->_start_frame_time = glfwGetTime();

    list<timed_callback_config_t> *callbacks = time_state->_callbacks;
    uint length = callbacks->length;
    for (uint i = 0; i < length; ++i) {
        timed_callback_config_t config = callbacks->items[i];

        float time = 0;
        if (config.use_unscaled_time)
            time = get_unscaled_time();
        else
            time = get_time();

        if (time >= config.desired_time) {
            config.callback(config.payload);
            remove_at(callbacks, i);
        }
    }
}

void time_end_frame() {
    time_state->_end_frame_time = glfwGetTime();
    time_state->unscaled_dt = (float) (time_state->_end_frame_time - time_state->_start_frame_time);
    time_state->dt = time_state->unscaled_dt * time_state->time_scale;
    time_state->time += time_state->dt;
    time_state->unscaled_time += time_state->unscaled_dt;
}

void call_after(float seconds, timed_callback callback, void *payload, bool use_unscaled_time) {
    float desired_time = 0;
    if (use_unscaled_time)
        desired_time = get_unscaled_time() + seconds;
    else
        desired_time = get_time() + seconds;

    call_at(desired_time, callback, payload, use_unscaled_time);
}

void call_at(float seconds, timed_callback callback, void *payload, bool use_unscaled_time) {
    ENSURE(callback != null);

    timed_callback_config_t config;
    config.desired_time = seconds;
    config.callback = callback;
    config.payload = payload;
    config.use_unscaled_time = use_unscaled_time;

    add(time_state->_callbacks, config);
}