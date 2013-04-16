/*
 * pipeline.hpp
 *
 *  Created on: 2013. 4. 2.
 *      Author: jeongseok.kim
 */

#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <boost/shared_ptr.hpp>
//#include <gst/video/video.h>
//#include <gst/app/gstappsrc.h>
//#include <gst/app/gstappbuffer.h>
#include <glib.h>
#include <gst/gst.h>
#include <pipeline/pf_export.hpp>

typedef enum {
    GST_PLAY_FLAG_VIDEO         = 0x00000001,
    GST_PLAY_FLAG_AUDIO         = 0x00000002,
    GST_PLAY_FLAG_TEXT          = 0x00000004,
    GST_PLAY_FLAG_VIS           = 0x00000008,
    GST_PLAY_FLAG_SOFT_VOLUME   = 0x00000010,
    GST_PLAY_FLAG_NATIVE_AUDIO  = 0x00000020,
    GST_PLAY_FLAG_NATIVE_VIDEO  = 0x00000040,
    GST_PLAY_FLAG_DOWNLOAD      = 0x00000080,
    GST_PLAY_FLAG_BUFFERING     = 0x000000100
} GstPlayFlags;


namespace mediapipeline {

class Pipeline {

protected:
	Pipeline();

public:
	typedef boost::shared_ptr<Pipeline> bsp_t;

    enum State /* player status enum type */
    {
        StoppedState,
        PlayingState,
        PausedState
    };
    enum MediaStatus /* playing media status enum type */
    {
        UnknownMediaStatus,
        NoMedia,
        LoadingMedia,
        LoadedMedia,
        StalledMedia,
        BufferingMedia,
        BufferedMedia,
        EndOfMedia,
        InvalidMedia
    };
    enum Error /* player error enum type */
    {
        NoError,
        ResourceError,
        FormatError,
        NetworkError,
        AccessDeniedError,
        ServiceMissingError
    };

	virtual ~Pipeline();

	//virtual void load() = 0;
	virtual void load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType) = 0;
	virtual void load(MEDIA_CLIPOPT_T *clipOpt) = 0;
	virtual void unload() = 0;
	virtual void play(int rate = 1) = 0;

    virtual gint64 duration() const = 0;
    virtual gint64 position() const = 0;
    virtual gint volume() const = 0;
	virtual gboolean isMuted() const = 0;
	virtual gboolean isAudioAvailable() const = 0;
	virtual gboolean isVideoAvailable() const = 0;
	virtual gboolean isSeekable() const = 0;
	virtual gfloat playbackRate() const = 0;

	//virtual Error error() const;
	virtual GString errorString() const = 0;

	enum Error gstError;
	//enum MediaStatus MediaStatus;

	/*
	virtual void feed(unsigned char * data);
	virtual void stop();
	virtual void seek();
	virtual void pause();
	virtual void setProperty();
	virtual void getProperty();
	*/

	enum State m_gstPipelineState; /* gstreamer pipeline status */
    GstBus * m_busHandler;         /* gstreamer bus callback handler */
	GstElement* m_pipeHandle;      /* gstreamer pipeline handler */
	gint m_playbinVersion;         /* playbin version 1/2/3 */
    gboolean m_audioAvailable;     /* audio track available */
	gboolean m_videoAvailable;     /* video track available */
	gfloat m_playbackRate;         /* playback rate value */
	gint m_volume;                 /* for gstreamer local volume support */
	gboolean m_muted;
	gboolean m_seekable;           /* seekable stream */
	gint64 m_duration;             /* total stream duration */
	gint64 m_currentPosition;      /* current position time */
	gint m_durationQueryCount;     /* timeout duration query count */

};
}

#endif /* PIPELINE_HPP_ */
