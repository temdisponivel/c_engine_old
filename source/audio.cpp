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

bool stream_music_to_buff(music_t *music, uint buffer) {
    #define STREAM_BUFFER_SIZE 1024
    short stream_buffer[STREAM_BUFFER_SIZE];
    uint data_read_size = 0;

    // Read till end of buffer or end of file
    while (data_read_size < STREAM_BUFFER_SIZE) {
        int samples_read;
        samples_read = stb_vorbis_get_samples_short_interleaved(
                music->audio_data,
                music->info.channels,
                stream_buffer + data_read_size, // advance the buffer pointer to where it was last written
                STREAM_BUFFER_SIZE - data_read_size // try to get the remaining of the buffer worth of data
        );

        // End of file (or stream)
        if (samples_read == 0)
            break;
        else
            data_read_size = samples_read * music->info.channels;
    }

    if (data_read_size == 0)
        return false;
    else {
        music->samples_left -= data_read_size;

        alBufferData(
                buffer,
                music->info.format,
                stream_buffer,
                data_read_size * sizeof(short),
                music->info.sample_rate
        );
    }
}


void init_stream_source(audio_source_t *source) {
    ENSURE(source->type == SOURCE_MUSIC);

    music_t *music = source->music;
    
    stb_vorbis_seek_start(music->audio_data);

    uint samples = stb_vorbis_stream_length_in_samples(music->audio_data);
    music->samples_left = samples * music->info.channels;

    // Only buffer back buffer if the front was buffered
    if (stream_music_to_buff(music, music->front_buffer)) {
        alSourceQueueBuffers(source->handle, 1, &music->front_buffer);
        
        if (stream_music_to_buff(music, music->back_buffer)) {
            alSourceQueueBuffers(source->handle, 1, &music->back_buffer);
        }
    }
}

void update_music(audio_source_t *source) {
    int buffers_streamed;
    alGetSourcei(source->handle, AL_BUFFERS_PROCESSED, &buffers_streamed);

    // If the front buffer was already streamed
    if (buffers_streamed > 0) {

        uint back_buffer;
        alSourceUnqueueBuffers(source->handle, 1, &back_buffer);

        if (stream_music_to_buff(source->music, back_buffer)) {
            alSourceQueueBuffers(source->handle, 1, &back_buffer); // Put the buffer back into the queue
        } else {
            if (source->loop) {
                init_stream_source(source); // reset the stream because we should loop
            } else {
                source->state = SOURCE_STOPPED; // reach end
            }
        }
    }
}

void update_audio_source(audio_source_t *source) {
    glm::vec3 pos = source->position;

    source->volume = fmaxf(source->volume, 0);
    alSourcef(source->handle, AL_GAIN, source->volume);
    CHECK_AL_ERROR();

    source->pitch = fmaxf(source->pitch, 0);
    alSourcef(source->handle, AL_PITCH, source->pitch);
    CHECK_AL_ERROR();

    alSourcei(source->handle, AL_LOOPING, source->loop ? AL_TRUE : AL_FALSE);
    CHECK_AL_ERROR();

    alSource3f(source->handle, AL_POSITION, pos.x, pos.y, pos.z);
    CHECK_AL_ERROR();

    alSource3f(source->handle, AL_VELOCITY, 0, 0, 0); // TODO: add velocity?
    CHECK_AL_ERROR();

    alSource3f(source->handle, AL_DIRECTION, 0, 0, 0); // TODO: add orientation?
    CHECK_AL_ERROR();

    if (source->type == SOURCE_MUSIC)
        update_music(source);
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

AUDIO_FORMAT get_format(uint channels) {
    // TODO: handle more formats
    if (channels == 2)
        return AUDIO_STEREO;
    else
        return AUDIO_MONO;
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

    sound->info.channels = (uint) channels;
    sound->info.sample_rate = (uint) sample_rate;
    sound->sound_data = out_put;
    sound->data_length = (uint) (ogg_length * channels * sizeof(short));
    sound->file_content = file_content;
    sound->info.format = get_format(channels);

    // TODO: move this into a functions so we can update the sound's data
    alGenBuffers(1, &sound->handle);
    alBufferData(sound->handle, sound->info.format, sound->sound_data, sound->data_length, sound->info.sample_rate);

    // NOTE: in theory we can safely delete the file content here, since it's been uploaded to openal

    return sound;
}

void destroy_sound(sound_t *sound) {
    alDeleteBuffers(1, &sound->handle);
    free_file_content(sound->file_content);
    memfree(sound->sound_data);
    memfree(sound);
}

music_t *create_music(const char *file_path) {
    uint buffers[2];
    alGenBuffers(2, buffers);

    uint front_buffer = buffers[0];
    uint back_buffer = buffers[1];

    #define ALLOC_BUFFER_SIZE 1024

    stb_vorbis_alloc *alloc_buffer = (stb_vorbis_alloc *) memalloc(sizeof(stb_vorbis_alloc));
    alloc_buffer->alloc_buffer = (char *) memalloc(ALLOC_BUFFER_SIZE); // 1 KB TODO: is this enough?!
    alloc_buffer->alloc_buffer_length_in_bytes = ALLOC_BUFFER_SIZE;

    int error;
    stb_vorbis *data = stb_vorbis_open_filename(file_path, &error, alloc_buffer);
    ENSURE(data != null);
    stb_vorbis_info info = stb_vorbis_get_info(data);

    music_t *music = (music_t *) memalloc(sizeof(music_t));

    music->audio_data = data;
    music->alloc_buffer = alloc_buffer;
    music->front_buffer = front_buffer;
    music->back_buffer = back_buffer;
    music->info.sample_rate = info.sample_rate;
    music->info.channels = (uint) info.channels;
    music->info.format = get_format((uint) info.channels);

    return music;
}

void destroy_music(music_t *music) {
    stb_vorbis_close(music->audio_data);

    memfree(music->alloc_buffer->alloc_buffer);
    memfree(music->alloc_buffer);

    uint buffers[2] = {music->front_buffer, music->back_buffer};
    glDeleteBuffers(2, buffers);
}

audio_source_t *create_audio_source() {
    audio_source_t *source = (audio_source_t *) memalloc(sizeof(audio_source_t));

    alGenSources(1, &source->handle);

    source->state = SOURCE_STOPPED;
    source->position = glm::vec3(0, 0, 0);
    source->volume = 1;
    source->pitch = 1;

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
        if (source->type == SOURCE_MUSIC) {
            init_stream_source(source);
        }

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

void set_music_on_source(audio_source_t *source, music_t *music) {
    source->type = SOURCE_MUSIC;
    source->music = music;

    init_stream_source(source);
}

// TODO: make play_music