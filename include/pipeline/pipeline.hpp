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
#include <pipeline/event/playeventlistener.hpp>
#include <gst/app/gstappsrc.h>
//#include <gst/app/gstappbuffer.h>
//#include <gst/app/gstappsrc.h>


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
	virtual bool load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType) = 0;
	virtual bool load(MEDIA_CLIPOPT_T *clipOpt) = 0;
	virtual void unload() = 0;
	virtual bool play(int rate = 1) = 0;
	virtual bool pause() = 0;
	virtual void stop() = 0;
    virtual bool seek(gint64 ms) = 0;

    virtual void setSeekable(bool seekable) = 0;

    virtual gint64 duration() const = 0;
    virtual gint64 position() const = 0;
    virtual gint volume() const = 0;
	virtual gboolean isMuted() const = 0;
	virtual gboolean isAudioAvailable() const = 0;
	virtual gboolean isVideoAvailable() const = 0;
	virtual gboolean isSeekable() const = 0;
	virtual gfloat playbackRate() const = 0;

	/* notify state change */
    void stateChanged(Pipeline::State state);
	void seekableStateChanged(bool seekable);
	void playbackRateChanged(gfloat rate);
	//virtual Error error() const;
	virtual GString errorString() const = 0;

	enum Error gstError;
	//enum MediaStatus MediaStatus;
	virtual bool setGstreamerDebugLevel(guint select, gchar *category, GstDebugLevel level) = 0;


	/*
	virtual void feed(unsigned char * data);
	virtual void stop();
	virtual void seek();
	virtual void pause();
	virtual void setProperty();
	virtual void getProperty();
	*/
    std::string m_uri;
    std::string m_subtitle_uri;
    
	enum State m_gstPipelineState; /* gstreamer pipeline status */
	enum State m_pendingState;     /* pending pipeline state */
    GstBus * m_busHandler;         /* gstreamer bus callback handler */
    gulong m_sigBusAsync;
    //GstBus                      *bus;
    guint       m_tag; /* temp handle for bus  */

	GstElement* m_pipeHandle;      /* gstreamer pipeline handler */
	gint m_playbinVersion;         /* playbin version 1/2/3 */
    gboolean m_audioAvailable;     /* audio track available */
	gboolean m_videoAvailable;     /* video track available */
	gfloat m_playbackRate;         /* playback rate value */
	gint m_volume;                 /* for gstreamer local volume support */
	gboolean m_muted;
	gboolean m_seekable;           /* seekable stream */
	gint64 m_duration;             /* total stream duration */
	mutable gint64 m_currentPosition;      /* current position time */
	gint m_durationQueryCount;     /* timeout duration query count */
    gboolean m_blockByVideoSink;   /* blocked (pause,seek) by videosink */

   /* streaming info */
    gint64                      m_InstantBps;
    gint                        m_BufBeginSec;
    gint                        m_BufEndSec;
    gint                        m_BufRemainSec;
    guint32                     m_bufferedByte;
    gint                        m_BufPercent;
    gboolean                    m_bIsBufferFull;
    gint                        m_LastSeekPTS;
    gint64                      m_bufferedBytesCorrection;  // A/V 틀어진 경우, 데이터 버퍼링이 되어도 A/V underrun 발생하는 경우 보정.
    guint                       m_MaxBitrate;
    guint                       m_AvgBitrate;
    gboolean                    m_bLiveStreaming;
    gboolean                    m_bAsfLive;
    gboolean                    m_bNeedToRestart; // Error 발생 시 자동 restart

    gboolean                    m_bPendingNotSupportedAudioMessage; // demuxer에서 not supported audio 정보가 올라오거나 audio tag 에서 unknown으로 판별시  저장하고, 적정 time에 UI로 올린다.
    gboolean                    m_bSendNotSupportedAudioMessageAtPlayState; // UI에 not support audio message 보냄.
    gboolean                    m_bPendingNotSupportedVideoMessage; // demuxer에서 not supported video 정보가 올라오거나 video tag 에서 unknown으로 판별시 저장하고, 적정 time에 UI로 올린다.
    gboolean                    m_bSendNotSupportedVideoMessageAtPlayState; // UI에 not support video message 보냄.

    gboolean                    m_bIsSeeking;
    //gdouble                     SessionPlaybackRate;
    gboolean                    m_bPlaybackStarted; // TRUE after stopped->playing
    gboolean                    m_bPlaybackStopped; // TRUE after playing->stopped : to stop monitoring
    gboolean                    m_bEndOfFile;     // EndOfFile 인 경우



    PlayEventListener *m_eventHelper; /* for listen gstreamer event and handle */
};
}

#endif /* PIPELINE_HPP_ */
