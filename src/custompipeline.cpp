/*
 * custompipeline.cpp
 *
 *  Created on: 2013. 4. 22.
 *
 */

/*******************************************************************************
  File Inclusions
*******************************************************************************/
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>
#include <string.h>
#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif
#include <pipeline/custom/custompipeline.hpp>

/******************************************************************************
   Macro Definitions
******************************************************************************/
#define LMF_DBG_PRINT g_print
#define LMF_ERR_PRINT g_print
#define LMF_BUFFER_PERI_PRINT g_print

/*******************************************************************************
  Local Constant Definitions
*******************************************************************************/
#define MEDIA_QUEUE_MAX_SIZE (2*1024*1024)

/******************************************************************************
  Function Definitions
******************************************************************************/
using namespace mediapipeline;

CustomPipeline::CustomPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");
  /*  demuxer pad check num initiallize    */
  m_CheckNum = 0;
  /*  demuxer list initiallize    */
  m_stDmxList[0].contentType = MEDIA_CUSTOM_CONTAINER_TS;
  strcpy (m_stDmxList[0].pElementName, "tsdemux");
  m_stDmxList[1].contentType = MEDIA_CUSTOM_CONTAINER_MCAST_TS;
  strcpy (m_stDmxList[1].pElementName, "mcastdemux");
  m_stDmxList[2].contentType = MEDIA_CUSTOM_CONTAINER_MPEG2PES;
  strcpy (m_stDmxList[2].pElementName, "mpegpsdemux");

  /* last */
  m_stDmxList[3].contentType = 0xFF;
  m_stDmxList[3].pElementName[0] = '\0';



  /* multi audio track info initialize */
  m_audCount = 0;
  m_audCurrent = 0;
  m_lastPid = 0;
#if 0                           /* jhtark - webos refactory - temp [ pipeline is created at other fucntion ] */
  m_pipeHandle = gst_element_factory_make ("playbin2", NULL);
  if (m_pipeHandle) {
    m_playbinVersion = 2;       //playbin2
    int flags = 0;
    //GST_PLAY_FLAG_NATIVE_VIDEO omits configuration of ffmpegcolorspace and videoscale
    g_object_get (G_OBJECT (m_pipeHandle), "flags", &flags, NULL);
    flags |= GST_PLAY_FLAG_NATIVE_VIDEO;
    g_object_set (G_OBJECT (m_pipeHandle), "flags", flags, NULL);
  } else {
    m_pipeHandle = gst_element_factory_make ("playbin", NULL);
    m_playbinVersion = 1;       //playbin

  }

  // connect source notify.
  g_signal_connect (G_OBJECT (m_pipeHandle), "notify::source",
                    G_CALLBACK (playbinNotifySource), this);

  // connect volume notify.
  double volume = 1.0;
  g_object_get (G_OBJECT (m_pipeHandle), "volume", &volume, NULL);
  m_volume = int (volume * 100);

  g_signal_connect (G_OBJECT (m_pipeHandle), "notify::volume",
                    G_CALLBACK (handleVolumeChange), this);
  if (m_playbinVersion == 2)
    g_signal_connect (G_OBJECT (m_pipeHandle), "notify::mute",
                      G_CALLBACK (handleMutedChange), this);
#endif
}

CustomPipeline::~CustomPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");
}


void
CustomPipeline::unload ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");
}

bool CustomPipeline::play (int rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

  return true;
}

gboolean CustomPipeline::positionSpi(gpointer data, gint64 *pos)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  gint64 position = 0;
  char *pPlayerName = gst_element_get_name(self->m_pipeHandle);

  if((pPlayerName != NULL) && (!strcmp("static-player", pPlayerName)))
  {
    /*
    * we found 4-second block issue when we play HLS content.
    * MTK said he put wait mechanism for Netlfix certification when we get current pts
    * so, below code is changed to  [if(lph->SessionState != PlayingState)] from [if(lph->SessionState == PausedState)]
    * for resolving 4-second block issue.
    */

    if (self->m_gstPipelineState != PlayingState)
    {
      //TODO pos = self->m_SessionPlayPositionStatic;
    }
    else
    {
      //TODO if (_STATIC_GetPlayInfo(pPlayerHandle->ch, &pos) != LMF_OK)
      {
        pos = 0;
      }
      //TODO self->m_SessionPlayPositionStatic = pos;
    }
  }
  else
  {
    GstFormat queryFormat = GST_FORMAT_TIME;
#if (GST_VERSION_MAJOR >= 1)
    if(!gst_element_query_position(self->m_pipeHandle, queryFormat, &position))
#else
    if(!gst_element_query_position(self->m_pipeHandle, &queryFormat, &position))
#endif
      position = 0;
  }
  if (pPlayerName != NULL)
    g_free(pPlayerName);

  *pos = position;
  return true;
}

gboolean CustomPipeline::informationMonitorStartSpi(guint32 timeInterval)
{
  if ((m_pipeHandle == NULL) || (GST_IS_ELEMENT(m_pipeHandle) == FALSE))
  {
    std::cout << "Error. Gstreamer Player Handle is NULL!!!  " << endl;
    return false;
  }

  //TODO if(_STATIC_COMM_bNeedCheckState(pPlayerHandle->ch))
  {
    //g_print("[%s] Buffering Timer Start!(Interval: 200ms) - update buffering info.\n", __FUNCTION__);
    //m_bufferingTimerId = g_timeout_add(200, (GSourceFunc)updateBufferingInfo, this);
  }
  return true;
}

MEDIA_STATUS_T
CustomPipeline::load (MEDIA_CUSTOM_SRC_TYPE_T srcType,
                      const gchar * pSrcPath,
                      const gchar * pWritePath,
                      guint64 startOffset, MEDIA_CUSTOM_CONTENT_INFO_T * pstContentInfo)
{

  MEDIA_STATUS_T retVal = MEDIA_OK;

#if 0                           /* jhtark - webos refactory - temp [ need to organize NC4.0 'static init' parameter ] */
  LMF_DBG_PRINT ("========================================\n");
  LMF_DBG_PRINT ("[%s:%d] ch:%d, SrcType: %d\n", __FUNCTION__, __LINE__, ch,
                 srcType);
  LMF_DBG_PRINT ("src loc: %s, write loc: %s\n",
                 (pSrcPath == NULL) ? "NULL" : (pSrcPath),
                 (pWritePath == NULL) ? "NULL" : (pWritePath));
  LMF_DBG_PRINT ("content type: [%d][%d:%d][pts:%lld][%s][adecCh:%d]\n",
                 contentInfo.container, contentInfo.vcodec, contentInfo.acodec,
                 contentInfo.ptsToDecode,
                 (contentInfo.bSeperatedPTS) ? "Seperated" : "Included",
                 contentInfo.audioDataInfo.adecCh);
  LMF_DBG_PRINT
  (" size: %lld, esCh: %d, videoBuff [%d:%d], audioBuff [%d:%d]\n",
   contentInfo.size, contentInfo.esCh,
   contentInfo.videoDataInfo.bufferMinLevel,
   contentInfo.videoDataInfo.bufferMaxLevel,
   contentInfo.audioDataInfo.bufferMinLevel,
   contentInfo.audioDataInfo.bufferMaxLevel);
  LMF_DBG_PRINT ("========================================\n");
#endif

  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomerPipeline");

  //pipeline is already created on initSpi_pre()

#if 0                           /* jhtark - webos refactory - temp [ need to organize NC4.0 pipeline sturcture ] */
  if (contentInfo.videoDataInfo.isSSPK == TRUE)       //To prevent sspk forced resume
    _gIsStartedPlay[ch] = TRUE;

  LMF_DBG_PRINT ("[%s:%d] Set StartedPlay value-> [%d]\n", __FUNCTION__,
                 __LINE__, _gIsStartedPlay[ch]);

  gPipelineInfo[ch].ch = ch;

  gPipelineInfo[ch].feededTotalSize = 0;
#endif

  m_eSrcType = srcType;
  m_eContentType = CONTENT_TYPE_UNKNOWN;
  m_uNumOfSrc = 1;
  memcpy (&m_stContentInfo, pstContentInfo, sizeof (MEDIA_CUSTOM_CONTENT_INFO_T));

#if 1                           /* changbok.chea - webos refactory - temp [ need to organize NC4.0 pipeline sturcture ] */
  if (m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_ES)
    m_uNumOfSrc = _CheckContentType ();

  LMF_DBG_PRINT ("gPipelineInfo[ch].numOfSrc == %d\n", m_uNumOfSrc);
  m_audCount = 0;
#endif

  /* create src element based on each src type */
  if (_addSrcElement (startOffset, pSrcPath) != MEDIA_OK) {
    LMF_ERR_PRINT ("[%s:%d] fail to create the src element (%s)\n", __FUNCTION__, __LINE__, pSrcPath);
    return MEDIA_ERROR;
  }


  /* create demux element & sink element link */
  switch (m_eSrcType) {
  case MEDIA_CUSTOM_SRC_TYPE_ES:
  {
#if 0                           /* jhtark - webos refactory - temp [ need to add ] */
    retVal =
      LMF_STATIC_ES_AddSinkElement (&gPipelineInfo[ch], srcType,
                                    contentInfo, pWritePath);
#endif
  }
  break;
  case MEDIA_CUSTOM_SRC_TYPE_DOWNLOAD:
  {
#if 0                           /* jhtark - webos refactory - temp [ need to add ] */
    retVal =
      LMF_STATIC_DN_AddSinkElement (&gPipelineInfo[ch], srcType,
                                    contentInfo, pWritePath);
#endif
  }
  break;
  default:
  {
    //push case must find demuxer element
    retVal = _addDemuxElement ();
  }
  break;
  }

  if (retVal != MEDIA_OK) {
    LMF_ERR_PRINT ("[%s:%d] fail to create the sink element or find demux \n", __FUNCTION__, __LINE__);
    return MEDIA_ERROR;
  }

  //set pipeline status to READY
  gst_element_set_state(m_pipeHandle, GST_STATE_READY);


  /*
     <<      TODO    >>
     1. who set pipeline's state to 'puased'?
     (customerplayer or customerpipeline)
     2. after updating 'pad-added' signal' from demuxer, link elements.
     3. need to consider using multiqueue  -----
   */

#if 0                           /* jhtark - webos refactory - temp [ need to add ] */

  // 5. [Thread] create thread
  if (_LMF_STATIC_COMM_CreateThread (ch, srcType, contentInfo,
                                     pWritePath) != LMF_OK) {
    LMF_ERR_PRINT ("[%s:%d] fail to create thread\n", __FUNCTION__, __LINE__);
    return LMF_NOT_OK;
  }
  // #. [debug] dump file when feeding - push case
  _LMF_STATIC_COMM_DBG_Dump (srcType);

  LMF_STATIC_COMM_StartBufferMonitoring (ch);
#endif

  //set pipeline status to Pause
  if(pause() == false)
    return MEDIA_ERROR;

  return MEDIA_OK;
}

gboolean CustomPipeline::loadSpi_pre()
{

  /* initial parameter in customer pipeline */
  if (MEDIA_OK != _initValue ()) {
    return false;
  } else {

    /* create pipeline */
    m_pstPipeline = gst_pipeline_new ("custom-player");

    if (!m_pstPipeline)
    {
      LMF_DBG_PRINT ("[%s:%d] pipeline could not be created!!!\n", __FUNCTION__,  __LINE__);
      return false;
    }
  }

  //register custom pipeline handle
  m_pipeHandle = m_pstPipeline;

  return true;
}
gboolean CustomPipeline::loadSpi_post()
{
  MEDIA_CUSTOM_CONTENT_INFO_T contentInfo;

  //TODO 1. parse Json

  //TODO 2. make content info

  //TODO 3. call load ()
  memset(&contentInfo,0x00,sizeof(MEDIA_CUSTOM_CONTENT_INFO_T));

  contentInfo.container = MEDIA_CUSTOM_CONTAINER_TS;

  load(MEDIA_CUSTOM_SRC_TYPE_PUSH, NULL,NULL, 0, &contentInfo);

  return true;
}


guint8
CustomPipeline::_CheckContentType (void)
{
  guint8 numOfSrc = 0;

  LMF_DBG_PRINT ("[%s:%d] vcodec= 0x%X, acodec= 0x%X\n", __FUNCTION__, __LINE__,
                 m_stContentInfo.vcodec, m_stContentInfo.acodec);

  if (m_stContentInfo.acodec != MEDIA_AUDIO_NONE)       // audio
    m_eContentType =
      (CUSTOM_CONTENT_TYPE_T) (m_eContentType | CONTENT_TYPE_AUDIO);

  if (m_stContentInfo.vcodec != MEDIA_VIDEO_NONE)       // video
    m_eContentType =
      (CUSTOM_CONTENT_TYPE_T) (m_eContentType | CONTENT_TYPE_VIDEO);

  if (m_eContentType == CONTENT_TYPE_MULTI)
    numOfSrc = 2;
  else
    numOfSrc = 1;


  return numOfSrc;
}

gint
CustomPipeline::_transSampleRate(MEDIA_AUDIO_SAMPLERATE_T sampleRate)
{
  gint sample = 0;

  switch(sampleRate)
  {
  case MEDIA_AUDIO_SAMPLERATE_48K:
    sample = 48000;
    break;
  case MEDIA_AUDIO_SAMPLERATE_44_1K:
    sample = 44100;
    break;
  case MEDIA_AUDIO_SAMPLERATE_32K:
    sample = 32000;
    break;
  case MEDIA_AUDIO_SAMPLERATE_24K:
    sample = 24000;
    break;
  case MEDIA_AUDIO_SAMPLERATE_16K:
    sample = 16000;
    break;
  case MEDIA_AUDIO_SAMPLERATE_12K:
    sample = 12000;
    break;
  case MEDIA_AUDIO_SAMPLERATE_8K:
    sample = 8000;
    break;
  case MEDIA_AUDIO_SAMPLERATE_22_05K:
    sample = 22050;
    break;
  default:
    break;
  }

  LMF_DBG_PRINT("[%s:%d] Get Sample Rate Value [%d -> %d]\n", __FUNCTION__, __LINE__, sampleRate, sample);
  return sample;
}


MEDIA_STATUS_T
CustomPipeline::_addSrcElement_Push (void)
{
  MEDIA_SRC_T *pSrcInfo;
  MEDIA_STATUS_T retVal = MEDIA_OK;

  guint bufferMaxLevel = 0, bufferMinPercent = 0, preBufferLevel = 0;

  GstAppSrcCallbacks callbacks = {&_callbackStartFeed,&_callbackStopFeed,&_callbackSeekData};

  LMF_DBG_PRINT ("[%s:%d][PUSH]\n", __FUNCTION__, __LINE__);

  pSrcInfo = &(m_stSrcInfo[IDX_MULTI]);

  bufferMaxLevel = m_stContentInfo.videoDataInfo.bufferMaxLevel;
  if ((bufferMaxLevel > MEDIA_BUFFER_SIZE) || (bufferMaxLevel == 0))
    bufferMaxLevel = MEDIA_BUFFER_SIZE;

  if (bufferMaxLevel != 0)
    bufferMinPercent =
      (guint) (((gfloat) m_stContentInfo.videoDataInfo.bufferMinLevel /
                (gfloat) bufferMaxLevel) * 100);

  preBufferLevel = m_stContentInfo.videoDataInfo.prebufferLevel;
  if (preBufferLevel > (bufferMaxLevel / 2))
    preBufferLevel = (bufferMaxLevel / 2);

  pSrcInfo->bufferMinByte = m_stContentInfo.videoDataInfo.bufferMinLevel;
  pSrcInfo->bufferMaxByte = bufferMaxLevel;
  pSrcInfo->prebufferByte = preBufferLevel;

  LMF_DBG_PRINT
  ("[PUSH][BUFFER LEVEL][REQ] MAX : %u (MIN : %u bytes), PRE:%u\n",
   m_stContentInfo.videoDataInfo.bufferMaxLevel,
   m_stContentInfo.videoDataInfo.bufferMinLevel,
   m_stContentInfo.videoDataInfo.prebufferLevel);
  LMF_DBG_PRINT
  ("[PUSH][BUFFER LEVEL][ADJ] MAX : %u (MIN : %u bytes / %u% ), PRE:%u\n",
   bufferMaxLevel, m_stContentInfo.videoDataInfo.bufferMinLevel,
   bufferMinPercent, preBufferLevel);

  m_eNeedFeedData[IDX_MULTI] = CUSTOM_BUFFER_FEED;

  if (preBufferLevel != 0) {
    m_stPrerollState.bUsePreroll = TRUE;
    m_stPrerollState.bIsPreroll = TRUE;
    m_stPrerollState.bufferedSize = 0;
    m_stPrerollState.maxBufferSize = preBufferLevel;
  } else {
    m_stPrerollState.bUsePreroll = FALSE;
    m_stPrerollState.bIsPreroll = FALSE;
    m_stPrerollState.bufferedSize = 0;
    m_stPrerollState.maxBufferSize = 0;
  }

  pSrcInfo->pSrcElement = gst_element_factory_make ("appsrc", "app-src");
  if (!pSrcInfo->pSrcElement) {
    LMF_DBG_PRINT ("appsrc element create fail\n");
    return MEDIA_ERROR;
  }

  g_object_set (G_OBJECT (pSrcInfo->pSrcElement),
                "format", GST_FORMAT_TIME,
                "max-bytes", (guint64) (bufferMaxLevel),
                "min-percent", bufferMinPercent, NULL);

  // gst_util_set_object_arg (G_OBJECT(pInfo->pPipeline), "stream-type", "stream");
  gst_util_set_object_arg (G_OBJECT (pSrcInfo->pSrcElement), "stream-type",
                           "seekable");

#if 0 /* jhtark - webos refactory - temp [ need to confirm gst ] */ // changbok : need to check compile error 
  /* changbok:Todo check callback compile error */
//  g_signal_connect (G_OBJECT (pSrcInfo->pSrcElement), "enough-data",G_CALLBACK (_callbackStopFeed), this);
//  g_signal_connect (G_OBJECT (pSrcInfo->pSrcElement), "seek-data",G_CALLBACK (_callbackSeekData), this);
//  g_signal_connect(pSrcInfo->pSrcElement, "need-data", G_CALLBACK(_LMF_StartFeed), pSrcInfo);
#else
  {
    GstAppSrc *pstAppSrc = reinterpret_cast<GstAppSrc*>(static_cast<GstElement*>(pSrcInfo->pSrcElement));

//  TODO : the reason that the function is not included.  gst_app_src_set_callbacks(pstAppSrc/*pSrcInfo->pSrcElement*/,&callbacks,this,NULL );
  }
#endif

  //register appsrc to pipeline
  gst_bin_add (GST_BIN (m_pstPipeline), pSrcInfo->pSrcElement);

#if 0                           /* jhtark - webos refactory - temp [ still not use MTK platform ] */
#if (PLATFORM_TYPE == MTK_PLATFORM)

  // MD지수 개선 : MTK전용 pipeline 구성한 부분을 분리
  retVal = _LMF_STATIC_PKG_ConfigureMTKPipeline (pPipeline);

  if (retVal != LMF_OK) {
    LMF_ERR_PRINT
    ("[%s:%d][PUSH] _LMF_STATIC_PKG_ConfigureMTKPipeline() return fail \n",
     __FUNCTION__, __LINE__);
    return retVal;
  }
#endif
#endif

  return retVal;
}

MEDIA_STATUS_T
CustomPipeline::_addSrcElement (guint64 startOffset, const gchar * pSrcPath)
{
  MEDIA_SRC_T *pSrcInfo;
  MEDIA_STATUS_T retVal = MEDIA_OK;

  pSrcInfo = &m_stSrcInfo[IDX_MULTI];

  pSrcInfo->srcIdx = IDX_MULTI;

#if 0                           /* jhtark - webos refactory - temp [ not used ] */
  pSrcInfo->ch = ch;            // save channel. 20111025
#endif

  LMF_DBG_PRINT ("[%s:%d] srcType : %d\n", __FUNCTION__, __LINE__, m_eSrcType);

  switch (m_eSrcType) {
  case MEDIA_CUSTOM_SRC_TYPE_URI:
  case MEDIA_CUSTOM_SRC_TYPE_DOWNLOAD:
  {
#if 0                           /* jhtark - webos refactory - temp [ need to add ] */
    if (pPath == NULL) {
      LMF_ERR_PRINT ("[%s:%d][ERROR] pPath is NULL!!!\n", __FUNCTION__,
                     __LINE__);
      return MEDIA_ERROR;
    }
    // protocol : HTTP only
    pSrcInfo->pSrcElement =
      gst_element_make_from_uri (GST_URI_SRC, pPath, "uri-source");
    if (!pSrcInfo->pSrcElement) {
      LMF_ERR_PRINT ("[%s:%d][ERROR] urisrc element can not be created!!!\n",
                     __FUNCTION__, __LINE__);
      return MEDIA_ERROR;
    }

    LMF_STATIC_COMM_SetPropertyOfProtocol ((gchar *) pPath,
                                           pSrcInfo->pSrcElement);
    gst_bin_add (GST_BIN (gPipelineInfo[ch].pPipeline),
                 pSrcInfo->pSrcElement);
#endif
  }
  break;
  case MEDIA_CUSTOM_SRC_TYPE_FILE:
  {
#if 0                           /* jhtark - webos refactory - temp [ need to add ] */
    /* add element for File case */
    retVal = LMF_STATIC_OTHER_AddFileElement (&gPipelineInfo[ch], pPath);
    if (retVal != MEDIA_OK) {
      LMF_ERR_PRINT ("[%s:%d] LMF_STATIC_OTHER_AddFileElement return Fail!\n",
                     __FUNCTION__, __LINE__);
      return MEDIA_ERROR;
    }
#endif
  }
  break;
  case MEDIA_CUSTOM_SRC_TYPE_SCREEN_SHARE:
  {
#if 0                           /* jhtark - webos refactory - temp [ need to add ] */
    /* add element for UDP case */
    retVal =
      LMF_STATIC_OTHER_AddUDPElement (&gPipelineInfo[ch], pcontentInfo);
    if (retVal != MEDIA_OK) {
      LMF_ERR_PRINT ("[%s:%d] _LMF_STATIC_AddElementOnUDPCase return Fail!\n",
                     __FUNCTION__, __LINE__);
      return MEDIA_ERROR;
    }
#endif
  }
  break;
  case MEDIA_CUSTOM_SRC_TYPE_FCC:
  case MEDIA_CUSTOM_SRC_TYPE_RTP_MCAST:
  {
#if 0                           /* jhtark - webos refactory - temp [ need to add  ] */
    /* add element for FCC case */
    retVal =
      LMF_STATIC_OTHER_AddFCCElement (&gPipelineInfo[ch], pcontentInfo,
                                      pPath);
    if (retVal != MEDIA_OK) {
      LMF_ERR_PRINT ("[%s:%d] LMF_STATIC_OTHER_AddFCCElement return Fail!\n",
                     __FUNCTION__, __LINE__);
      return MEDIA_ERROR;
    }
#endif
  }
  break;
  case MEDIA_CUSTOM_SRC_TYPE_ES:
  {
    /* Add Elements on ES case */
//        retVal = LMF_STATIC_ES_AddElement(&gPipelineInfo[ch],pcontentInfo);
    if (retVal != MEDIA_OK)
    {
      LMF_ERR_PRINT ("[%s:%d] _LMF_STATIC_AddElementOnESCase return Fail!\n",
                     __FUNCTION__, __LINE__);
      return MEDIA_ERROR;
    }
  }
  break;
  case MEDIA_CUSTOM_SRC_TYPE_PUSH:   // no prebuffer
  case MEDIA_CUSTOM_SRC_TYPE_TTS:    // no prebuffer
  {
    /* Add Elements on Push and TTS case */
    retVal = _addSrcElement_Push ();
    if (retVal != MEDIA_OK)
    {
      LMF_ERR_PRINT ("[%s:%d] LMF_STATIC_PKG_AddElement return Fail!\n", __FUNCTION__, __LINE__);
      return MEDIA_ERROR;
    }
  }
  break;
  default:
  {
    LMF_DBG_PRINT ("[%s:%d] Src Type is wrong.(%d)\n", __FUNCTION__, __LINE__,m_eSrcType);
    return MEDIA_ERROR;
    // break;
  }
  }

  return MEDIA_OK;

}

void
CustomPipeline::_callbackStopFeed (GstAppSrc * element, gpointer data /*MEDIA_SRC_T * app*/)
{

#if 0
#if 0                           /* jhtark - webos refactory - temp [ need to organize below statements ] */
  MEDIA_CHANNEL_T ch = app->ch;

  if (ch == MEDIA_CH_UNKNOWN)
    return;
  LMF_APP_HNDL_T *pLmfAppHndl = API_LMF_PLYR_GetAppHandle (ch);
  if (pLmfAppHndl == NULL)
    return;
  LMF_PLYR_HNDL_T *pLmfPlayerHndl = &(pLmfAppHndl->priv_lph);
  if (pLmfPlayerHndl == NULL)
    return;
#endif

  if ((m_eNeedFeedData[app->srcIdx] != CUSTOM_BUFFER_FULL) &&
      (m_eNeedFeedData[app->srcIdx] != CUSTOM_BUFFER_LOCKED)) {
    guint64 queuedSize = 0;

#if 0                           /* jhtark - webos refactory - temp [ 추후 진행  ] */
    queuedSize =
      gst_app_src_get_queued_bytes (GST_APP_SRC (app->pSrcElement
                                    /*m_stSrcInfo[app->srcIdx].pSrcElement */ ));
#endif

    LMF_DBG_PRINT ("[Buffer Full]__________  stop feeding... (%s:%llu) \n",
                   (app->srcIdx == IDX_VIDEO) ? "Video" : "Audio", queuedSize);
    m_eNeedFeedData[app->srcIdx] = CUSTOM_BUFFER_FULL;

#if 0                           /* jhtark - webos refactory - temp [ 추후 연결 - Event ] */
    // es case only?    need to keep srcType...;;
    if (app->srcIdx == IDX_VIDEO)
      API_LMF_EVENT_Notify (pLmfPlayerHndl->ch, MEDIA_CB_MSG_ERR_BUFFERFULL);   //(MSG_FLASHES2UI_BUFFER_FULL);
#endif
  }

  if ((m_stPrerollState.bUsePreroll == TRUE) && (app->srcIdx == IDX_VIDEO) &&   // multi or video
      (m_stPrerollState.bIsPreroll == TRUE)) {
    LMF_DBG_PRINT ("#### Stream Play start (Buffer FULL)--- \n");

#if 0                           /* jhtark - webos refactory - temp [ 추후 연결 - Event ] */
    LMF_PLYR_CTRL_Play (pLmfPlayerHndl);        //LMF_STATIC_Play();
#endif
    m_stPrerollState.bIsPreroll = FALSE;

#if 0                           /* jhtark - webos refactory - temp [ 추후 추가 ] */
    _gIsStartedPlay[ch] = TRUE;
#endif
  }

#endif
}

gboolean
CustomPipeline::_callbackSeekData (GstAppSrc * element, guint64 offset, gpointer data /*   MEDIA_SRC_T * app */)
{
//  LMF_DBG_PRINT ("[SEEK DATA]__________ offset:%lld / IDX_%s \n", offset,   (app->srcIdx == IDX_VIDEO) ? "Video" : "Audio");
  return true;
}


void
CustomPipeline::_callbackStartFeed (GstAppSrc * element, guint32 size,  gpointer data /*MEDIA_SRC_T * app*/)
{
#if 0                           /* jhtark - webos refactory - temp [ ned to organize below statements  ] */
  MEDIA_CHANNEL_T ch = app->ch;

  if (ch == MEDIA_CH_UNKNOWN)
    return;
  LMF_APP_HNDL_T *pLmfAppHndl = API_LMF_PLYR_GetAppHandle (ch);
  if (pLmfAppHndl == NULL)
    return;
  LMF_PLYR_HNDL_T *pLmfPlayerHndl = &(pLmfAppHndl->priv_lph);
  if (pLmfPlayerHndl == NULL)
    return;
#endif

#if 0                           /* changbok - webos refactory - temp [ need to link event(HTML5 Youtube callback) ] */
  //LMF_DBG_PRINT("***************[Need-Data]Buffer Low state = %d *************\n", gPipelineInfo[ch].bNeedFeedData[app->srcIdx]);
  if (app->srcIdx == IDX_VIDEO) //HTML5 youtube playback test
    API_LMF_EVENT_Notify (pLmfPlayerHndl->ch, MEDIA_CB_MSG_YOUTUBE_NEED_VDATA);
  else if (app->srcIdx == IDX_AUDIO)
    API_LMF_EVENT_Notify (pLmfPlayerHndl->ch, MEDIA_CB_MSG_YOUTUBE_NEED_ADATA);
#endif

#if 0
  if (m_eNeedFeedData[app->srcIdx] == CUSTOM_BUFFER_FULL) {
    //LMF_DBG_PRINT("[Buffer Low]__________ FULL -> LOW (%s)\n", (app->srcIdx == IDX_VIDEO)?"Video":"Audio");
    m_eNeedFeedData[app->srcIdx] = CUSTOM_BUFFER_LOW;
  }
#endif
}

void CustomPipeline::_underrunSignalCb(GstElement *element, gpointer data)
{
  CustomPipeline *pCustomPipe = (CustomPipeline*)data;

#if 0     /* jhtark - webos refactory - temp [ need to organize below statements  ] */
  LMF_APP_HNDL_T *pLmfAppHndl = NULL;
  LMF_PLYR_HNDL_T *pLmfPlayerHndl = NULL;

  pLmfAppHndl = API_LMF_PLYR_GetAppHandle(pPipeline->ch);
  if(pLmfAppHndl==NULL)
  {
    LMF_ERR_PRINT("[%s:%d]pLmfAppHndl is NULL\n", __FUNCTION__, __LINE__);
    return;
  }

  pLmfPlayerHndl = &(pLmfAppHndl->priv_lph);
  if(pLmfPlayerHndl==NULL)
  {
    LMF_ERR_PRINT("[%s:%d]pLmfPlayerHndl is NULL\n", __FUNCTION__, __LINE__);
    return;
  }
#endif

  LMF_BUFFER_PERI_PRINT("[%s:%d]-------- VIDEO underrun dectected\n", __FUNCTION__, __LINE__);

  if( pCustomPipe->m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_ES ) // ES case only.. To Do TS/PS case
  {
    if(pCustomPipe->m_eNeedFeedData[IDX_VIDEO] != CUSTOM_BUFFER_LOW)
    {
      if (strcmp(gst_element_get_name(element), "vdec-sink") == 0)
        LMF_BUFFER_PERI_PRINT("[Buffer Low]_____EVENT_(V_Underrun_Signal)_ Notify Callback to APP\n");
      else
        LMF_BUFFER_PERI_PRINT("[Buffer Low]_____EVENT_(A_Underrun_Signal)_ Notify Callback to APP\n");

#if 0  /* jhtark - webos refactory - temp [ need to link event ] */
      API_LMF_EVENT_Notify(pLmfPlayerHndl->ch, MEDIA_CB_MSG_ERR_BUFFERLOW);//(MSG_FLASHES2UI_BUFFER_LOW);
#endif
      pCustomPipe->m_eNeedFeedData[IDX_VIDEO] = CUSTOM_BUFFER_LOW;
    }
  }
}


MEDIA_STATUS_T
CustomPipeline::_initValue (void)
{

  GError *err;
  LMF_DBG_PRINT ("[%s] ----- start -----\n", __FUNCTION__);

  // thread init
  if (!g_thread_supported ()) {
    g_thread_init (NULL);
  }

  /* jhtark - webos refactory - temp [ need to add gst  ] */
  if (!gst_init_check (NULL, NULL, &err)) {
    LMF_DBG_PRINT ("gst_init Error: %s\n", err->message);
    g_error_free (err);

    return MEDIA_ERROR;
  }

#if 0                           /* jhtark - webos refactory - temp [ need to organize NC4.0 global values ] */
  gbReceivedEos[ch] = FALSE;
  _gbSeekLog = FALSE;
  currentPts[ch] = 0;
  currentPlayPts[ch] = 0;
  _gBufferCheckInterval[ch] = 0;
  memset (&_gBufferLowStableCnt[ch], 0x00, (sizeof (guint) * IDX_MAX));
  _gCurrentPtsUpdateCnt[ch] = 0;
  _gIsStartedPlay[ch] = FALSE;

  memset (&_gFeedTotal[ch], 0x00, (sizeof (guint64) * IDX_MAX));
  memset (&_gBufferLevel[ch], 0x00, (sizeof (guint64) * IDX_MAX));
  memset (&_gFeedRate[ch], 0x00, (sizeof (LMF_FEED_RATE_T) * IDX_MAX));
  memset (&gPipelineInfo[ch], 0x00, sizeof (LMF_PIPELINE_INFO_T));

  gCheckPadNum = 0;
  gbCheckFirstPlay = FALSE;
#endif

  return MEDIA_OK;
}

void
CustomPipeline::_addNewPad (GstElement * element, GstPad * pad, gpointer data)
{

  char *pad_name;
  guint32 cnt;

  char *pElementName = NULL;
  char *pStrStructure = NULL;

  GstStructure *capsStr = NULL;
  GstCaps *caps = NULL;

  CustomPipeline *pCustomPipe = (CustomPipeline *)data;

  LMF_DBG_PRINT ("Singal: pad-added\n");

  pad_name = gst_pad_get_name (pad);
  pElementName = gst_element_get_name (element);

  LMF_DBG_PRINT ("Element name= %s \n", pElementName);
  LMF_DBG_PRINT ("Pad name= %s\n", pad_name);

#if 0 /* changbok - webos refactory - temp [ need to organize NC4.0 ifdef statements ] */
#ifdef INCLUDE_ACTVILA
  if (g_strrstr (pad_name, "subtitle")) {
    guint *datalength = NULL;
    datalength = (guint *) & pPESDataBuffer[0];

    if ((datalength > 0) && (pPESDataBuffer != NULL)) {
      BOOLEAN IsOnOff = FALSE;
      LMF_STATUS_T res = LMF_NOT_OK;

      LMF_SUBT_PES_SetExist (); // on subtitle thread.

      res = LMF_SUBT_IsPESDataFeedStart (&IsOnOff);
      if ((res == LMF_OK) && (IsOnOff == TRUE)) {
        LMF_SUBT_QueuingPESData ((*datalength) + sizeof (guint),
                                 pPESDataBuffer);
      } else {
        LMF_DBG_PRINT ("(PES subtitle = OFF) - skip queuing. \n");
      }
    }

    goto exit_addPad;
  }
#endif //INCLUDE_ACTVILA
#endif
  caps = gst_pad_query_caps (pad, NULL);

  capsStr = gst_caps_get_structure (caps, 0);
  if (capsStr == NULL) {
    LMF_DBG_PRINT ("%s.%d: unable to get structure from caps\n", __FUNCTION__,  __LINE__);
    goto exit_addPad;
  }
  cnt = gst_caps_get_size (caps);

  if (g_strrstr (pad_name, "video"))
  {
    LMF_DBG_PRINT ("gst test] enter video pad added\n");

    pStrStructure = gst_structure_to_string (capsStr);

    LMF_DBG_PRINT ("[Video] cnt=%d, str = %s\n", cnt, pStrStructure);

    pCustomPipe->_addVideoPad (pad);

    //add pad number
//    m_CheckNum++;
  } else if (g_strrstr (pad_name, "audio")) {
    LMF_DBG_PRINT ("gst test] enter audio pad added\n");

    pStrStructure = gst_structure_to_string (capsStr);

    LMF_DBG_PRINT ("[Audio] cnt=%d, str = %s\n", cnt, pStrStructure);

    pCustomPipe->_addAudioPad (capsStr, pad);

    //add pad number
    //    m_CheckNum++;
  } else if (g_strrstr (pad_name, "application")) {
    LMF_DBG_PRINT ("gst test] enter text pad added\n");
    pCustomPipe->_addTextPad (pad);

  } else {
    LMF_DBG_PRINT ("NO LINK:: Mime Type is %s\n", pad_name);
  }

exit_addPad:

  if (pad_name != NULL)
    g_free (pad_name);
  if (pElementName != NULL)
    g_free (pElementName);
  if (pStrStructure != NULL)
    g_free (pStrStructure);

  return;
}

void
CustomPipeline::_addVideoPad (GstPad * pad)
{
  GstPad *sinkpad = NULL;
  MEDIA_STATUS_T retVal = MEDIA_ERROR;

  //FF 1 : Add Video Element
  retVal = _addVideoElement ();

  if (retVal == MEDIA_ERROR) {
    LMF_ERR_PRINT
    ("%s.%d: _LMF_STATIC_COMM_AddVideoElement() return Error !!!\n",
     __FUNCTION__, __LINE__);
    goto exit_addVideoPad;
  }
  //FF 2 : set Property on Video
  retVal = _setPropertyOnVideo ();
  if (retVal == MEDIA_ERROR) {
    LMF_ERR_PRINT
    ("%s.%d: _LMF_STATIC_COMM_SetPropertyOnVideo() return Error !!!\n",
     __FUNCTION__, __LINE__);
//    goto exit_addVideoPad;
  }
  //FF 3 : Add Pad
  sinkpad = gst_element_get_static_pad (m_pstVideoQueueElement, "sink");
  if (sinkpad == NULL) {
    LMF_DBG_PRINT ("%s.%d: Video Queue Pad is NULL!!!\n", __FUNCTION__,
                   __LINE__);
    goto exit_addVideoPad;
  }

  if (GST_PAD_LINK_OK != gst_pad_link (pad, sinkpad)) {
    LMF_DBG_PRINT ("%s.%d: link video queue was failed!!!!\n", __FUNCTION__,
                   __LINE__);
    // goto exit_addPad;
  } else {
    GstElement *parent;
    GstState target;

    LMF_DBG_PRINT ("%s.%d: link video queue was successed...\n", __FUNCTION__,
                   __LINE__);
    /*
    #if (PLATFORM_TYPE == MTK_PLATFORM)             // still not use MTK platform in webOS

        if ((pInfo->srcType != LMF_MEDIA_SRC_TYPE_FILE) &&
          (pInfo->srcType != LMF_MEDIA_SRC_TYPE_SCREEN_SHARE) &&
          (pInfo->srcType != LMF_MEDIA_SRC_TYPE_FCC) &&
          (pInfo->srcType != LMF_MEDIA_SRC_TYPE_RTP_MCAST))
        {
          gst_element_link(pInfo->pVideoQueueElement, pInfo->pVideoDecoderElement);
        }
        else
    #endif
    */
    {
      if ((parent =
             GST_ELEMENT_CAST (gst_element_get_parent (m_pstVideoQueueElement)))) {
        GstState parent_current, parent_pending;

        GST_OBJECT_LOCK (parent);
        parent_current = GST_STATE (parent);
        parent_pending = GST_STATE_PENDING (parent);
        GST_OBJECT_UNLOCK (parent);

        /* set to pending if there is one, else we set it to the current state of
         * the parent */
        if (parent_pending != GST_STATE_VOID_PENDING)
          target = parent_pending;
        else
          target = parent_current;

        LMF_DBG_PRINT
        ("[video sink] syncing state (%s) to parent %s %s (%s, %s)\n",
         gst_element_state_get_name (GST_STATE (m_pstVideoQueueElement)),
         gst_element_get_name (parent), gst_element_state_get_name (target),
         gst_element_state_get_name (parent_current),
         gst_element_state_get_name (parent_pending));
      }

      gst_element_sync_state_with_parent (m_pstVideoQueueElement);
      // link audio queue and audio sink
      gst_element_link (m_pstVideoQueueElement, m_pstVideoDecoderElement);
      // change state sub-title sink
      gst_element_sync_state_with_parent (m_pstVideoDecoderElement);
    }

  }

  m_eContentType =
    (CUSTOM_CONTENT_TYPE_T) (m_eContentType | CONTENT_TYPE_VIDEO);

exit_addVideoPad:

  if (sinkpad != NULL)
    gst_object_unref (sinkpad);

  return;
}

MEDIA_STATUS_T
CustomPipeline::_setPropertyOnVideo (void)
{
  MEDIA_STATUS_T retVal = MEDIA_OK;

  if (m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_SCREEN_SHARE) {
    //widi property
    g_object_set (G_OBJECT (m_pstVideoDecoderElement), "app-type", "RTC", NULL);

//#if (PLATFORM_TYPE == LG_PLATFORM)
    g_object_set (G_OBJECT (m_pstVideoDecoderElement), "lipsync-offset", 33, NULL);
    g_object_set (G_OBJECT (m_pstVideoDecoderElement), "vsink-async-handling",
                  FALSE, NULL);
    g_object_set (G_OBJECT (m_pstVideoDecoderElement), "vsink-sync-on-clock", FALSE,
                  NULL);
    g_object_set (G_OBJECT (m_pstVideoDecoderElement), "low-delay", TRUE, NULL);
//#endif
  }
  // set 3D Type
  LMF_DBG_PRINT ("[%s:%d] 3D type : %d \n", __FUNCTION__, __LINE__,
                 m_stContentInfo.videoDataInfo.interleaving_type);
  if (m_stContentInfo.videoDataInfo.interleaving_type != MEDIA_3D_NONE) {
    if ((g_object_class_find_property (G_OBJECT_GET_CLASS
                                       (m_pstVideoDecoderElement), "interleaving_type")))
      g_object_set (G_OBJECT (m_pstVideoDecoderElement), "interleaving_type",
                    (m_stContentInfo.videoDataInfo.interleaving_type), NULL);
    else
      LMF_DBG_PRINT ("[%s:%d] There is no 'interleaving_type' property.\n",
                     __FUNCTION__, __LINE__);
  }
  /*
  #if (PLATFORM_TYPE == MTK_PLATFORM)  // still not use MTK platform in webOS
    // set seamless play
    if(pPipeInformation->contentInfo.videoDataInfo.adaptiveInfo.bSeamlessPlay == TRUE)
    {

      gboolean seamlessPlay = pPipeInformation->contentInfo.videoDataInfo.adaptiveInfo.bSeamlessPlay;

      guint maxWidth      = pPipeInformation->contentInfo.videoDataInfo.adaptiveInfo.maxWidth;
      guint maxHeight     = pPipeInformation->contentInfo.videoDataInfo.adaptiveInfo.maxHeight;

      LMF_DBG_PRINT("[%s:%d] Seamless TRUE : %d x %d\n", __FUNCTION__, __LINE__, maxWidth, maxHeight);

      // set seemless-play
      if ((g_object_class_find_property (G_OBJECT_GET_CLASS (pPipeInformation->pVideoDecoderElement), "seamless-play")))
        g_object_set(G_OBJECT(pPipeInformation->pVideoDecoderElement), "seamless-play", seamlessPlay, NULL);
      else
        LMF_DBG_PRINT("[%s:%d] There is no [seamless-play] property\n", __FUNCTION__, __LINE__);

      // set seemless-width
      if ((g_object_class_find_property (G_OBJECT_GET_CLASS (pPipeInformation->pVideoDecoderElement), "seamless-width")))
        g_object_set(G_OBJECT(pPipeInformation->pVideoDecoderElement), "seamless-width", maxWidth, NULL);
      else
        LMF_DBG_PRINT("[%s:%d] There is no [seamless-width] property\n", __FUNCTION__, __LINE__);

      // set seemless-height
      if ((g_object_class_find_property (G_OBJECT_GET_CLASS (pPipeInformation->pVideoDecoderElement), "seamless-height")))
        g_object_set(G_OBJECT(pPipeInformation->pVideoDecoderElement), "seamless-height", maxHeight, NULL);
      else
        LMF_DBG_PRINT("[%s:%d] There is no [seamless-height] property\n", __FUNCTION__, __LINE__);
    }
    else
    {
      LMF_DBG_PRINT("[%s:%d] This is not adaptive Streaming case\n", __FUNCTION__, __LINE__);
    }
  #endif
  */
  return retVal;
}

MEDIA_STATUS_T
CustomPipeline::_addVideoElement (void)
{
  MEDIA_STATUS_T retVal = MEDIA_ERROR;

  if (!m_pstVideoQueueElement) {
    LMF_DBG_PRINT ("%s.%d: pVideoQueueElement is NULL -> Start to create\n",
                   __FUNCTION__, __LINE__);

    m_pstVideoQueueElement = gst_element_factory_make ("queue", "video-queue");
    if (m_pstVideoQueueElement) {
      LMF_DBG_PRINT ("%s.%d: Video queue can not be created!!!\n", __FUNCTION__,
                     __LINE__);
      return retVal;
    }

    g_object_set (G_OBJECT (m_pstVideoQueueElement),
                  "max-size-bytes", (guint) MEDIA_QUEUE_MAX_SIZE,
                  "max-size-time", (guint64) 0, "max-size-buffers", (guint) 0, NULL);

    gst_bin_add (GST_BIN (m_pstPipeline), m_pstVideoQueueElement);
  }

  if (!m_pstVideoDecoderElement) {
    LMF_DBG_PRINT ("%s.%d: pVideoDecoderElement is NULL -> Start to create\n",
                   __FUNCTION__, __LINE__);

    m_pstVideoDecoderElement = gst_element_factory_make ("vdecsink", "video-sink");
    if (!m_pstVideoDecoderElement) {
      LMF_DBG_PRINT ("%s.%d: Video decoder can not be created!!!\n",
                     __FUNCTION__, __LINE__);
      return retVal;
    }

    /* changbok:Todo check callback compile error */
    g_signal_connect(m_pstVideoDecoderElement, "video-underrun", G_CALLBACK(_underrunSignalCb), this);

    gst_bin_add (GST_BIN (m_pstPipeline), m_pstVideoDecoderElement);
  }

  retVal = MEDIA_OK;
  return retVal;
}

void
CustomPipeline::_addAudioPad (GstStructure * capsStr, GstPad * pad)
{
  int number = 0;
  const gchar *mime;
  MEDIA_STATUS_T retVal = MEDIA_ERROR;

  gint channelsInfo, samplerate, bitrate, codecId, bitspercodedsample;
  guint pid_num = -1; //for multi track

  number = m_audCount;
  LMF_DBG_PRINT("%s.%d: audio track %d\n", __FUNCTION__, __LINE__, number);

  m_pstAudioSrcPad[number] = pad;

  gst_structure_get_int (capsStr, "channels", &channelsInfo);
  gst_structure_get_int (capsStr, "rate", &samplerate);
  gst_structure_get_int (capsStr, "bitrate", &bitrate);
  gst_structure_get_int (capsStr, "format", &codecId);
  gst_structure_get_int (capsStr, "depth", &bitspercodedsample);
  gst_structure_get_uint (capsStr, "user-audio-track-num", &pid_num);

  LMF_DBG_PRINT("[Audio pid num check] PID Num = %d\n", pid_num);
  LMF_DBG_PRINT("channels %d, s_rate %d, bitrate %d, c_id: %d, bitspersample: %d\n", channelsInfo, samplerate, bitrate, codecId, bitspercodedsample);
  LMF_DBG_PRINT("[Audio stream Num] Aud Num = %d\n", number);

  if ((mime = gst_structure_get_name(capsStr)) == NULL)
  {
    LMF_ERR_PRINT("%s:%d] unable to get MIME type\n", __FUNCTION__, __LINE__);
    goto exit_addAudioPad;
  }

  LMF_DBG_PRINT("%s:%d] mime = %s\n", __FUNCTION__, __LINE__, mime);

  //FF1 : Add Audio elemt and set Property
  retVal = _addAudioElement(number);

  if(retVal == MEDIA_ERROR)
  {
    LMF_ERR_PRINT("%s.%d: _addAudioElement() return Error !!!\n", __FUNCTION__, __LINE__);
    goto exit_addAudioPad;
  }

  //In Error case, before using 'sink pad', Returns immediately without 'unref'.
  if(number == 0)   // Temp : first track only.
  {
    LMF_DBG_PRINT("%s.%d: Linking audio pad to audio decoder\n", __FUNCTION__, __LINE__);

    //FF2 : Add SinkPad
    retVal = _addAudioSink(number,mime,pad,pid_num);

    if(retVal == MEDIA_ERROR)
    {
      LMF_ERR_PRINT("%s.%d: _addAudioSink() return Error !!!\n", __FUNCTION__, __LINE__);
      goto exit_addAudioPad;
    }
  }

  else if (number!= 0)     //for multi auido track
  {
    if(!m_pstAudioSelectorElement)
    {
      LMF_DBG_PRINT("%s.%d: Audio Selector Element is NULL!!!\n", __FUNCTION__, __LINE__);
      goto exit_addAudioPad;
    }
    else
    {
      //FF2 : Add SinkPad
      retVal = _addAudioSink(number,mime,pad,pid_num);

      if(retVal == MEDIA_ERROR)
      {
        LMF_ERR_PRINT("%s.%d: _LMF_STATIC_COMM_AddAudioSink() return Error !!!\n", __FUNCTION__, __LINE__);
        goto exit_addAudioPad;
      }
    }
  }

  m_audCount++;
  m_eContentType = (CUSTOM_CONTENT_TYPE_T)(m_eContentType |CONTENT_TYPE_AUDIO);

exit_addAudioPad:

//    if( sinkpad != NULL ) gst_object_unref(sinkpad);

  return;
}

MEDIA_STATUS_T
CustomPipeline::_addAudioElement(gint audio_num)
{
  MEDIA_STATUS_T retVal = MEDIA_ERROR;

  if(!m_pstAudioSelectorElement)
  {
    LMF_DBG_PRINT("%s.%d: AudioSelectorElement is NULL -> Start to create\n", __FUNCTION__, __LINE__);

    LMF_DBG_PRINT("Use LG Selector Bin \n ");
    m_pstAudioSelectorElement = gst_element_factory_make("lgselectorbin", NULL);

    if (!m_pstAudioSelectorElement)
    {
      LMF_DBG_PRINT("%s.%d: Audio Selector could not be created!!!\n", __FUNCTION__, __LINE__);
      return retVal;
    }
    gst_bin_add(GST_BIN(m_pstPipeline), m_pstAudioSelectorElement);
  }

  if(audio_num == 0)
  {
    if(!m_pstAudioQueueElement)
    {
      LMF_DBG_PRINT("%s.%d: AudioQueueElement is NULL -> Start to create\n", __FUNCTION__, __LINE__);

      // need audio queue.
      m_pstAudioQueueElement = gst_element_factory_make("queue", "audio-queue");
      if (!m_pstAudioQueueElement)
      {
        LMF_DBG_PRINT("%s.%d: Audio queue could not be created!!!\n", __FUNCTION__, __LINE__);
        return retVal;
      }

      g_object_set(G_OBJECT(m_pstAudioQueueElement),
                   "max-size-bytes", (guint)MEDIA_QUEUE_MAX_SIZE,
                   "max-size-time",  (guint64)0,
                   "max-size-buffers", (guint)0,
                   NULL);

      gst_bin_add(GST_BIN(m_pstPipeline), m_pstAudioQueueElement);
    }

    if(!m_pstAudioDecoderElement)
    {
      LMF_DBG_PRINT("%s.%d: AudioDecoderElement is NULL -> Start to create\n", __FUNCTION__, __LINE__);

      // make audio sink element
      m_pstAudioDecoderElement = gst_element_factory_make("adecsink", "audio-sink");
      if (!m_pstAudioDecoderElement)
      {
        LMF_DBG_PRINT("%s.%d: ADEC sink could not be created!!!\n", __FUNCTION__, __LINE__);
        \
        return retVal;
      }
      g_signal_connect(m_pstAudioDecoderElement, "audio-underrun", G_CALLBACK(_underrunSignalCb), this);

      gst_bin_add(GST_BIN(m_pstPipeline), m_pstAudioDecoderElement);
    }

//#if (PLATFORM_TYPE == LG_PLATFORM)
    if(m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_SCREEN_SHARE)
    {
      //widi property
      g_object_set(G_OBJECT(m_pstAudioDecoderElement), "app-type","RTC", NULL);

//#if (PLATFORM_TYPE == LG_PLATFORM)
      g_object_set(G_OBJECT(m_pstAudioDecoderElement), "lipsync-offset",90, NULL);
//#endif
    }
//#endif
#if 0  // still not use MTK platform in webOS
#if (PLATFORM_TYPE == MTK_PLATFORM)
    g_object_set (G_OBJECT(gPipelineInfo[pPipeInformation->ch].m_pstAudioDecoderElement), "serverside-trickplay", FALSE, NULL);
#endif
#endif
  }

  return MEDIA_OK;
}

MEDIA_STATUS_T
CustomPipeline::_addAudioSink(gint audio_num, const gchar *mime_type, GstPad *pad, guint pid_num)
{
  MEDIA_STATUS_T retVal = MEDIA_ERROR;
  GstPad *sinkpad = NULL;

  if(audio_num == 0)
  {
    sinkpad = gst_element_get_request_pad(m_pstAudioSelectorElement, "sink%d");
    m_pstAudioSinkPad[audio_num] = sinkpad;
    if (sinkpad == NULL)
    {
      LMF_DBG_PRINT("%s.%d: Audio Selector Pad is NULL!!!\n", __FUNCTION__, __LINE__);

      retVal = MEDIA_ERROR;
      goto exit_addAudioSink;
    }

    //Set Caps on Audio Sink
    retVal = _setCapsOnAudioSink(mime_type,sinkpad);

    if (GST_PAD_LINK_OK != gst_pad_link(pad, sinkpad))
    {
      if(m_pstAudioSelectorElement != NULL)
        LMF_DBG_PRINT("%s.%d: link audio Selector was failed!!!!\n", __FUNCTION__, __LINE__);
      else
        LMF_DBG_PRINT("%s.%d: link audio queue was failed!!!!\n", __FUNCTION__, __LINE__);
      // goto exit_addPad;
    }
    else
    {
      GstElement *parent;
      GstState  target;

#if 0
#if (PLATFORM_TYPE == MTK_PLATFORM)

      if((pPipeInformation->srcType != LMF_MEDIA_SRC_TYPE_FILE) &&
          (pPipeInformation->srcType != LMF_MEDIA_SRC_TYPE_SCREEN_SHARE) &&
          (pPipeInformation->srcType != LMF_MEDIA_SRC_TYPE_FCC) &&
          (pPipeInformation->srcType != LMF_MEDIA_SRC_TYPE_RTP_MCAST))
      {
        LMF_DBG_PRINT("%s.%d: link audio selector was successed...\n", __FUNCTION__, __LINE__);
        LMF_STATIC_COMM_SetLanguage(pPipeInformation->ch, audio_num);
        gst_element_link_many(pPipeInformation->m_pstAudioSelectorElement, pPipeInformation->m_pstAudioQueueElement, pPipeInformation->m_pstAudioDecoderElement, NULL);
        pPipeInformation->last_pid = pid_num;
      }
      else
#endif
#endif
      {
        if ((parent = GST_ELEMENT_CAST (gst_element_get_parent (m_pstAudioSelectorElement))))
        {
          GstState parent_current, parent_pending;

          GST_OBJECT_LOCK (parent);
          parent_current = GST_STATE (parent);
          parent_pending = GST_STATE_PENDING (parent);
          GST_OBJECT_UNLOCK (parent);

          /* set to pending if there is one, else we set it to the current state of
          * the parent */
          if (parent_pending != GST_STATE_VOID_PENDING)
            target = parent_pending;
          else
            target = parent_current;

          LMF_DBG_PRINT ("[audio sink] syncing state (%s) to parent %s %s (%s, %s)\n",
                         gst_element_state_get_name (GST_STATE (m_pstAudioQueueElement)),
                         gst_element_get_name (parent), gst_element_state_get_name (target),
                         gst_element_state_get_name (parent_current),
                         gst_element_state_get_name (parent_pending));
        }

        gst_element_sync_state_with_parent(m_pstAudioSelectorElement);
        // link audio selector, queue, audio sink
        gst_element_link_many(m_pstAudioSelectorElement, m_pstAudioQueueElement, m_pstAudioDecoderElement, NULL);
        // change state sub-title sink
        gst_element_sync_state_with_parent(m_pstAudioQueueElement);
        gst_element_sync_state_with_parent(m_pstAudioDecoderElement);

      }

    }
  }
  else if (audio_num != 0)     //for multi auido track
  {
    sinkpad = gst_element_get_request_pad(m_pstAudioSelectorElement, "sink%d");
    if (sinkpad == NULL)
    {
      LMF_DBG_PRINT("%s.%d: Audio Selector Pad is NULL!!!\n", __FUNCTION__, __LINE__);

      retVal = MEDIA_ERROR;
      goto exit_addAudioSink;
    }
    m_pstAudioSinkPad[audio_num] = sinkpad;
    LMF_DBG_PRINT("[%s:%d]pid num = %d, audioNum = %d\n", __FUNCTION__, __LINE__, pid_num, audio_num);

    //Set Caps on Audio Sink
    _setCapsOnAudioSink(mime_type,sinkpad);

    if (GST_PAD_LINK_OK != gst_pad_link(pad, sinkpad))
    {
      LMF_DBG_PRINT("%s.%d: link audio Selector %d sinkpad was failed!!!!\n", __FUNCTION__, __LINE__, audio_num);
    }
    else if(pid_num < m_lastPid)
    {
      LMF_DBG_PRINT("%s.%d: link audio Selector %d sinkpad was successed!!!!\n", __FUNCTION__, __LINE__, audio_num);
      SetLanguage(audio_num);   //multi track
      m_lastPid = pid_num;
    }
  }

  //Todo : if 'error' is not occurred, return value should be MEDIA_OK
  retVal = MEDIA_ERROR;

exit_addAudioSink:
  if( sinkpad != NULL ) gst_object_unref(sinkpad);

  return  retVal;
}

MEDIA_STATUS_T
CustomPipeline::_setCapsOnAudioSink(const char *mime_type, GstPad *sinkpad)
{
  GstCaps* pCaps = NULL;
  gint sampleRate = 0;
  MEDIA_AUDIO_PCM_INFO_T *pAudioInfo;

  if ((strstr(mime_type, "audio/x-lpcm")))// && (gbWifiDebugMode == FALSE)) //Todo: changbok. need to check wifi debugmode
  {
    pAudioInfo = &(m_stContentInfo.audioDataInfo.audio_data.pcmInfo);
    sampleRate = _transSampleRate(pAudioInfo->sampleRate);

    LMF_DBG_PRINT("[%s:%d]x-lpcm : need to set the caps info manually.\n", __FUNCTION__, __LINE__);
    LMF_DBG_PRINT("[x-lpcm] sampleRate: 	%d\n", sampleRate);
    LMF_DBG_PRINT("[x-lpcm] bitsPerSample:	%d\n", pAudioInfo->bitsPerSample);
    LMF_DBG_PRINT("[x-lpcm] channelMode:	%d\n", pAudioInfo->channelMode);

//#if (PLATFORM_TYPE == LG_PLATFORM)
    pCaps = gst_caps_new_simple( "audio/x-raw-int",
                                 "rate",     G_TYPE_INT,   sampleRate,
                                 "channels",   G_TYPE_INT,   (pAudioInfo->channelMode == MEDIA_AUDIO_PCM_MONO)?(1):(2), // num of ch
                                 "depth",    G_TYPE_INT,   (pAudioInfo->bitsPerSample == MEDIA_AUDIO_16BIT)?(16):(8),
//                      "width",    G_TYPE_INT,   (pAudioInfo->bitsPerSample == HOA_AUDIO_16BIT)?(16):(8),
                                 "endianness", G_TYPE_INT,   4321,
                                 NULL );
    /*  //still not use MTK platform
    #else // TODO : MTK
        pCaps = gst_caps_new_simple( "audio/x-raw-int",
                      "rate",     G_TYPE_INT,   sampleRate,
                      "channels",   G_TYPE_INT,   (pAudioInfo->channelMode == HOA_AUDIO_PCM_MONO)?(1):(2),  // num of ch
                      "depth",    G_TYPE_INT,   (pAudioInfo->bitsPerSample == HOA_AUDIO_16BIT)?(16):(8),
                      "width",    G_TYPE_INT,   (pAudioInfo->bitsPerSample == HOA_AUDIO_16BIT)?(16):(8),
                      "signed",   G_TYPE_BOOLEAN, TRUE,
                      "endianness", G_TYPE_INT,   4321,
                      NULL );
    #endif
    */
    gst_pad_set_caps(sinkpad, pCaps);
    gst_pad_use_fixed_caps(sinkpad);
    gst_caps_unref(pCaps);
  }

  return MEDIA_OK;

}

void
CustomPipeline::_addTextPad(GstPad *pad)
{
  GstPad *sinkpad = NULL;

  // make sub-title sink element
  pSubtitleSinkElement = gst_element_factory_make("subtsink", "text_sink");
  if (!pSubtitleSinkElement)
  {
    LMF_DBG_PRINT("[%s][%d]: Subtitle sink could not be created!!!\n", __FUNCTION__, __LINE__);
    goto exit_addTextPad;
  }

  //Only avi.
  g_object_set(G_OBJECT(pSubtitleSinkElement)
               , "subt-type", 1  //AVI 1, MKV 2
               , "xsub-type", 1  // AVI MODE Only.. LMF_FOURCC('D','X','S','A') 1, LMF_FOURCC('D','X','S','B') 2
               , "txt-position", 0 // MKV MODE Only
               , NULL);

  gst_bin_add(GST_BIN (m_pstPipeline), pSubtitleSinkElement);
  sinkpad = gst_element_get_static_pad(pSubtitleSinkElement, "sink");
  if (GST_PAD_LINK_OK != gst_pad_link(pad, sinkpad))
  {
    LMF_DBG_PRINT("%s.%d: link subtitle sink was failed!!!!\n", __FUNCTION__, __LINE__);
  }
  else
  {
    gst_element_sync_state_with_parent(pSubtitleSinkElement);
  }

exit_addTextPad:

  if( sinkpad != NULL ) gst_object_unref(sinkpad);

  return;
}

void
CustomPipeline::_findDemux (gchar ** pDemuxName)
{
  int checkCnt = 0;

  LMF_DBG_PRINT ("[%s:%d] input file container (%d)\n", __FUNCTION__, __LINE__, m_stContentInfo.container);

  while (m_stDmxList[checkCnt].contentType != 0xFF) {
    if (m_stDmxList[checkCnt].contentType == (guint8) m_stContentInfo.container) {
      LMF_DBG_PRINT ("[%s:%d] Find the demux [%d : %s]\n", __FUNCTION__,  __LINE__, m_stContentInfo.container,  m_stDmxList[checkCnt].pElementName);
      *pDemuxName = m_stDmxList[checkCnt].pElementName;
      break;
    }
    checkCnt++;
  }
}

MEDIA_STATUS_T
CustomPipeline::_addDemuxElement (void)
{
  gchar *pDemuxName = NULL;

  // find demux element
  _findDemux (&pDemuxName);

  if (pDemuxName == NULL) {
    LMF_DBG_PRINT ("[%s:%d] demux find fail! container:%d\n", __FUNCTION__,  __LINE__, m_stContentInfo.container);
    return MEDIA_ERROR;
  }

  // create demux element
  m_pstDemuxElement = gst_element_factory_make (pDemuxName, NULL);
  if (!m_pstDemuxElement) {
    LMF_DBG_PRINT ("[%s:%d] demux element(%s) could not be created!\n",__FUNCTION__, __LINE__, pDemuxName);
    return MEDIA_ERROR;
  }

#if 0 /* changbok - webos refactory - temp [ need to organize NC4.0 ifdef statements ] */
#ifdef INCLUDE_ACTVILA
  //Create shared memory for tsdemux and set the memory address property of demux
  if (g_strrstr (pDemuxName, "mpegtsdemux"))    // 20120119
  {
    //AF_SHM_CreateSharedMemory(pHndl,152);
    LMF_DBG_PRINT ("[%s:%d][src:%d] pPESDataBuffer address  pass set \n",
                   __FUNCTION__, __LINE__);
    g_object_set (G_OBJECT (gPipelineInfo[ch].pDemuxElement), "mem-add",
                  &pPESDataBuffer, NULL);
  }
#endif //INCLUDE_ACTVILA
#endif

  // add & link demux element
  gst_bin_add (GST_BIN (m_pstPipeline), m_pstDemuxElement);

  if (m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_SCREEN_SHARE) {
    //widi property
    g_object_set (G_OBJECT (m_pstDemuxElement), "app-type", "RTC", NULL);
    gst_element_link (pDepayElement, m_pstDemuxElement);
  } else if ((m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_FCC) || (m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_RTP_MCAST))
  {

#if 0 /* changbok - webos refactory - temp [ need to organize NC4.0 ifdef statements ] */
#ifdef INCLUDE_VERIMATRIX
#ifdef INCLUDE_DRM_MODULE
    if (contentInfo.drmType == LMF_DRM_TYPE_VERIMATRIX)
#else
    if (contentInfo.drmType == DRM_TYPE_VERIMATRIX)
#endif
    {
      gst_element_link (gPipelineInfo[ch].pDrmElement,
                        gPipelineInfo[ch].pDemuxElement);
    } else {
      gst_element_link (gPipelineInfo[ch].srcInfo[IDX_MULTI].pSrcElement,
                        gPipelineInfo[ch].pDemuxElement);
    }
#endif
#endif
  } else {
    gst_element_link (m_stSrcInfo[IDX_MULTI].pSrcElement, m_pstDemuxElement);
  }

  /* changbok:Todo check callback compile error */
//  g_signal_connect(gPipelineInfo.pDemuxElement, "pad-added", G_CALLBACK(_LMF_STATIC_AddNewPad_MTK), &gPipelineInfo);
  g_signal_connect(m_pstDemuxElement, "pad-added", G_CALLBACK(_addNewPad),this);

  /*  changbok - webos refactory - temp [ need to organize NC4.0 ifdef statements ]
      if(m_eSrcType == MEDIA_CUSTOM_SRC_TYPE_FCC)
      {
          g_signal_connect(G_OBJECT(m_pstDemuxElement), "pad-removed", G_CALLBACK(LMF_STATIC_OTHER_RemovePad_for_Fcc), this);
      }
  */
  return MEDIA_OK;
}

gboolean
CustomPipeline::SetLanguage(gint32 audioNum)
{
  GstPad *sinkpad;
  if ((audioNum == -1)||(audioNum == -2))
    audioNum = 0;
  LMF_DBG_PRINT("setting language to %d \r\n", audioNum);
  LMF_DBG_PRINT("%s.%d: found selector's pad...\n", __FUNCTION__, __LINE__);
  sinkpad = m_pstAudioSinkPad[audioNum];
  if (sinkpad)
  {
    LMF_DBG_PRINT("%s.%d: Selector current sinkpad was successed...\n", __FUNCTION__, __LINE__);
    m_audCurrent = audioNum;
    g_object_set(G_OBJECT(m_pstAudioSelectorElement), "active-pad", sinkpad, NULL);
  }
  else
    return FALSE;
  return TRUE;
}

MEDIA_STATUS_T
CustomPipeline::FeedStream (guint8 * pBuffer, guint32 bufferSize, guint64 pts,
                            MEDIA_DATA_CHANNEL_T esData)
{
  GstBuffer *pAppbuf = NULL;
  GstFlowReturn ret;

  guint8 *pFeedBuf = NULL;
  guint32 feedSize = 0;
  guint64 timestamp = 0;        // to decode the raw data

  guint8 srcIdx = IDX_MULTI;
  static guint8 feedIndex = 0;  // to check prebuffering state
  static unsigned long long tempTimestamp[IDX_MAX] = { 0, };    // to do the MR TEST
  static gint _idx[IDX_MAX] = { 0, };

  if (m_pstPipeline == NULL)
    LOG_FUNCTION_SCOPE_NORMAL_D
    ("[CustomPipeline][FeedStream]pipeline is NULL \n");

  /* changbok:Todo  */
}


gint64
CustomPipeline::duration () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gint64
CustomPipeline::position () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gint
CustomPipeline::volume () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gboolean
CustomPipeline::isMuted () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gboolean
CustomPipeline::isAudioAvailable () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gboolean
CustomPipeline::isVideoAvailable () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gboolean
CustomPipeline::isSeekable () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

gfloat
CustomPipeline::playbackRate () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

GString
CustomPipeline::errorString () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}

void
CustomPipeline::playbinNotifySource (GObject * pObject, GParamSpec * pParam,
                                     gpointer u_data)
{
  CustomPipeline *genericPipeline =
    reinterpret_cast < CustomPipeline * >(u_data);

  if (g_object_class_find_property (G_OBJECT_GET_CLASS (pObject), "source")) {
    GObject *pSrcElement = NULL;
    GstBaseSrc *pBaseSrc = NULL;
    GstElementFactory *pFactory = NULL;
    const gchar *pName = NULL;

    // get source element
    g_object_get (pObject, "source", &pSrcElement, NULL);

    //check soup http src
    pFactory = gst_element_get_factory ((GstElement *) pSrcElement);
    pName = gst_plugin_feature_get_name (GST_PLUGIN_FEATURE (pFactory));

    if (!strcmp (pName, "souphttpsrc")) // do only http:
    {
      LOG_FUNCTION_SCOPE_NORMAL_D
      ("[GST_SIGNAL] ##### found soup http src. !!! ##### \n");
      if (g_object_class_find_property (G_OBJECT_GET_CLASS (pSrcElement),
                                        "cookies")
          && g_object_class_find_property (G_OBJECT_GET_CLASS (pSrcElement),
                                           "user-agent")) {
        //_LMF_PLAYBIN2_SetHttpHeader(pPlayerHandle, (GstElement *)pSrcElement, pPlayerHandle->cookies, pPlayerHandle->userAgent, pPlayerHandle->extraHeader);
      } else {
        LOG_FUNCTION_SCOPE_NORMAL_D
        ("[GST_SIGNAL]source has no property named \"cookies\" or \"user-agent\"\n");
      }

      if (g_object_class_find_property (G_OBJECT_GET_CLASS (pSrcElement),
                                        "timeout")) {
        //_LMF_PLAYBIN2_SetSoupHttpSrcTimeOut(pPlayerHandle, (GstElement *)pSrcElement, pPlayerHandle->SoupHttpSrcTimeOut);
      } else {
        LOG_FUNCTION_SCOPE_NORMAL_D
        ("[GST_SIGNAL]source has no property named \"timeout\" \n");
      }
    }
#if 0                           //TODO: ./src/libpf.so: undefined reference to `gst_base_src_get_type'
    pBaseSrc = GST_BASE_SRC (pSrcElement);

    if (g_object_class_find_property (G_OBJECT_GET_CLASS (pBaseSrc),
                                      "blocksize")) {
      LOG_FUNCTION_SCOPE_NORMAL_D
      ("[GST_SIGNAL] blocksize property found: setting to 24KB\n");
      g_object_set (G_OBJECT (pBaseSrc), "blocksize", (gulong) (24 * 1024),
                    NULL);
    } else {
      LOG_FUNCTION_SCOPE_NORMAL_D
      ("[GST_SIGNAL] basesrc has no property named 'blocksize'. \n");
    }
#endif
    gst_object_unref (pSrcElement);
  }

}

void
CustomPipeline::handleVolumeChange (GObject * pObject, GParamSpec * pParam,
                                    gpointer u_data)
{


}

void
CustomPipeline::handleMutedChange (GObject * pObject, GParamSpec * pParam,
                                   gpointer u_data)
{


}
void CustomPipeline::setInterleavingTypeSpi(gpointer data, GstObject *pObj, gint stream, gpointer user_data)
{

}

void CustomPipeline::getUndecodedSizeSpi(gpointer data, guint64* pVdecBufferedSize, guint64* pAdecBufferedSize)
{

}

gboolean CustomPipeline::isReadyToPlaySpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

  /* TODO :
      // static player case
      g_print("[%s:%d] Check Static Case\n", __FUNCTION__, __LINE__);
      if (GST_STATIC_IsPrerolling(ch))
          return FALSE;        // prebuffering 중...
      else
          return TRUE;
  */
  return TRUE;
}
void CustomPipeline::getStreamsInfoSpi(gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

  return;
}
gboolean CustomPipeline::updateVideoInfoSpi(gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

  return true;
}
void CustomPipeline::handleStateMsgPauseSpi_pre(gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}
void CustomPipeline::correctBufferedBytesSpi(gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}
void CustomPipeline::videoDecodeUnderrunCbSpi(GstElement *pObj, gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}
void CustomPipeline::audioDecodeUnderrunCbSpi(GstElement *pObj, gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

}


gboolean CustomPipeline::checkTimeToDecodeSpi(gpointer data)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPipeline");

#if 0 // TODO ...
  LMF_STATUS_T retVal = LMF_OK;
  GstClockTime start_time, base_time, curTime, curTime2;
  GstClock *cur_clock, *new_clock;

  MF_ENTER_FUNC();

  cur_clock = NULL;
  new_clock = NULL;
  start_time = base_time = curTime = curTime2 = 0;

  if ((gPipelineInfo[ch].contentInfo.ptsToDecode > 0) && (gPipelineInfo[ch].srcType == LMF_MEDIA_SRC_TYPE_ES))
  {
    g_print("[%s:%d][Ch:%d] set start pts time = %lld\n", __FUNCTION__, __LINE__, ch, gPipelineInfo[ch].contentInfo.ptsToDecode);

    base_time = gst_element_get_base_time(gPipelineInfo[ch].pPipeline);
    start_time = gst_element_get_start_time(gPipelineInfo[ch].pPipeline);
    cur_clock = gst_element_get_clock (gPipelineInfo[ch].pPipeline);

    if (cur_clock) {
      curTime = gst_clock_get_time (cur_clock);
      curTime2 = curTime - gst_element_get_base_time (gPipelineInfo[ch].pPipeline);
      gst_object_unref (cur_clock);
    }

    g_print("[Before] base_time = %lld, start_time = %lld, cur = %lld, %lld\n", base_time, start_time, curTime, curTime2);

    start_time += gPipelineInfo[ch].contentInfo.ptsToDecode;

    gst_element_set_start_time(gPipelineInfo[ch].pPipeline, start_time);
    //gst_element_set_base_time(gPipelineInfo[ch].pPipeline, GST_CLOCK_TIME_NONE);

    if (gPipelineInfo[ch].pAudioDecoderElement != NULL)
      gst_element_set_start_time(gPipelineInfo[ch].pAudioDecoderElement, start_time);

    if (gPipelineInfo[ch].pVideoDecoderElement != NULL)
      gst_element_set_start_time(gPipelineInfo[ch].pVideoDecoderElement, start_time);

    if (gPipelineInfo[ch].pAudioQueueElement != NULL)
      gst_element_set_start_time(gPipelineInfo[ch].pAudioQueueElement, start_time);

    if (gPipelineInfo[ch].pVideoQueueElement != NULL)
      gst_element_set_start_time(gPipelineInfo[ch].pVideoQueueElement, start_time);

    if (gPipelineInfo[ch].pDemuxElement!= NULL)
      gst_element_set_start_time(gPipelineInfo[ch].pDemuxElement, start_time);

    if (gPipelineInfo[ch].srcInfo[IDX_VIDEO].pSrcElement != NULL)
      gst_element_set_start_time(gPipelineInfo[ch].srcInfo[IDX_VIDEO].pSrcElement, start_time);

    if (gPipelineInfo[ch].srcInfo[IDX_AUDIO].pSrcElement != NULL)
      gst_element_set_start_time(gPipelineInfo[ch].srcInfo[IDX_AUDIO].pSrcElement, start_time);

    gPipelineInfo[ch].contentInfo.ptsToDecode = 0;

    base_time = start_time = curTime = curTime2 = 0;

    base_time = gst_element_get_base_time(gPipelineInfo[ch].pPipeline);
    start_time = gst_element_get_start_time(gPipelineInfo[ch].pPipeline);
    cur_clock = gst_element_get_clock (gPipelineInfo[ch].pPipeline);

    if (cur_clock) {
      curTime = gst_clock_get_time (cur_clock);
      curTime2 = curTime - gst_element_get_base_time (gPipelineInfo[ch].pPipeline);
      gst_object_unref (cur_clock);
    }

    g_print("[After] base_time = %lld, start_time = %lld, cur = %lld, %lld\n", base_time, start_time, curTime, curTime2);
  }

  return retVal;
#endif
}

/*
* set gstreamer debug LOG level.
*/
bool CustomPipeline::setGstreamerDebugLevel (guint select, gchar * category, GstDebugLevel level)
{
  GError *err;
  if (!gst_init_check (NULL, NULL, &err)) {
    std::cout << "Error:" << err->message << endl;
    g_error_free (err);
    return false;
  }
  if (select == 1) {
    if (category != NULL)
      gst_debug_set_threshold_for_name ((const char *) category, level);
  } else {
    gst_debug_set_default_threshold (level);
  }
  return true;
}




//end of file
