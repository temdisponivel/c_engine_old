//
// Created by temdisponivel on 17/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_TIME_H
#define CYNICAL_ENGINE_CPP_TIME_H

#include "engine.h"
#include "collections.h"

typedef void (*timed_callback)(void *payload);

typedef struct timed_callback_config {
    timed_callback callback;
    void *payload;
    float desired_time;
    bool use_unscaled_time;
} timed_callback_config_t ;

typedef struct time_state {
    uint frame_index;
    float dt;
    float unscaled_dt;

    float time_scale;

    float time;
    float unscaled_time;

    double _start_frame_time;
    double _end_frame_time;

    list<timed_callback_config_t> *_callbacks;
} time_state_t;

void prepare_time();

void release_time();

time_state_t get_time_state();

float get_dt();

float get_unscaled_dt();

float get_time_scale();

float get_time();

float get_unscaled_time();

void time_start_frame();

void time_end_frame();

void call_after(float seconds, timed_callback callback, void *payload, bool use_unscaled_time);
void call_at(float seconds, timed_callback callback, void *payload, bool use_unscaled_time);

#endif //CYNICAL_ENGINE_CPP_TIME_H
