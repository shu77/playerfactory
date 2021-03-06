/*
 * genericpipeline.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 *  Modified on: 2013. 5. 5.
 *      Modifier: mf-dev
 */

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include <pthread.h>
#include <gio/gio.h>
#include <cstring>
#if (GST_VERSION_MAJOR >= 1)
#include <gst/video/video.h>
#else
//#include <gst/app/gstappbuffer.h>//playbin+appsrc not support at NC5.0
#endif
#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif
#include <pipeline/generic/genericpipeline.hpp>

using namespace mediapipeline;

GenericPipeline::GenericPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");


}

GenericPipeline::~GenericPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
}

bool GenericPipeline::loadFromURI ()
{
  if (m_pipeHandle) {
    GstBus *
    pBus = NULL;
    // message를 막기 위해 flushing 한다.
    pBus = gst_element_get_bus (m_pipeHandle);
    gst_bus_set_flushing (pBus, true);
    gst_element_set_state (m_pipeHandle, GST_STATE_READY);
    gst_bus_set_flushing (pBus, false);
    gst_object_unref (pBus);

    g_object_set (G_OBJECT (m_pipeHandle), "uri", m_uri.c_str (), NULL);        //c-string copy.

    //TODO: notify stream change status. if possible.
  } else
    return false;

  return true;
}

bool GenericPipeline::handleURI ()
{
  if (m_pipeHandle) {
    GFile *
    pTmpFile = NULL;
    char *
    pNewUri = NULL;
    char *
    pTmpPath = NULL;
    std::cout << "handleURI"<< endl;

    /* get pipline options handler */
    Options::bsp_t options = getOptionsHandler ();
    /* get value from options handler */
    //options->checkKeyExistance("clipOpt.mediafile");
    //std::string filename = options->getString ("clipOpt.mediafile");
    std::string filename;
    if(options->getString ("clipOpt.mediafile", &filename) == false)
    {
      std::cout << "(clipOpt.mediafile)uri not found" << endl;
      return false;
    }
    std::cout << "URI = " << filename << endl;

    if (!strncmp (filename.c_str(), "mms://", strlen ("mms://"))) {
      // set serverside tirck enable at mms streamming.
      m_bServerSideTrick = true; // isMmsURI
    }
    // DQMS 1206-00199 (mlm can't filtering this)
    // not supported known type media filtering.
    if ((strstr (filename.c_str(), ".ogm") != NULL)
        || (strstr (filename.c_str(), ".ogv") != NULL)
        || (strstr (filename.c_str(), ".OGM") != NULL)
        || (strstr (filename.c_str(), ".OGV") != NULL)) {
      //TODO :: API_BASIC_EVENT_Notify(pPipeContainerHandle, MEDIA_CB_MSG_ERR_PLAYING);
      return false;
    }
    /* this allows non-URI type of files in the thumbnailer and so on */
    pTmpFile = g_file_new_for_commandline_arg (filename.c_str());
    if (pTmpFile == NULL) {
      pNewUri = g_strdup (filename.c_str());
    } else {
      pTmpPath = g_file_get_path (pTmpFile);
      if (pTmpPath) {
        pNewUri = g_filename_to_uri (pTmpPath, NULL, NULL);
        g_free (pTmpPath);
      } else {
        pNewUri = g_strdup (filename.c_str());
      }
      g_object_unref (pTmpFile);
    }
    m_uri.clear ();
    m_uri.append (pNewUri);
    if (pNewUri != NULL)
      g_free (pNewUri);
    filename.clear();
  } else
    return false;

  return true;
}

gboolean GenericPipeline::setExtraElementOption ()
{
  if (m_pipeHandle) {
#if 0                           //TODO: 구현..
    // DLNA info setting
    if (pPipeContainerHandle->isDLNA) {
      g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline),
                    "dlna-protocolinfo",
                    (gchar *) (pPipeContainerHandle->pDlnaInfo->pProtocolInfo), NULL);
      g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline),
                    "dlna-contentlength",
                    (guint64) (pPipeContainerHandle->pDlnaInfo->dContentLength), NULL);
      g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline),
                    "dlna-cleartextsize",
                    (guint64) (pPipeContainerHandle->pDlnaInfo->dCleartextSize), NULL);
      g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline), "dlna-opval",
                    (guint32) (pPipeContainerHandle->pDlnaInfo->opVal), NULL);
      g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline), "dlna-flagval",
                    (guint32) (pPipeContainerHandle->pDlnaInfo->flagVal), NULL);
    }

    /* DRM type setting */
    //cout << "drm-type:", pPipeContainerHandle->drmType;
    g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline), "drm-type",
                  (gint) pPipeContainerHandle->drmType, NULL);
#endif
  } else
    return false;

  return true;
}

gboolean GenericPipeline::loadSpi_pre ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");

  m_pipeHandle = gst_element_factory_make ("playbin2", NULL);
  if (m_pipeHandle) {
    m_playbinVersion = 2;       //playbin2
    int flags = 0;
    //GST_PLAY_FLAG_NATIVE_VIDEO omits configuration of ffmpegcolorspace and videoscale
    g_object_get (G_OBJECT (m_pipeHandle), "flags", &flags, NULL);
    flags |= GST_PLAY_FLAG_NATIVE_VIDEO;
    g_object_set (G_OBJECT (m_pipeHandle), "flags", flags, NULL);
    cout << "genericpipeline] created playbin2 pipeline" << endl;
  } else {
    m_pipeHandle = gst_element_factory_make ("playbin", NULL);
    if (m_pipeHandle) {
      cout << "genericpipeline] created playbin pipeline" << endl;
      m_playbinVersion = 1;     //playbin
    } else {
      cout << "genericpipeline] fail create playbin pipeline" << endl;
    }
  }
  return true;
}

gboolean GenericPipeline::loadSpi_post ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");

  // connect source notify.
  g_signal_connect (G_OBJECT (m_pipeHandle), "notify::source", G_CALLBACK (playbinNotifySource), this);

  // setting values
  if (handleURI () == false)
    return false;

  // connect signals
  //m_eventHelper = new PlayEventListener();
  //connectGstBusCallback(); //move to create time.
  // start timers

  // set media
  cout << "genericpipeline] load file to play: " << m_uri << endl;
  if (loadFromURI () == false)
    return false;
  // etc
  // set DLNA info setting.


  // set pause
  if (pause () == false)
    return false;

  return true;
}

gboolean GenericPipeline::unloadSpi()
{

  return true;
}

gboolean GenericPipeline::seekSpi (gint64 posMs)
{
#if 0 //TODO.. DLNA.. seek 보정.. 
		// Check DLNA + PS + seek not ready case.
		if(_LMF_PLYR_CTRL_CheckSeekNotReady(pPlayerHandle, &posMs) == LMF_NOT_READY)
		{
			//pPlayerHandle->LastSeekPTS = -1;
			//return LMF_NOT_READY;
			position = posMs * GST_MSECOND;
			LMF_DBG_PRINT("[%s:%d] Seek to %lld(%llu)\n", __FUNCTION__, __LINE__, position, position );
		}
#endif
		if ((m_duration > 0) &&
			((posMs * GST_MSECOND) >= m_duration) )
		{
#if 0 //TODO.. DLNA 특정 case 처리 
			// dlna duration보다 demuxer에서 간혹 duration을 작게 올려주면 마지막 부분이 seek안되는 문제 fix.
			if(pPlayerHandle->isDLNA == TRUE && pPlayerHandle->dlna_duration > pPlayerHandle->SessionDuration && position<pPlayerHandle->dlna_duration )
			{// seek pos 위치가 demuxer duration 넘어서더라도. dlna_duration 범위 내면 seek.
				return seekCommon(posMs);
			}
#endif
			g_print("[%s:%d] seek pos >= dur (%"G_GINT64_FORMAT" ns)\n", __FUNCTION__, __LINE__, m_duration);

			// GStreamer 에는 seek 명령 내리지 않고, pause 시키고 position 값만 변경하여서
			// UpdatePlayPosition 에서 EOF 처리되도록 함.
			m_bEndOfFile = TRUE;
			return true;
		}
		else
			return seekCommon(posMs);

}
/* for prebuffering action */
gboolean GenericPipeline::isReadyToPlaySpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");

  // play already done -> Resume Case
  if (m_bPlaybackStarted == true) {
    cout << " Resume Case" << endl;
    return true;
  }
  //TODO for prebuffering //return false;   // prebuffering 중...
  return true;                  //temp..
}

gboolean GenericPipeline::setServerSideTrick(gpointer data, gboolean serverSideTrick)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  Pipeline *
  self = reinterpret_cast < Pipeline * >(data);

  GstElement *pAdecsink = NULL;
  if (self == NULL)
  {
    g_print("[%s:%d] Error. LMF Player Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return FALSE;
  }
  g_object_get (G_OBJECT(self->m_pipeHandle), "audio-sink", &pAdecsink, NULL);
  if (pAdecsink != NULL)
  {
    if(serverSideTrick)
    {
      g_print ("[%s] setting serverside-trickplay = TRUE !!!!! \r\n", __FUNCTION__);
      g_object_set(G_OBJECT(pAdecsink), "serverside-trickplay", TRUE, NULL);
    }
    else
    {
      g_print ("[%s] setting serverside-trickplay = FALSE !!!!! \r\n", __FUNCTION__);
      g_object_set(G_OBJECT(pAdecsink), "serverside-trickplay", FALSE, NULL);
    }
    gst_object_unref(pAdecsink);
  }
  else
  {
    g_print ("[%s] no audio sink!!!!! \r\n", __FUNCTION__);
    return FALSE;
  }
  return TRUE;
}

gboolean GenericPipeline::setPlaybackRateSpi_pre (gpointer data, gfloat rate){
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  gboolean	retVal = true;
  Pipeline *
  self = reinterpret_cast < GenericPipeline * >(data);

  if ((self == NULL) ||
  (self->m_pipeHandle == NULL) ||
  (GST_IS_ELEMENT(self->m_pipeHandle) == FALSE))
  {
    g_print("%s:%d] Error. Player Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }

  if (!self->compareDouble(self->m_playbackRate, rate))
  {
    self->m_playbackRate = rate;
    if (self->m_pipeHandle)
    {
      if (rate < (double)(0.0)) /* FastRewind */
      {
        if (self->m_bServerSideTrick) // support for server side trick playback.
          self->setServerSideTrick(self, TRUE);
      } /* FastRewind */
      else if (self->compareDouble(rate, 1.0)) /* x1.0 */
      {
        if(self->m_bServerSideTrick) // fix for maxdome FF20120112
          self->setServerSideTrick(self, FALSE);
      } /* x1.0 */
      else /* FastForward */
      {
        if(self->m_bServerSideTrick) // fix for maxdome FF20120112
          self->setServerSideTrick(self, TRUE);
      } /* FastForward */
    }
  }
  return retVal;
}

GString GenericPipeline::errorString () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");

}

void
GenericPipeline::playbinNotifySource (GObject * pObject,
                                      GParamSpec * pParam, gpointer u_data)
{
  GenericPipeline *
  genericPipeline = reinterpret_cast < GenericPipeline * >(u_data);

  if (g_object_class_find_property (G_OBJECT_GET_CLASS (pObject), "source")) {
    GObject *
    pSrcElement = NULL;
    GstBaseSrc *
    pBaseSrc = NULL;
    GstElementFactory *
    pFactory = NULL;
    const gchar *
    pName = NULL;

    // get source element
    g_object_get (pObject, "source", &pSrcElement, NULL);

    //check soup http src
    pFactory = gst_element_get_factory ((GstElement *) pSrcElement);
    pName = gst_plugin_feature_get_name (GST_PLUGIN_FEATURE (pFactory));

    if (!strcmp (pName, "souphttpsrc")) {       // do only http:
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

gboolean GenericPipeline::positionSpi(gpointer data, gint64 *pos)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  gint64 position = 0;
  
  GstFormat format = GST_FORMAT_TIME;
#if (GST_VERSION_MAJOR >= 1)
  if (self->m_pipeHandle
      && gst_element_query_position (self->m_pipeHandle, format, &position))
#else
  if (self->m_pipeHandle
      && gst_element_query_position (self->m_pipeHandle, &format, &position))
#endif
    *pos = position;
  else
  {
    pos = 0;
    return false;
  }
  return true;
}

void GenericPipeline::setInterleavingTypeSpi(gpointer data, GstObject *pObj, gint stream, gpointer user_data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  GstElement *pVideosink = NULL;

  g_print("[%s:%d] 3D type : %d \n", __FUNCTION__, __LINE__, self->m_interleavingType);

  if ((self->m_pipeHandle!= NULL))// && (pPipeContainerHandle->bUsePlaybin))
  {
    g_object_get(G_OBJECT(self->m_pipeHandle), "video-sink", &pVideosink, NULL);

    if ((pVideosink != NULL)) // && (self->bUsePlaybin))
    {
      if ((g_object_class_find_property(G_OBJECT_GET_CLASS(pVideosink), "interleaving_type")))
      {
        g_object_set(G_OBJECT(pVideosink), "interleaving_type", self->m_interleavingType, NULL);
        g_print("[%s:%d] 3D type setting ok \n", __FUNCTION__, __LINE__);
      }
      else
      {
        g_print("[%s:%d] There is no 'interleaving_type' property.\n", __FUNCTION__, __LINE__);
      }
      gst_object_unref(pVideosink);
    }
    else
    {
      g_print("[%s:%d] No Videosink or No playbin\n", __FUNCTION__, __LINE__);
    }
  }
  else
  {
    g_print("[%s:%d] No player or static \n", __FUNCTION__, __LINE__);
  }
}

void GenericPipeline::getUndecodedSizeSpi(gpointer data, guint64* pVdecBufferedSize, guint64* pAdecBufferedSize)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  guint64 vdecBufferedSize = 0;
  guint64 adecBufferedSize = 0;
  GstElement *pVdecsink = NULL;
  GstElement *pAdecsink = NULL;

  if (self == NULL)
  {
    g_print("[%s:%d] handle is NULL!!!!!!\n", __FUNCTION__, __LINE__);
    return;
  }

  g_object_get (G_OBJECT(self->m_pipeHandle), "video-sink", &pVdecsink, NULL);
  if (pVdecsink != NULL)
  {
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(pVdecsink), "undecoded-size"))
    {
      g_object_get(G_OBJECT(pVdecsink), "undecoded-size", &vdecBufferedSize, NULL);
    }
    else
    {
      g_print("[%s] no such property 'undecoded-size' in video-sink\n", __FUNCTION__);
    }

    gst_object_unref(pVdecsink);
  }
  else
  {
    g_print("[%s] no video-sink\n", __FUNCTION__);
  }

  g_object_get (G_OBJECT (self->m_pipeHandle), "audio-sink", &pAdecsink, NULL);
  if (pAdecsink != NULL)
  {
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(pAdecsink), "undecoded-size"))
    {
      g_object_get(G_OBJECT(pAdecsink), "undecoded-size", &adecBufferedSize, NULL);
    }
    else
    {
      g_print("[%s] no such property 'undecoded-size' in audio-sink\n", __FUNCTION__);
    }

    gst_object_unref(pAdecsink);
  }
  else
  {
    g_print("[%s] no audio-sink\n", __FUNCTION__);
  }

  *pVdecBufferedSize = vdecBufferedSize;
  *pAdecBufferedSize = adecBufferedSize;

}


gboolean GenericPipeline::informationMonitorStartSpi(guint32 timeInterval)
{
  if ((m_pipeHandle == NULL) || (GST_IS_ELEMENT(m_pipeHandle) == FALSE))
  {
    std::cout << "Error. Gstreamer Player Handle is NULL!!!  " << endl;
    return false;
  }
  /* playbin using buffer monitor timer */
  g_print("[%s] Buffering Timer Start!(Interval: 200ms) - update buffering info.\n", __FUNCTION__);
  m_bufferingTimerId = g_timeout_add(200, (GSourceFunc)updateBufferingInfo, this);
  return true;
}

void GenericPipeline::getStreamsInfoSpi(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  gint audioTrackCount = 0;
  gint videoTrackCount = 0;

  g_object_get(G_OBJECT(self->m_pipeHandle), "n-audio", &audioTrackCount, NULL);
  g_object_get(G_OBJECT(self->m_pipeHandle), "n-video", &videoTrackCount, NULL);
  g_print("n-audio = %d   \r\n", audioTrackCount);
  g_print("n-video = %d   \r\n", videoTrackCount);

  if (audioTrackCount > 0){
    self->m_bAudioAvailable = TRUE;
    self->S_audioChanged (self->m_pipeHandle, (gpointer)self); // sink attached lately.  //TODO: check audio-changed signal at gst core.
    /* notify total audio track list to upper layer. */
    //PLAYBIN_UpdateAudioTrackinfo(self);
  }
  if (videoTrackCount > 0){
    self->m_bVideoAvailable = TRUE;
    self->S_videoChanged (self->m_pipeHandle, (gpointer)self); // sink attached lately.  //TODO: check video-changed signal at gst core.
    /* notify total video angle list to upper layer. */
    //PLAYBIN_UpdateVideoAngleinfo(self);
  }
}

gboolean GenericPipeline::updateVideoInfoSpi(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipeline Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return FALSE;
  }

  if ((self->m_pipeHandle) && (self->m_gstPipelineState != self->StoppedState))
  {
    /* get video w/h info */
    GstElement *pVideosink = NULL;
    g_object_get (G_OBJECT (self->m_pipeHandle), "video-sink", &pVideosink, NULL);
    if (pVideosink != NULL)
    {
      GstPad *pPad = NULL;
      if ((pPad = gst_element_get_static_pad(pVideosink, "sink")) != NULL)
      {
        GstCaps *pCaps = NULL;
        
#if (GST_VERSION_MAJOR >= 1)
        if ((pCaps = gst_pad_get_current_caps(pPad)) != NULL)
#else
        if ((pCaps = gst_pad_get_negotiated_caps(pPad)) != NULL)
#endif
        {
          GstStructure *str;
          gint width, height;
          str = gst_caps_get_structure(pCaps, 0);
          if (gst_structure_get_int(str, "width", &width) && gst_structure_get_int(str, "height", &height))
          {
            g_print("[%s:%d] VIDEO: %dx%d\n", __FUNCTION__, __LINE__, width, height);
            self->m_source_width = width;
            self->m_source_height = height;
          }
          else
          {
            g_print("[%s:%d] VIDEO resolution not found\n", __FUNCTION__, __LINE__);
          }
          gst_caps_unref(pCaps);
        }
        else
        {
          g_print("[%s:%d] VIDEO caps not found\n", __FUNCTION__, __LINE__);
        }
        gst_object_unref(GST_OBJECT(pPad));
      }
      else
      {
        g_print("[%s:%d] VIDEO pads not found\n", __FUNCTION__, __LINE__);
      }
      gst_object_unref(GST_OBJECT(pVideosink));
    }
    else
    {
      g_print("[%s:%d] couldn't find VIDEOSINK \n", __FUNCTION__, __LINE__);
      return FALSE;
    }
  }
  else
  {
    g_print("[%s] failed (pPlayerHandle->player is null)\n", __FUNCTION__);
    return FALSE;
  }
  return TRUE;
}


void GenericPipeline::correctBufferedBytesSpi(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  
  GstQuery *pQuery = NULL;
  GstFormat queryFormat = GST_FORMAT_BYTES;
  gint64 bufferedSize = 0;
  guint64 vdecDataSize = 0;
  guint64 adecDataSize = 0;
  gint64 totalBufferedSize = 0;
  GstElement *pVdecsink = NULL;
  GstElement *pAdecsink = NULL;

  if (self->m_bLiveStreaming)
  {
    g_print("[%s:%d] underrun while playing live stream\n", __FUNCTION__, __LINE__);
    return;
  }
  g_print(" playbin CorrectBufferedBytes %p %p \n", self->m_pipeHandle, self);
  pQuery = gst_query_new_buffering(GST_FORMAT_BYTES);
  if (gst_element_query(self->m_pipeHandle, pQuery) == FALSE)
  {
    gst_query_unref(pQuery);
    g_print("[%s:%d] query failed: cannot correct buffered bytes\n", __FUNCTION__, __LINE__);
    return;
  }
  // query success.
  gst_query_parse_buffering_range(pQuery, &queryFormat, NULL, &bufferedSize, NULL);
  gst_query_unref(pQuery);
  if (bufferedSize == -1)
  {
    g_print("[%s:%d] Streaming is already finished!! (don't need to adj)\n", __FUNCTION__, __LINE__);
    return;
  }
  g_object_get (G_OBJECT (self->m_pipeHandle), "video-sink", &pVdecsink, NULL);
  if (pVdecsink != NULL)
  {
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(pVdecsink), "undecoded-size"))
    {
    g_object_get(G_OBJECT(pVdecsink), "undecoded-size", &vdecDataSize, NULL);
    }

    gst_object_unref(pVdecsink);
  }

  g_object_get (G_OBJECT (self->m_pipeHandle), "audio-sink", &pAdecsink, NULL);
  if (pAdecsink != NULL)
  {
    if (g_object_class_find_property(G_OBJECT_GET_CLASS(pAdecsink), "undecoded-size"))
    {
      g_object_get(G_OBJECT(pAdecsink), "undecoded-size", &adecDataSize, NULL);
    }
    gst_object_unref(pAdecsink);
  }

  totalBufferedSize = bufferedSize + vdecDataSize + adecDataSize;
  g_print("%"G_GINT64_FORMAT": %"G_GINT64_FORMAT"+%"G_GUINT64_FORMAT"+%"G_GUINT64_FORMAT"\n",
                                            totalBufferedSize, bufferedSize, vdecDataSize, adecDataSize);

  if (totalBufferedSize >= BUFF_ADJ_MAX)
  {
    g_print("new cor = %"G_GINT64_FORMAT"(spec over)\n", totalBufferedSize);
  }
  else
  {
    // AV encoding에 gap 이 있는 경우로 판단함.
    // 현재 buffering된 total size 만큼 차감하여 buffered data를 계산하기 위함.
    self->m_bufferedBytesCorrection = totalBufferedSize;
  }
}

void GenericPipeline::videoDecodeUnderrunCbSpi(GstElement *pObj, gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  g_print("[GST_SIGNAL] ------------- video underrun ------------\n");
  correctBufferedBytesSpi(self); // <-- TODO : Check...!!!!
}

void GenericPipeline::audioDecodeUnderrunCbSpi(GstElement *pObj, gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  g_print("[GST_SIGNAL] ------------- audio underrun ------------\n");
  correctBufferedBytesSpi(self); // <-- TODO : Check...!!!!
}

void GenericPipeline::registerUnderrunSignalHandlerSpi(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. LMF Player Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return;
  }

  if ((self->m_pipeHandle) &&
  (self->m_gstPipelineState != self->StoppedState))
  {
    GstElement *pVideosink = NULL;
    GstElement *pAudiosink = NULL;

    /* register video/audio-underrun handler */
    g_object_get (G_OBJECT (self->m_pipeHandle), "video-sink", &pVideosink, NULL);
    if (pVideosink != NULL)
    {
      g_signal_connect(pVideosink, "video-underrun", G_CALLBACK(videoDecodeUnderrunCbSpi), self); // <-- TODO : Check...!!!!
      gst_object_unref(pVideosink);
    }
    else
    {
      g_print("[%s:%d] couldn't find VIDEOSINK \n", __FUNCTION__, __LINE__);
    }

    g_object_get (G_OBJECT (self->m_pipeHandle), "audio-sink", &pAudiosink, NULL);
    if (pAudiosink != NULL)
    {
      g_signal_connect(pAudiosink, "audio-underrun", G_CALLBACK(audioDecodeUnderrunCbSpi), self); // <-- TODO : Check...!!!!
      gst_object_unref(pAudiosink);
    }
    else
    {
      g_print("[%s:%d] couldn't find AUDIOSINK \n", __FUNCTION__, __LINE__);
    }

  }
  else
  {
  g_print("[%s] Player is not ready\n", __FUNCTION__);
  }

  g_print("[%s:%d] finished!\n", __FUNCTION__, __LINE__);
}

void GenericPipeline::handleStateMsgPauseSpi_pre(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  
  getStreamsInfoSpi(self); /// genericpipeline only...  // <-- TODO : Check...!!!!
  std::cout << "[BUS][P2] Update Video Info & Register Unerrun Cb " << endl;
  updateVideoInfoSpi(self);  // <-- TODO : Check...!!!!
  registerUnderrunSignalHandlerSpi(self); // <-- TODO : Check...!!!!
}

void GenericPipeline::handlePlayEndSpi(gpointer data){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if(self->m_playbackRate >= 0){
  // forward direction EOS.

  }
  else{
    self->setPlaybackRate (self, 1.0);
  // backward direction EOS.
  }
  self->pause();

}
void GenericPipeline::handlePlayErrorSpi(gpointer data){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if(self->bUserStop == false)
    self->pause();
}
/*
gboolean GenericPipeline::checkTimeToDecodeSpi(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  
  return true;
}
*/

//multi track audio, multi angle video APIs
/**
* freeGList
* free the pList
*
* @param pList [in/out] list pointer
* @return void
* @see
*/
gboolean GenericPipeline::freeGList(GList *pList)
{
    GList *pTempList = NULL;
    for (pTempList=pList ; pTempList!=NULL ; pTempList=pTempList->next)
    {
        g_free (pTempList->data);
    }
    g_list_free (pList);
    pList = NULL;
    return true;
}

/**
* _PLAYBIN_GetLangListForType
* get the audio or text language list
*
* @param pPlayerHandle [in] player handle
* @param numOfTrack [in] total num of track
* @param pSignalStr [in] signal name to emit
* @return if succeeded - language list, else - NULL
* @see
*/
GList* GenericPipeline::getLangListForType(gpointer data, gint numOfTrack, const gchar *pSignalStr)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  GList   *pLangList      = NULL;
  gchar   *pLangCodeList  = NULL;
  gchar   *pCodecList     = NULL;
  gint    idx             = 0;
  gint    trackNum        = 1;

  if ((self->m_pipeHandle == NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == FALSE))
  {
    g_print("[%s:%d] Error. Player Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return NULL;
  }
  for (idx = 0 ; idx < numOfTrack ; idx++)
  {
    GstTagList *pTags = NULL;
    g_signal_emit_by_name(G_OBJECT(self->m_pipeHandle), pSignalStr, idx, &pTags);
    if (pTags)
    {
      gboolean res1=0; // fix for WBTest 0810010
      gboolean res2=0;
      res1 = gst_tag_list_get_string(pTags, GST_TAG_LANGUAGE_CODE, &pLangCodeList);
      res2 = gst_tag_list_get_string(pTags, GST_TAG_CODEC, &pCodecList);

      if (res1==1 && pLangCodeList)
      {
        pLangList = g_list_prepend(pLangList, pLangCodeList);
        g_free(pCodecList);
      }
      else if (res2 == 1 && pCodecList)
      {
        pLangList = g_list_prepend(pLangList, pCodecList);
      }
      else
      {
        pLangList = g_list_prepend(pLangList, g_strdup_printf(("A-Track #%d"), trackNum++));
      }
      gst_tag_list_free(pTags);
    }
    else
    {
      pLangList = g_list_prepend(pLangList, g_strdup_printf(("A-Track #%d"), trackNum++));
    }
  }
  return g_list_reverse(pLangList);
}

gboolean GenericPipeline::getAudioLanguagesListSpi(gpointer data, gchar **ppLangList, gint *pLangListSize, gint *pTotalLangNum){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle == NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }

  GList   *pList = NULL;
  gint    numOfTrack = 0;
  g_object_get(G_OBJECT(self->m_pipeHandle), "n-audio", &numOfTrack, NULL);
  if (numOfTrack != 0)
  {
    pList = self->getLangListForType(self, numOfTrack, (const char *)"get-audio-tags");
  }
#if 0 // fixed for MHP..
  /* When we have only one language, we don't need to show
  * any languages, we default to the only track */
  if (pList !=NULL && g_list_length (pList) == 1)
  {
    MEDIAPIPE_DBG_PRINT("only one track. found !!! \r\n");
    g_free (pList->data);
    g_list_free (pList);
    pList = NULL;
  }
#endif
  *pTotalLangNum = numOfTrack;
  
  if(pList == NULL)
  {
    g_print("not multi audio track playback case. \r\n");
    return false;
  }
  else
  {
    g_print("[getAudioLanguagesListSpi] multi audio track playback case. \r\n");
    //*pTotalLangNum = g_list_length(pList);
    //if (*pTotalLangNum <= 1)
    if(g_list_length(pList)<1 || *pTotalLangNum<1)
    {
      g_print("not multi audio track playback case. \r\n");
      //*pTotalLangNum = 1;
      return false;
    }
    if (*pTotalLangNum > MAX_LANGUAGE_LIST_NUM)
      *pTotalLangNum = MAX_LANGUAGE_LIST_NUM;
    *pLangListSize = MAX_LANGUAGE_STR_LENGTH * (*pTotalLangNum);
    g_print("memory allocation for string array. \r\n");
    *ppLangList = (char *)calloc(1, (size_t)(*pLangListSize)); // alloc send array
    if ((*ppLangList) == NULL)
    {
      g_print("[media_API] error memory calloc fail.!!! size=%d \r\n", *pTotalLangNum * MAX_LANGUAGE_STR_LENGTH);
      return false;
    }
    g_print("[getAudioLanguagesListSpi] copy language field.\r\n");
    int idx=0;
    for (idx=0 ; idx<(*pTotalLangNum) ; idx++) // copy to array
    {
      g_print("[getAudioLanguagesListSpi][%d of %d]%s \r\n", idx, *pTotalLangNum, (char *)(pList->data));
      strcpy((*ppLangList) + (MAX_LANGUAGE_STR_LENGTH*idx), (char *)(pList->data));
      if (pList->next != NULL)
      pList = pList->next;
    }
    pList = g_list_first(pList);
    self->freeGList(pList);
  }
  return true;
}
gboolean GenericPipeline::setAudioLanguageSpi(gpointer data, char *pAudioLang){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  gboolean retVal = false;
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle== NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  gint TotalLangNum =1;
  gchar *langStr = NULL;
  gint StrLength = 0;
  int lang_index = -1;
  if(self->getAudioLanguagesListSpi(self, &langStr, &StrLength, &TotalLangNum)==false)
    return false;
  g_print("Total Language Number = %d, Received String Length = %d \r\n",TotalLangNum,  StrLength);
  if(TotalLangNum>0 && StrLength>0)
  {
    // multi track playback
    int offset = 0;
    if(TotalLangNum>MAX_LANGUAGE_LIST_NUM)
    TotalLangNum = MAX_LANGUAGE_LIST_NUM;
    for(int i=0; i<TotalLangNum; i++)
    {
      g_print("%s>[num:%d][offset:%d][str_len=%ld][%s] \r\n ",__FUNCTION__, i, offset, strlen(langStr+offset), (langStr+offset));
      if(strcmp((langStr+offset),pAudioLang)==0)
      {
        lang_index = i;
        g_print("%s we found language. %s at i=%d \r\n",__FUNCTION__,pAudioLang,i );
        break;
      }
      offset = offset + MAX_LANGUAGE_STR_LENGTH;
    }
    if(lang_index != -1){
      retVal = self->setAudioTrackSpi(self, lang_index);
    }
  }
  else{
    // no track
  }
  if(langStr!=NULL)  {
    g_print(" free memory. langStr. \r\n");
    free(langStr);
  }
  langStr = NULL;
  return true;
}
gboolean GenericPipeline::setAudioTrackSpi(gpointer data, gint AudioTrackNum){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle== NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if (AudioTrackNum == -1)
    AudioTrackNum = 0;
  else if (AudioTrackNum == -2)
    AudioTrackNum = -1;
  g_print("setting language to %d \r\n", AudioTrackNum);
  g_object_set(self->m_pipeHandle, "current-audio", AudioTrackNum, NULL);
  g_object_get(self->m_pipeHandle, "current-audio", &AudioTrackNum, NULL);
  g_print("current-audio now: %d \r\n", AudioTrackNum);
  return true;
}
gboolean GenericPipeline::getCurAudioLanguageSpi(gpointer data, char **ppAudioLang){
  return true;
}
gboolean GenericPipeline::getCurAudioTrackSpi(gpointer data, gint *pCurAudioTrackNum){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle == NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  *pCurAudioTrackNum = -1;
  g_print("[%s:%d] g_object_get  current-audio \n", __FUNCTION__, __LINE__);
  g_object_get(G_OBJECT(self->m_pipeHandle), "current-audio", pCurAudioTrackNum, NULL);
  g_print("[%s:%d] g_object_get  current-audio = %d \n", __FUNCTION__, __LINE__, *pCurAudioTrackNum);
  return true;
}
gboolean GenericPipeline::getTotalVideoAngleSpi(gpointer data,  gint *pTotalVideoAngleNum){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle== NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  guint n_video = 0;
  g_object_get (G_OBJECT(self->m_pipeHandle), "n-video", &n_video, NULL);
  g_print("[%s] TotalVideoAngle = %d \n", __FUNCTION__, n_video);
  *pTotalVideoAngleNum = n_video;
  return true;
}
gboolean GenericPipeline::setVideoAngleSpi(gpointer data, gint VideoAngleNum){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle== NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  guint n_video = 0;
  g_print("[%s] set VideoAngleNum = %d \n", __FUNCTION__, VideoAngleNum);

  g_object_get (G_OBJECT(self->m_pipeHandle), "n-video", &n_video, NULL);
  if (n_video <= 1 || VideoAngleNum+1 > n_video)
  {
    g_print ("Not setting video stream, we have %d video streams \r\n", n_video);
    return false;
  }
  g_object_set (G_OBJECT (self->m_pipeHandle), "current-video", VideoAngleNum, NULL);
  return true;
}
gboolean GenericPipeline::getCurrentVideoAngleSpi(gpointer data, gint *pCurrentVideoAngleNum){
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if (self == NULL)
  {
    g_print("[%s:%d] Error. pipe container Handle is NULL!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  if ((self->m_pipeHandle== NULL) || (GST_IS_ELEMENT(self->m_pipeHandle) == false) )
  {
    g_print("[%s:%d] Error. Gstreamer Player Handle is NULL or not playbin!!!  \n", __FUNCTION__, __LINE__);
    return false;
  }
  guint current_video = 0;
  g_object_get (G_OBJECT(self->m_pipeHandle), "current-video", &current_video, NULL);
  g_print("[%s] CurrentVideoAngleNum = %d \n", __FUNCTION__, current_video);
  *pCurrentVideoAngleNum = current_video;
  return true;
}
// end 

gint GenericPipeline::greatestCommonDivisor(int a, int b)
{
    while (b) {
        int temp = a;
        a = b;
        b = temp % b;
    }
    return ABS(a);
}

gboolean GenericPipeline::videoChangeTimeoutCallback(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if(self == NULL || self->m_pipeHandle == NULL)
    return false;
  
  GstElement* element = self->m_pipeHandle;
  
  gint videoTracks = 0;
  GstElement* videoSink = NULL;
  g_print("element = %p \r\n", element);
  g_object_get(element, "n-video", &videoTracks, "video-sink", &videoSink, NULL);
  if(videoSink == NULL)
  {
    g_print("videoSink = %p \r\n", videoSink);
    return false;
  }
  GstPad* pad = gst_element_get_static_pad(videoSink, "sink");
#if (GST_VERSION_MAJOR >= 1)
  GstCaps* caps = gst_pad_get_current_caps(pad);
#else
  GstCaps* caps = GST_PAD_CAPS(pad);
#endif
  guint64 width = 0, height = 0;
  gint originalWidth, originalHeight;
  GstVideoFormat format;
  int pixelAspectRatioNumerator, pixelAspectRatioDenominator;
#if (GST_VERSION_MAJOR >= 1)
  GstVideoInfo videoinfo;
#if 0 //TODO : resolve linking issue. ../src/libpf.so: undefined reference to `gst_video_info_from_caps'
  if (!GST_IS_CAPS(caps) || !gst_caps_is_fixed(caps)
  || !gst_video_info_from_caps(&videoinfo ,caps))
    goto clean_res;
#endif  
  originalWidth = videoinfo.width;
  originalHeight = videoinfo.height;
  pixelAspectRatioNumerator = videoinfo.par_n;
  pixelAspectRatioDenominator = videoinfo.par_d;
#else
  if (!GST_IS_CAPS(caps) || !gst_caps_is_fixed(caps)
  || !gst_video_format_parse_caps(caps, &format, &originalWidth, &originalHeight)
  || !gst_video_parse_caps_pixel_aspect_ratio(caps, &pixelAspectRatioNumerator,
                        &pixelAspectRatioDenominator))
    goto clean_res;
#endif
  // Calculate DAR based on PAR and video size.
  int displayWidth;
  displayWidth = originalWidth * pixelAspectRatioNumerator;
  int displayHeight;
  displayHeight = originalHeight * pixelAspectRatioDenominator;

  // Divide display width and height by their GCD to avoid possible overflows.
  int displayAspectRatioGCD ;
  displayAspectRatioGCD = self->greatestCommonDivisor(displayWidth, displayHeight);
  displayWidth /= displayAspectRatioGCD;
  displayHeight /= displayAspectRatioGCD;

  // Apply DAR to original video size. This is the same behavior as in xvimagesink's setcaps function.
  if (!(originalHeight % displayHeight)) {
    g_print("Keeping video original height");
    width = gst_util_uint64_scale_int(originalHeight, displayWidth, displayHeight);
    height = (guint64) originalHeight;
  } else if (!(originalWidth % displayWidth)) {
    g_print("Keeping video original width");
    height = gst_util_uint64_scale_int(originalWidth, displayHeight, displayWidth);
    width = (guint64) originalWidth;
  } else {
    g_print("Approximating while keeping original video height");
    width = gst_util_uint64_scale_int(originalHeight, displayWidth, displayHeight);
    height = (guint64) originalHeight;
  }
  g_print("Natural size: %" G_GUINT64_FORMAT "x%" G_GUINT64_FORMAT, width, height);
clean_res:
  gst_caps_unref(caps);
  gst_object_unref(pad);
  gst_object_unref(videoSink);

  self->notifyVideoTrackUpdate(self, videoTracks > 0, width, height);

  return FALSE;
}

void GenericPipeline::S_videoChanged (GstElement* element, gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if(self == NULL || self->m_pipeHandle == NULL)
    return;
  g_warning("S_videoChanged: invoked \n");
  g_print("[%s:%d] S_videoChanged: invoked  \n", __FUNCTION__, __LINE__);
  // This function is called in the GStreamer streaming thread, so schedule
  // a function to be executed in the main thread to comply with luna-service
  // requirements.
  g_timeout_add(0, (GSourceFunc) videoChangeTimeoutCallback, (gpointer)self); // <--- TODO! check... 
}

gboolean GenericPipeline::audioChangeTimeoutCallback(gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if(self == NULL || self->m_pipeHandle == NULL)
    return false;
  gint audioTracks = 0;
  g_object_get(self->m_pipeHandle, "n-audio", &audioTracks, NULL);
  self->notifyAudioTrackUpdate(self, audioTracks > 0);
  return FALSE;
}

void GenericPipeline::S_audioChanged (GstElement* element, gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  if(self == NULL || self->m_pipeHandle == NULL)
    return;
  g_warning("S_audioChanged: invoked \n");
  g_print("[%s:%d] S_audioChanged: invoked  \n", __FUNCTION__, __LINE__);

  // This function is called in the GStreamer streaming thread, so schedule
  // a function to be executed in the main thread to comply with luna-service
  // requirements.
  g_timeout_add(0, (GSourceFunc) audioChangeTimeoutCallback, self); // <--- TODO! check... 
}

