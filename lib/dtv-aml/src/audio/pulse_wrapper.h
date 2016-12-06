#ifndef foosimplehfoo
#define foosimplehfoo

#include <sys/types.h>
#include <pulse/sample.h>
#include <pulse/channelmap.h>
#include <pulse/def.h>
#include <pulse/cdecl.h>
#include <pulse/version.h>
#include <pulse/volume.h>

typedef struct pa_wrapper pa_wrapper;

pa_wrapper* pa_wrapper_new(
    const char *server,                 /**< Server name, or NULL for default */
    const char *name,                   /**< A descriptive name for this client (application name, ...) */
    pa_stream_direction_t dir,          /**< Open this stream for recording or playback? */
    const char *dev,                    /**< Sink (resp. source) name, or NULL for default */
    const char *stream_name,            /**< A descriptive name for this stream (application name, song title, ...) */
    const pa_sample_spec *ss,           /**< The sample type to use */
    const pa_channel_map *map,          /**< The channel map to use, or NULL for default */
    const pa_buffer_attr *attr,         /**< Buffering attributes, or NULL for default */
    int *error                          /**< A pointer where the error code is stored when the routine returns NULL. It is OK to pass NULL here. */
    );

/** Close and free the connection to the server. The connection object becomes invalid when this is called. */
void pa_wrapper_free(pa_wrapper *s);

/** Write some data to the server. */
int pa_wrapper_write(pa_wrapper *s, const void *data, size_t bytes, int *error);

/** Wait until all data already written is played by the daemon. */
int pa_wrapper_drain(pa_wrapper *s, int *error);

/** Read some data from the server. This function blocks until \a bytes amount
 * of data has been received from the server, or until an error occurs.
 * Returns a negative value on failure. */
int pa_wrapper_read(
    pa_wrapper *s, /**< The connection object. */
    void *data,   /**< A pointer to a buffer. */
    size_t bytes, /**< The number of bytes to read. */
    int *error
    /**< A pointer where the error code is stored when the function returns
     * a negative value. It is OK to pass NULL here. */
    );

/** Return the playback or record latency. */
pa_usec_t pa_wrapper_get_latency(pa_wrapper *s, int *error);

/** Flush the playback or record buffer. This discards any audio in the buffer. */
int pa_wrapper_flush(pa_wrapper *s, int *error);

int pa_wrapper_pause(pa_wrapper *p, int pause, int *rerror);
int pa_wrapper_setvolume(pa_wrapper *p, pa_cvolume *vol, int *rerror);

#endif

