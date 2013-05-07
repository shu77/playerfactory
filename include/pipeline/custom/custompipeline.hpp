/*
 * custompipeline.hpp
 *
 *  Created on: 2013. 4. 22.
 *
 */

#ifndef CUSTOMPIPELINE_HPP_
#define CUSTOMPIPELINE_HPP_

#include <pipeline/pipeline.hpp>
#include <pipeline/custom/custom_comm_types.hpp>

namespace mediapipeline
{

/* src element handle */
typedef struct
{
  GstElement *pSrcElement;
//  GstElement          *pHlsQueue;

  MEDIA_SRC_ELEM_IDX_T srcIdx;
//  MEDIA_CHANNEL_T ch; // add 20111025

  guint bufferMinByte;
  guint bufferMaxByte;
  guint prebufferByte;

} MEDIA_SRC_T;

typedef enum
{
  CONTENT_TYPE_UNKNOWN = 0x00,
  CONTENT_TYPE_VIDEO = 0x01,  // video mask
  CONTENT_TYPE_AUDIO = 0x02,  // audio mask
  CONTENT_TYPE_MULTI = 0x03,  // video + audio mask : do not change the value
} CUSTOM_CONTENT_TYPE_T;

typedef enum
{
  CUSTOM_BUFFER_FEED = 0,
  CUSTOM_BUFFER_LOW,
  CUSTOM_BUFFER_FULL,
  CUSTOM_BUFFER_LOCKED,
} CUSTOM_BUFFERING_STATE_T;

typedef struct
{
  gboolean bUsePreroll;
  gboolean bIsPreroll;
  guint32 bufferedSize;
  guint32 maxBufferSize;
} CUSTOM_PREROLL_STATE_T;

typedef struct
{
  guint8 contentType;
  gchar pElementName[15];
} CUSTOM_CONTAINER_TABLE_T;

class CustomPipeline:public Pipeline
{
  friend class CustomPlayer;

private:

  /*
      Gstreamer Element Handle
  */
  /* pipeline handle */
  GstElement * m_pstPipeline;

  /* element handle */

  /* src element */
  MEDIA_SRC_T m_stSrcInfo[IDX_MAX];

  /* demux element */
  GstElement *m_pstDemuxElement;

  /* video element */
  GstElement *m_pstVideoQueueElement;
  GstElement *m_pstVideoDecoderElement;

  /* audio element */
  GstElement *m_pstAudioQueueElement;
  GstElement *m_pstAudioDecoderElement;
  GstElement *m_pstAudioSelectorElement;

  GstPad *m_pstAudioSrcPad[MAX_TRACK_NUM];
  GstPad *m_pstAudioSinkPad[MAX_TRACK_NUM];
  //GstPad *pVideoPad[MAX_TRACK_NUM]; // To Do (for TS)

  /* other element */
  GstElement *pRtpJitterBufferElement;
  GstElement *pDepayQueueElement;
  GstElement *pDepayElement;

  GstElement *pDrmElement;

  GstElement *pDownloadQueueElement;
  GstElement *pDownloadSinkElement;

  GstElement *pSubtitleSinkElement;

  /* local values */
  MEDIA_CUSTOM_CONTENT_INFO_T m_stContentInfo;
  CUSTOM_CONTENT_TYPE_T m_eContentType;
  MEDIA_CUSTOM_SRC_TYPE_T m_eSrcType;
  guint8 m_uNumOfSrc;
  gint m_CheckNum;
  gint m_audCount;
  gint  m_audCurrent;
  gint m_lastPid;
  CUSTOM_BUFFERING_STATE_T m_eNeedFeedData[IDX_MAX];
  CUSTOM_PREROLL_STATE_T m_stPrerollState;
  CUSTOM_CONTAINER_TABLE_T m_stDmxList[4];

  CustomPipeline ();

  guint8 checkContentType (void);
  gint transSampleRate(MEDIA_AUDIO_SAMPLERATE_T sampleRate);
  gint getAACFreqIdx(gint frequency);

  //callback functions
  static void callbackStopFeed (GstAppSrc * element, gpointer data /*MEDIA_SRC_T * app*/);
  static gboolean callbackSeekData (GstAppSrc * element, guint64 offset, gpointer data /*MEDIA_SRC_T * app*/);
  static void callbackStartFeed (GstAppSrc * element, guint32 size,  gpointer data /*MEDIA_SRC_T * app*/);

  static void addNewPad (GstElement * element, GstPad * pad, gpointer data);

  static void underrunSignalCb(GstElement *element, gpointer data);

  MEDIA_STATUS_T initValue (void);
  MEDIA_STATUS_T addSrcElement (guint64 startOffset, const gchar * pSrcPath);
  MEDIA_STATUS_T addSrcElement_Push (void);
  MEDIA_STATUS_T addSrcElement_ES(void);
  MEDIA_STATUS_T addDemuxElement (void);
  MEDIA_STATUS_T addSinkElement_ES(void);
  MEDIA_STATUS_T setVideoPath_ES(void);
  MEDIA_STATUS_T setVdecSinkInfo_ES(void);
  MEDIA_STATUS_T setAudioPath_ES(void);
  MEDIA_STATUS_T setAdecSinkInfo_ES(void);

  void findDemux (gchar ** pDemuxName);
  void addVideoPad (GstPad * pad);
  MEDIA_STATUS_T addVideoElement (void);
  MEDIA_STATUS_T setPropertyOnVideo (void);
  void addAudioPad (GstStructure * capsStr, GstPad * pad);
  MEDIA_STATUS_T addAudioElement(gint audio_num);
  MEDIA_STATUS_T addAudioSink(gint audio_num, const gchar *mime_type, GstPad *pad, guint pid_num);
  MEDIA_STATUS_T setCapsOnAudioSink(const char *mime_type, GstPad *sinkpad);
  void addTextPad (GstPad * pad);

  static void notifySourceCallbackFunc (GObject * o, GParamSpec * p, gpointer d);

public:
  ~CustomPipeline ();

  gboolean loadSpi_pre();
  gboolean loadSpi_post();
  gboolean unloadSpi ();
  //gboolean seekSpi (gint64 ms); // TODO..

  MEDIA_STATUS_T load (MEDIA_CUSTOM_SRC_TYPE_T srcType,
                       const gchar * pSrcPath,
                       const gchar * pWritePath,
                       guint64 startOffset, MEDIA_CUSTOM_CONTENT_INFO_T * pstContentInfo);

  gboolean informationMonitorStartSpi(guint32 timeInterval);
  gboolean positionSpi(gpointer data, gint64 *pos);
  void setInterleavingTypeSpi(gpointer data, GstObject *pObj, gint stream, gpointer user_data);
  void getUndecodedSizeSpi(gpointer data, guint64* pVdecBufferedSize, guint64* pAdecBufferedSize);
  gboolean isReadyToPlaySpi ();

  gboolean checkTimeToDecodeSpi(gpointer data);

  gboolean SetLanguage(gint32 audioNum);
  MEDIA_STATUS_T FeedStream (guint8 * pBuffer, guint32 bufferSize,
                             guint64 pts, MEDIA_DATA_CHANNEL_T esData);

  GstElement getInstance () const;
  GString errorString () const;

#if 0 // TODO:
  //multi track audio, multi angle video APIs
  gboolean getAudioLanguagesListSpi(gpointer data, gchar **ppLangList, gint *pLangListSize, gint *pTotalLangNum);
  gboolean setAudioLanguageSpi(gpointer data, char *pAudioLang);
  gboolean setAudioTrackSpi(gpointer data, gint AudioTrackNum);
  gboolean getCurAudioLanguageSpi(gpointer data, char **ppAudioLang);
  gboolean getCurAudioTrackSpi(gpointer data, gint *pCurAudioTrackNum);
  gboolean getTotalVideoAngleSpi(gpointer data,  gint *pTotalVideoAngleNum);
  gboolean setVideoAngleSpi(gpointer data, gint VideoAngleNum);
  gboolean getCurrentVideoAngleSpi(gpointer data, gint *pCurrentVideoAngleNum);
  // end
#endif

};
}

#endif                          /* CUSTOMPIPELINE_HPP_ */


//end of file
