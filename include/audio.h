//
// Created by temdisponivel on 17/02/2018.
//

#ifndef CYNICAL_ENGINE_CPP_AUDIO_H
#define CYNICAL_ENGINE_CPP_AUDIO_H

#include "engine.h"
#include "entities.h"
#include "collections.h"

enum AUDIO_SOURCE_STATE {
    SOURCE_PLAYING,
    SOURCE_PAUSED,
    SOURCE_STOPPED,
};

enum AUDIO_TYPE {
    SOURCE_SOUND,
    SOURCE_MUSIC
};

enum AUDIO_FORMAT {
    AUDIO_MONO = AL_FORMAT_MONO16,
    AUDIO_STEREO = AL_FORMAT_STEREO16,
};

typedef struct sound {
    uint handle;

    short *sound_data;
    uint data_length;
    AUDIO_FORMAT format;
    uint channels;
    uint sample_rate;

    char *file_content;
} sound_t;

typedef struct music {

} music_t;

typedef struct audio_source {
    uint handle;
    glm::vec3 position;
    float volume;
    float pitch;
    bool loop;
    AUDIO_SOURCE_STATE state;

    AUDIO_TYPE type;
    union {
        sound_t *sound;
        music_t *music;
    };
} audio_source_t;

typedef struct audio_state {
    transform_t *listener_trans;
    float volume_scale;
    list<audio_source_t *> *all_audio_sources;

    ALCdevice *device;
    ALCcontext *context;
} audio_state_t;

audio_state_t get_audio_state();

void prepare_audio();

void release_audio();

void update_audio();

sound_t *create_sound(const char *file_path);

void destroy_sound(sound_t *sound);

audio_source_t *create_audio_source();

void destroy_source(audio_source_t *source);

void set_audio_scale(float audio_scale);

void start_audio_source(audio_source_t *source);

void stop_audio_source(audio_source_t *source);

void pause_audio_source(audio_source_t *source);

void set_sound_on_source(audio_source_t *source, sound_t *sound);

#endif //CYNICAL_ENGINE_CPP_AUDIO_H
