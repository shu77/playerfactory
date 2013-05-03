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
//#include <gst/app/gstappbuffer.h>
#include <glib.h>
#include <gst/gst.h>
#include <pipeline/pf_export.hpp>
#include <pipeline/event/playeventlistener.hpp>
#include <gst/app/gstappsrc.h>
//#include <gst/app/gstappbuffer.h>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
//#include <glibmm-utils/glibmm-utils.h>
#include <pipeline/utils/options.hpp>

using namespace
    mediapipeline::utils;
using namespace std;

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MEDIAPIPE_BUFFER_SIZE (24*1024*1024)
#define DURATION_QUERY_MAX_NUM 20 //200ms x 20 =  4s
#define BITS_PER_BYTE 8
#define SOUP_HTTP_SRC_TIMEOUT 30 
#define BUFF_ADJ_MAX (5*1024*1024)

#define LMF_TO_LOWER(src)           \
  do{                   \
        for(int i = 0; i < strlen(src); i++)  \
          src[i] = tolower(src[i]); \
     }while(FALSE)
#define LMF_TITLE_MAX_LEN 512

typedef enum
{
  GST_PLAY_FLAG_VIDEO = 0x00000001,
  GST_PLAY_FLAG_AUDIO = 0x00000002,
  GST_PLAY_FLAG_TEXT = 0x00000004,
  GST_PLAY_FLAG_VIS = 0x00000008,
  GST_PLAY_FLAG_SOFT_VOLUME = 0x00000010,
  GST_PLAY_FLAG_NATIVE_AUDIO = 0x00000020,
  GST_PLAY_FLAG_NATIVE_VIDEO = 0x00000040,
  GST_PLAY_FLAG_DOWNLOAD = 0x00000080,
  GST_PLAY_FLAG_BUFFERING = 0x000000100
} GstPlayFlags;

namespace mediapipeline
{

class BufferController
{
protected:

public:
  typedef boost::shared_ptr < BufferController > bsp_t;
  ~BufferController ();
  BufferController ();

private:



};
}

namespace mediapipeline
{
class InformationHandler
{
protected:

public:
  typedef boost::shared_ptr < InformationHandler > bsp_t;
  ~InformationHandler ();
  InformationHandler ();

private:

};
}

namespace mediapipeline
{

class Pipeline
{

protected:
  Pipeline ();
  BufferController::bsp_t _bufferControl;
  void setBufferController (BufferController::bsp_t busController);
  Options::bsp_t _options;
  void setOptionsHandler (Options::bsp_t optionsHandler);
  InformationHandler::bsp_t _infomationHndl;
  void setInformationHandler (InformationHandler::bsp_t infomationHndl);
public:
  typedef boost::shared_ptr < Pipeline > bsp_t;

  enum State                  /* player status enum type */
  {
    StoppedState,
    PlayingState,
    PausedState,
    BufferingState,
    ErrorState
  };
  enum MediaStatus            /* playing media status enum type */
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
  enum Error                  /* player error enum type */
  {
    NoError,
    ResourceError,
    FormatError,
    NetworkError,
    AccessDeniedError,
    ServiceMissingError
  };

  virtual ~ Pipeline ();
  BufferController::bsp_t getBufferController ();
  Options::bsp_t getOptionsHandler ();
  InformationHandler::bsp_t getInformationHandler ();

  virtual gboolean loadSpi_pre () = 0;
  gboolean load (const std::string optionString);
  virtual gboolean loadSpi_post () = 0;
  gboolean unload ();
  virtual gboolean unloadSpi () = 0;
  gboolean play (int rate = 1);
  gboolean pause ();
  gboolean stop ();
  
  gboolean createSeekResource();
  gboolean releaseSeekResource();

  gboolean seek (gint64 ms);
  virtual gboolean seekSpi (gint64 ms){
    return seekCommon(ms);
  }
  gboolean seekCommon(gint64 ms);
  gboolean seekCommon_core (guint64 posMs, GstSeekFlags flag, GError **ppError); // common seek method. sub

  gboolean isReadyToPlay ();
  Pipeline::State getPendingPipelineState ();
  virtual gboolean isReadyToPlaySpi () = 0;

  //handle buscallback
  bool compareDouble (const double num1, const double num2);
  bool connectGstBusCallback ();
  gboolean disconnectGstBusCallback();

  void checkSupported (gpointer data);
  static void collectTags (const GstTagList * tag_list, const gchar * tag,
                           gpointer user_data);
  void handleBusTag (gpointer data, GstMessage * pMessage);
  gboolean handleBusPlayerMessage (gpointer data, GstMessage * pMessage);
  gboolean handleBusElementMessage (gpointer data, GstMessage * pMessage);
  gint64 getStreamLength (gpointer data);
  void handleBusDuration (gpointer data, GstMessage * pMessage);
  void handleBusStateMsg (gpointer data, GstMessage * pMessage);
  void handleBusApplication (gpointer data, GstMessage * pMessage);
  void handleBusEOS (gpointer data);
  void handleStateMsgPause (gpointer data, GstState oldState);
  void handleStateMsgPlay (gpointer data);

  static void handleVolumeChange (GObject * o, GParamSpec * p, gpointer d);
  static void handleMutedChange (GObject * o, GParamSpec * p, gpointer d);
  static gboolean gstBusCallbackHandle (GstBus * pBus, GstMessage * pMessage,
                                        gpointer data);
  //end buscallback

  //set seek,trick
  void setSeekable (gpointer data, bool seekable);
  void setSeekable (bool seekable);
  gfloat playbackRate () const;
  gboolean setPlaybackRate (gfloat rate);
  gboolean setPlaybackRate (gpointer data, gfloat rate);
  virtual gboolean setPlaybackRateSpi_pre (gpointer data, gfloat rate){
    return true;
  }
  virtual gboolean setServerSideTrick(gpointer data, gboolean serverSideTrick){
    return true;
  }

  //end seek,trick

  gint64 duration (gpointer data);
  gint64 duration ();
  gint64 position (gpointer data, gboolean bReadAgain);
  gint64 position (gboolean bReadAgain);

  virtual gboolean positionSpi(gpointer data, gint64 *pos) = 0;

  gint volume () const;
  gboolean isMuted () const;
  gboolean isAudioAvailable () const;
  gboolean isVideoAvailable () const;
  gboolean isSeekable ();
  gboolean isSeekable (gpointer data);

  /* notify state change */
  void pipelineEventNotify (gpointer data, MEDIA_CB_MSG_T msg);
  bool sendDuration(gint64 duration);
  bool sendPositionUpdate(gint64 currPosition);

  void stateChanged (Pipeline::State state);
  void seekableStateChanged (bool seekable);
  void playbackRateChanged (gfloat rate);
  virtual GString errorString () const = 0;
  enum Error gstError;

  Pipeline::State getPlayerState();
  void setPlayerState(Pipeline::State state);

  bool setGstreamerDebugLevel (guint select, gchar * category,
                                       GstDebugLevel level);
  /* start update information APIs */
  static gboolean updatePlayPosition(gpointer data);
  gboolean informationMonitorStop();
  gboolean informationMonitorStart(guint32 timeInterval);
  virtual gboolean informationMonitorStartSpi(guint32 timeInterval) = 0;
  static gboolean updateDuration(gpointer data);
  static gboolean updateBufferingInfo(gpointer data);
  gboolean updateBufferingInfoSub(gpointer data, GstMessage *pMessage, gint *pPercent);
  gboolean updateAsLive(gpointer data, gint *pPercent);
  gboolean setBufferProgress(gpointer data, int progress, int bufferedSec);
  virtual void setInterleavingTypeSpi(gpointer data, GstObject *pObj, gint stream, gpointer user_data)=0;
  virtual void getUndecodedSizeSpi(gpointer data, guint64* pVdecBufferedSize, guint64* pAdecBufferedSize)=0;
  void updateTags(gpointer data, GstTagList *pTagList);
  void handlePlayerMsg_AsyncDone(gpointer data);
  virtual void getStreamsInfoSpi(gpointer data){}
  virtual gboolean updateVideoInfoSpi(gpointer data){}
  virtual void handleStateMsgPauseSpi_pre(gpointer data){}
  //virtual void correctBufferedBytesSpi(gpointer data){} // TODO check!!!
  //virtual void videoDecodeUnderrunCbSpi(GstElement *pObj, gpointer data){}// TODO check!!!
  //virtual void audioDecodeUnderrunCbSpi(GstElement *pObj, gpointer data){}// TODO check!!!
  virtual gboolean checkTimeToDecodeSpi(gpointer data){
    return true;
  }
  /* start update information APIs */
  gboolean checkPendingEOS(gpointer data);
  gboolean isSeekableMedia(gpointer data);
  
  std::string m_uri;
  std::string m_subtitle_uri;
  /* common */
  enum State m_playertState;   /* user input control status */
  enum State m_gstPipelineState;      /* gstreamer pipeline status */
  enum State m_pendingState;  /* pending pipeline state */
  GstBus *m_busHandler;       /* gstreamer bus callback handler */
  gulong m_sigBusAsync;
  //GstBus                      *bus;
  //guint       m_tag; /* temp handle for bus  */
  GstElement *m_pipeHandle;   /* gstreamer pipeline handler */
  gint m_playbinVersion;      /* playbin version 1/2/3 */
  gboolean m_bAudioAvailable;  /* audio track available */
  gboolean m_bVideoAvailable;  /* video track available */
  gfloat m_playbackRate;      /* playback rate value */
  gint m_volume;              /* for gstreamer local volume support */
  gboolean m_muted;
  gboolean m_seekable;        /* seekable stream */
  gboolean m_bSeekableDuration;       /* check for live stream case */
  gboolean m_bSeekableIndex;  /* check for gst element not seekable msg */
  gint64 m_duration;          /* total stream duration - in nano sec */
  mutable gint64 m_currentPosition;   /* current position time - in nano sec */ 
  //gint64                      SessionPlayPositionStatic; // -> only custom pipeline
  //gint64                      SessionCurrentPTSStatic;   // -> only custom pipeline
  gint m_durationQueryCount;  /* timeout duration query count */
  gboolean m_blockByVideoSink;        /* blocked (pause,seek) by videosink */

  /* streaming info */
  gint64 m_InstantBps;
  gint m_BufBeginSec;
  gint m_BufEndSec;
  gint m_BufRemainSec;
  guint32 m_bufferedByte;
  gint m_BufPercent;
  gboolean m_bIsBufferFull;
  gint m_LastSeekPTS;
  gint64 m_bufferedBytesCorrection;   // A/V 틀어진 경우, 데이터 버퍼링이 되어도 A/V underrun 발생하는 경우 보정.
  guint m_MaxBitrate;
  guint m_AvgBitrate;
  gboolean m_bLiveStreaming;
  gboolean m_bAsfLive;
  gboolean m_bNeedToRestart;  // Error 발생 시 자동 restart
  /* for extra playbin event  */
  gboolean m_bPendingNotSupportedAudioMessage;        // demuxer에서 not supported audio 정보가 올라오거나 audio tag 에서 unknown으로 판별시.
  gboolean m_bSendNotSupportedAudioMessageAtPlayState;
  gboolean m_bPendingNotSupportedVideoMessage;        // demuxer에서 not supported video 정보가 올라오거나 video tag 에서 unknown으로 판별시.
  gboolean m_bSendNotSupportedVideoMessageAtPlayState;

  /* for control */
  gboolean m_bIsSeeking;
  //gdouble                     SessionPlaybackRate;
  gboolean m_bPlaybackStarted;        // TRUE after stopped->playing
  gboolean m_bPlaybackStopped;        // TRUE after playing->stopped : to stop monitoring
  gboolean m_bEndOfFile;      // EndOfFile 인 경우
  guint8 *pPendingAudioLanguage;
  gint pendingVideoAngleNum;  // multi video angle, pending angle number.
  //gchar                       isCheckPendingMultiTrackSetting;
  gboolean m_bPlayStatePreProcessingDone;

  //BOOLEAN bForward;           -> m_playbackRate
  //UINT8 speedInt;             -> m_playbackRate
  //UINT8 speedFrac;            -> m_playbackRate

  /* for source infomation */
  //gchar            m_source_title[512];         /**< Title of source */
  //gchar            m_source_artist[512];        /**< artist of source */
  //gchar            m_source_copyright[512];     /**< copyright of source */
  //gchar            m_source_album[512];         /**< album of source */
  //gchar            m_source_genre[512];         /**< genre of source */
  //guint16          m_source_titleSize;          /**< Size of title string */
  //TIME_T           m_source_date;               /**< Creation date of source */
  guint64 m_source_dataSize;                    /**< Total length of source */
  MEDIA_FORMAT_T m_source_format;               /**< Media format (container type) of source */
  MEDIA_CODEC_T m_source_codec;                 /**< Media Codec of source */
  guint16 m_source_width;                       /**< Width of source. (Not valid when audio ony) */
  guint16 m_source_height;                      /**< Height of source. (Not valid when audio ony) */
  gint32 m_source_durationMS;                   /**< Total duration in millisecond */
  gint32 m_source_targetBitrateBps;             /**< Needed average bitrate in bps (bits per second) */
  gboolean m_source_bIsValidDuration;           /**< durationMS가 유효한 값인지. (FALSE면 duration이 없는 경우 ex)live ) */
  gboolean m_source_bIsSeekable;                /**< HOA_MEDIA_SeekClip is available(TRUE) or not*/
  gboolean m_source_bIsScannable;               /**< HOA_MEDIA_SetPlaySpeed is available(TRUE) or not */

  //MEDIA_SRC_TYPE_T        srcType; // 재구조화 후, pipeline type 자체로 분기.

  /* for DLNA... gerneric player only */
  gboolean m_isDLNA;
  guint32 m_dlna_flagval;
  guint32 m_dlna_opval;
  guint64 m_dlna_duration;
  guint64 m_dlna_fileSize;    // player
  //guint64 m_dlna_duration;                // player

  /* DLNA INFO */
  //gchar                       pDLNAProtocolInfo[LMF_DLNA_PROTOCOL_LEN];
  //guint64                     DLNAFileSize;
  //guint64                     DLNACleartextSize;

  /* http header setting for cookies & user-agent */
  //gchar                       *cookies[LMF_HEADER_MAX_NUM+1];
  //gchar                       *userAgent;
  //gchar                       *extraHeader;
  //gchar                       *dlnaDeviceName;
  //guint                       SoupHttpSrcTimeOut;
  /* http header */
  //char *pHttpHeader[LMF_HEADER_MAX_NUM+1];

  /* for control */
  gboolean m_isDLNA_notSeekableContent;
  gboolean m_bPendingEOS;
  gboolean m_bServerSideTrick;        //m_isMmsURI;   /**< TRUE : server side trick mode */
  //  http url 에서 mms로 redirection 이 발생하는 경우가 있음.
  //TODO char                        *pNewUri;

  gint32 m_interleavingType;           /**< 3D type */

  //AF_BUFFER_HNDL_T    bufferHandle;      -----------> custom player only
  /* Feed Data Info */
  //LMF_FEED_BUFFER_STATE_T     feedState;    ----------> custom pipeline only
  //UINT64                      feededTotalSize; ----------> custom pipeline only
  //BOOLEAN                     feedEOS;        -----------> custom pipeline only
  // STATIC PIPELINE Global Variables
  //BOOLEAN                     bFeedPossible;   ----------> custom player only
  //SINT64                      seekDecodedPts[2]; --------> custom player only
  /* For MPEG-DASH */
  //char                        *pInitialStreamQualityPre; ----> mpeg dash pipeline only

  /* When seeking, queue up the seeks if they happen before
   * the previous one finished */
  GMutex *m_seek_mutex;
  GstClock *m_clock;
  GstClockTime m_seek_req_time;
  gint64 m_seek_time;
  /* 주기적 정보 업데이트를 위한 타이머 */
  guint32 m_positionTimerId;
  guint32 m_durationTimerId;
  guint32 m_bufferingTimerId;

  /* buffer control req info. */
  gboolean bUseBufferCtrl;    // if it is TRUE, the below time value is meaningful.
  gint32 bufferingMinTime;
  gint32 bufferingMaxTime;
  guint8 bufferMinPercent;
  guint8 bufferMaxPercent;

  /* pending seek for resume play */
  //gint64 pendingSeekPosition; // -> delete with 재구조화

  /* 버퍼링 상태에 따른 재생 컨트롤시 사용 */
  gint32 bufferProgress;
  gint32 prerollPercent;
  gint32 prerollSecond;

  gboolean bUserStop;                            /**< To distinguish function called stop and play end */

  PlayEventListener *m_eventHelper;   /* for listen gstreamer event and handle */
};

}



#endif                          /* PIPELINE_HPP_ */
