//
// Created by temdisponivel on 17/02/2018.
//

#include <utilities.h>
#include "audio.h"

static audio_state_t *audio_state;

audio_state_t get_audio_state() {
    return *audio_state;
}

void prepare_audio() {

    ALCdevice *device = alcOpenDevice(null);
    ENSURE(device != null);
    CHECK_AL_ERROR();

    ALCcontext *context = alcCreateContext(device, null);
    ENSURE(context != null);
    CHECK_AL_ERROR();

    alcMakeContextCurrent(context);
    CHECK_AL_ERROR();

    audio_state = (audio_state_t *) memalloc(sizeof(audio_state_t));
    audio_state->volume_scale = 1;
    audio_state->listener_trans = create_transform();
    audio_state->all_audio_sources = create_list<audio_source_t *>(10);

    audio_state->device = device;
    audio_state->context = context;
}

void release_audio() {
    for (int i = 0; i < audio_state->all_audio_sources->length; ++i) {
        audio_source_t *source = audio_state->all_audio_sources->items[i];
        destroy_source(source);
    }

    destroy_list(audio_state->all_audio_sources);
    memfree(audio_state);
}

void update_listener() {
    glm::vec3 pos = audio_state->listener_trans->position;

    alListenerf(AL_GAIN, audio_state->volume_scale);
    CHECK_AL_ERROR();

    alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
    CHECK_AL_ERROR();

    alListener3f(AL_VELOCITY, pos.x, pos.y, pos.z);
    CHECK_AL_ERROR();

    glm::vec3 forward = get_forward(audio_state->listener_trans);
    glm::vec3 up = get_up(audio_state->listener_trans);

    float values[6];
    values[1] = forward.x;
    values[2] = forward.y;
    values[3] = forward.z;

    values[1] = up.x;
    values[2] = up.y;
    values[3] = up.z;

    alListenerfv(AL_ORIENTATION, values);
    CHECK_AL_ERROR();
}

void update_audio_source(audio_source_t *source) {
    glm::vec3 pos = source->position;

    alSourcef(source->handle, AL_GAIN, source->volume);
    CHECK_AL_ERROR();

    alSourcef(source->handle, AL_PITCH, source->pitch);
    CHECK_AL_ERROR();

    alSource3f(source->handle, AL_POSITION, pos.x, pos.y, pos.z);
    CHECK_AL_ERROR();

    alSource3f(source->handle, AL_VELOCITY, pos.x, pos.y, pos.z); // TODO: add velocity?
    CHECK_AL_ERROR();

    alSource3f(source->handle, AL_DIRECTION, pos.x, pos.y, pos.z); // TODO: add orientation?
    CHECK_AL_ERROR();

    // TODO: maybe we need to update the source on al?!
}

void update_audio() {
    update_listener();

    list<audio_source_t *> *sources = audio_state->all_audio_sources;
    uint len = sources->length;

    for (int i = 0; i < len; ++i) {
        audio_source_t *source = sources->items[i];
        update_audio_source(source);
    }
}

sound_t *create_sound(const char *file_path) {
    long length;
    char *file_content = read_file_content(file_path, &length, false);
    ENSURE(file_content != null);

    int channels;
    int sample_rate;
    short *out_put;
    int ogg_length = stb_vorbis_decode_memory((ubyte *) file_content, length, &channels, &sample_rate, &out_put);
    ENSURE(ogg_length > 0);

    sound_t *sound = (sound_t *) memalloc(sizeof(sound_t));

    sound->channels = (uint) channels;
    sound->sample_rate = (uint) sample_rate;
    sound->sound_data = out_put;
    sound->data_length = (uint) (ogg_length * channels * sizeof(short));
    sound->file_content = file_content;

    // TODO: handle more formats
    if (channels == 2)
        sound->format = AUDIO_STEREO;
    else
        sound->format = AUDIO_MONO;

    // TODO: move this into a functions so we can update the sound's data
    alGenBuffers(1, &sound->handle);
    alBufferData(sound->handle, sound->format, sound->sound_data, sound->data_length, sound->sample_rate);

    // NOTE: in theory we can safely delete the file content here, since it's been uploaded to openal

    return sound;
}

void destroy_sound(sound_t *sound) {
    alDeleteBuffers(1, &sound->handle);
    free_file_content(sound->file_content);
    memfree(sound->sound_data);
    memfree(sound);
}

audio_source_t *create_audio_source() {
    audio_source_t *source = (audio_source_t *) memalloc(sizeof(audio_source_t));

    alGenSources(1, &source->handle);

    source->state = SOURCE_STOPPED;
    source->position = glm::vec3(0, 0, 0);
    source->volume = 1;

    add(audio_state->all_audio_sources, source);

    return source;
}

void destroy_source(audio_source_t *source) {
    remove(audio_state->all_audio_sources, source);
    alDeleteSources(1, &source->handle);
    memfree(source);
}

void set_audio_scale(float audio_scale) {
    audio_state->volume_scale = audio_scale;
    alListenerf(AL_GAIN, audio_scale);
}

void start_audio_source(audio_source_t *source) {
    if (source->state != SOURCE_PLAYING) {
        alSourcePlay(source->handle);
        source->state = SOURCE_PLAYING;
    }
}

void stop_audio_source(audio_source_t *source) {
    if (source->state == SOURCE_STOPPED) {
        alSourceStop(source->handle);
        source->state = SOURCE_STOPPED;
    }
}

void pause_audio_source(audio_source_t *source) {
    if (source->state == SOURCE_PLAYING) {
        alSourcePause(source->handle);
        source->state = SOURCE_PAUSED;
    }
}

void set_sound_on_source(audio_source_t *source, sound_t *sound) {
    source->type = SOURCE_SOUND;
    source->sound = sound;

    alSourcei(source->handle, AL_BUFFER, sound->handle);
}

// TODO: make play_music