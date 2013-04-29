/*
 * pipeline.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */


#include <pipeline/pipeline.hpp>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include <pthread.h>
#include <gio/gio.h>
#include <glibmm-utils/glibmm-utils.h>

using namespace mediapipeline;


Pipeline::Pipeline ()
{

  m_gstPipelineState = StoppedState;
  m_pendingState = StoppedState;
  m_busHandler = NULL;
  m_playbinVersion = -1;
  m_audioAvailable = false;
  m_videoAvailable = false;
  m_playbackRate = 1.0;
  m_volume = 100;
  m_muted = false;
  m_seekable = false;
  m_bSeekableDuration = false;
  m_bSeekableIndex = false;
  m_duration = -1;
  m_currentPosition = 0;
  m_durationQueryCount = 0;
  m_blockByVideoSink = 0;
  m_uri.clear ();
  m_subtitle_uri.clear ();

  /* create buffer controller class */
  BufferController::bsp_t buffercontrol = getBufferController ();
  buffercontrol.reset (new BufferController ());
  setBufferController (buffercontrol);  // save to abstractplayer.

}

Pipeline::~Pipeline ()
{

}

//------------------------------------------start basic controls //
gboolean Pipeline::init ()
{

  if (this->initSpi_pre () == false)    //custom pipeline control (pre) < create gstreamer pipeline here. > m_pipeHandle
    return false;
/* common gstreamer control start */

  std::cout << " >> init " << endl;

  connectGstBusCallback ();

  std::cout << "set pipe to NULL";

  gst_element_set_state (m_pipeHandle, GST_STATE_NULL);

  cout << "set buffer size";
  // default buffer-size (see gstqueue2)
  g_object_set (G_OBJECT (m_pipeHandle), "buffer-size",
      (gint) (MEDIAPIPE_BUFFER_SIZE), NULL);
  g_object_set (G_OBJECT (m_pipeHandle), "buffer-duration", (gint64) (0), NULL);

  // connect volume notify.
  double
      volume = 1.0;
  g_object_get (G_OBJECT (m_pipeHandle), "volume", &volume, NULL);
  m_volume = int (volume * 100);

  g_signal_connect (G_OBJECT (m_pipeHandle), "notify::volume",
      G_CALLBACK (handleVolumeChange), this);
  if (m_playbinVersion == 2)
    g_signal_connect (G_OBJECT (m_pipeHandle), "notify::mute",
        G_CALLBACK (handleMutedChange), this);

  cout << "pipeline] create finish " << endl;

/* common gstreamer control end */
  if (this->initSpi_post () == false)   //custom pipeline control (post)
    return false;

  return true;
}

gboolean Pipeline::play (int rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  if (m_pipeHandle) {
    m_pendingState = PlayingState;      // pending state //
    if (gst_element_set_state (m_pipeHandle,
            GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
      cout << "Pipeline] Unable to play : " << m_uri << endl;
      m_pendingState = m_gstPipelineState = StoppedState;

      stateChanged (m_gstPipelineState);
    } else
      return true;
  }
  return false;

}

gboolean Pipeline::unload ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  stop ();
  return true;
}

gboolean Pipeline::pause ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  if (m_pipeHandle) {
    m_pendingState = PausedState;       //pending state.
    if (m_blockByVideoSink != 0)        //blocked by videosink.
      return true;
    if (gst_element_set_state (m_pipeHandle,
            GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
      std::cout << "genericpipeline] Unable to pause : " << m_uri << endl;
      m_pendingState = m_gstPipelineState = StoppedState;

      stateChanged (m_gstPipelineState);        // notify state change.
    } else {
      return true;
    }
  }
  return false;

}

gboolean Pipeline::stop ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  if (m_pipeHandle) {
    //stopping (renderer if existed.) here.

    gst_element_set_state (m_pipeHandle, GST_STATE_NULL);

    m_currentPosition = 0;      //update position quickly.
    State
        oldState = m_gstPipelineState;
    m_pendingState = m_gstPipelineState = StoppedState;

    //finish something like video HW etc..

    //gstreamer will not emit bus messages any more
    setSeekable (false);
    if (oldState != m_gstPipelineState)
      stateChanged (m_gstPipelineState);        //notify state change.
  }
  return true;
}

gboolean Pipeline::seek (gint64 ms)
{
  //seek locks when the video output sink is changing and pad is blocked
  if (m_pipeHandle && !m_blockByVideoSink && m_gstPipelineState != StoppedState) {
    ms = MAX (ms, gint64 (0));
    gint64
        position = ms * 1000000;
    bool
        isSeeking = gst_element_seek (m_pipeHandle,
        m_playbackRate,
        GST_FORMAT_TIME,
        GstSeekFlags (GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
        GST_SEEK_TYPE_SET,
        position,
        GST_SEEK_TYPE_NONE,
        0);
    if (isSeeking)
      m_currentPosition = ms;

    return isSeeking;
  }

  return false;
}

gboolean Pipeline::isSeekable ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  return isSeekable (this);
}

gboolean Pipeline::isSeekable (gpointer data)
{
  Pipeline *
      self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");

  if (self == NULL) {
    std::cout << "isseekable) Error. LMF Player Handle is NULL!!!" << endl;
    return false;
  }
  gboolean
      bCheckSeekable = false;

  if (self->m_isDLNA) {
    //TODO if(_LMF_PLYR_CTRL_isNotSeekableDlnaMedia(pPlayerHandle)==true) // DLNA 에서 not seekable 한 media filtering.
    //TODO  return false;

    std::cout << "pPlayerHandle->dlna_opval:" << self->m_dlna_opval << endl;
    // 1. check standard opvalue seekable case.
    if (self->m_dlna_opval & 0x11) {
      std::
          cout << "Is (DLNA - standard) Seekable = " << (((self->m_dlna_opval &
                  0x11) == true) ? "O" : "X")
          << "(opval byte based seek:" << ((self->m_dlna_opval & 0x01) ? "O" :
          "X")
          << "opval time based seek:" << ((self->m_dlna_opval & 0x10) ? "O" :
          "X")
          << "dur:" << ((self->m_bSeekableDuration == true) ? "O" : "X");

      bCheckSeekable = true;
    }
#if 0                           //TODO...
    // 2. check nonstandard DLNA seekable case. only for JPN + buffalo NAS.
    else if (MF_MAIN_GetLocale () == LMF_LOCALE_JAPAN
        && pPlayerHandle->bSeekableDuration) {
      LMF_DBG_PRINT
          ("Is (DLNA JPN - nonstandard) Seekable = %s(opval byte based seek:%s,opval time based seek:%s,dur:%s)\n",
          (((pPlayerHandle->dlna_opval & 0x11
                      || pPlayerHandle->bSeekableDuration) ==
                  true) ? "O" : "X"),
          ((pPlayerHandle->dlna_opval & 0x01) ? "O" : "X"),
          ((pPlayerHandle->dlna_opval & 0x10) ? "O" : "X"),
          ((pPlayerHandle->bSeekableDuration == true) ? "O" : "X"));

      bCheckSeekable = true;
    }
#endif
  } else                        // non DLNA case
  {
    if (self->m_bSeekableDuration) {
      std::cout << "Is ( ) Seekable ="
          << ((self->m_bSeekableDuration == true) ? "O" : "X")
          << "(dur:"
          << ((self->m_bSeekableDuration == true) ? "O" : "X") << ")" << endl;

      bCheckSeekable = true;
    }
  }

  std::cout << "Is Seekable = "
      << (((self->m_seekable && bCheckSeekable
              && self->m_bSeekableIndex) == true) ? "O" : "X")
      << "(state:" << ((self->m_seekable == true) ? "O" : "X")
      << ",dur+opval:" << ((bCheckSeekable == true) ? "O" : "X")
      << ",index:" << ((self->m_bSeekableIndex == true) ? "O" : "X")
      << ")" << endl;
  return (self->m_seekable && bCheckSeekable && self->m_bSeekableIndex);


}

gboolean Pipeline::isReadyToPlay ()
{
  return this->isReadyToPlaySpi ();
}

Pipeline::State Pipeline::getPendingPipelineState ()
{
  std::cout << "pending pipeline state = " << this->m_pendingState << endl;
  return this->m_pendingState;
}


//------------------------------------------end basic gstreamer controls //


//------------------------------------------ start get/set basic play information //
     gint64 Pipeline::duration () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
       return
           m_duration;
     }

gint64
     Pipeline::position () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
       GstFormat format = GST_FORMAT_TIME;
       gint64 position = 0;
#if (GST_VERSION_MAJOR >= 1)
       if (m_pipeHandle
           && gst_element_query_position (m_pipeHandle, format, &position))
#else
       if (m_pipeHandle
           && gst_element_query_position (m_pipeHandle, &format, &position))
#endif
           m_currentPosition = position / 1000000;

         return m_currentPosition;
     }

gint
     Pipeline::volume () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");

     }

gboolean
     Pipeline::isMuted () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");

     }

gboolean
     Pipeline::isAudioAvailable () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");

     }

gboolean
     Pipeline::isVideoAvailable () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");

     }

//------------------------------------------ end get/set basic play information //





     void Pipeline::stateChanged (Pipeline::State newState)
{



}

void
Pipeline::seekableStateChanged (bool seekable)
{



}

void
Pipeline::playbackRateChanged (gfloat rate)
{


}


void
Pipeline::handleVolumeChange (GObject * pObject, GParamSpec * pParam,
    gpointer u_data)
{


}

void
Pipeline::handleMutedChange (GObject * pObject, GParamSpec * pParam,
    gpointer u_data)
{


}

void
Pipeline::setBufferController (BufferController::bsp_t busController)
{
  this->_bufferControl = busController;
}

BufferController::bsp_t Pipeline::getBufferController ()
{
  return this->_bufferControl;
}

void
Pipeline::pipelineEventNotify (gpointer data, MEDIA_CB_MSG_T msg)
{



}


void
Pipeline::checkSupported (gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  if (self->m_pipeHandle == NULL)       //(GST_IS_ELEMENT(pPlayerHandle->player) == false))
  {
    std::cout << "[BUS] Error. LMF Player Handle is NULL!!! ";
    return;
  }
#if 0                           //TODO::
  if (((pPlayerHandle->sourceInfo).codec >> 8) ==
      (MEDIA_VIDEO_NOT_SUPPORTED >> 8)) {
    LMF_DBG_PRINT ("[BUS][%s:%d] Codec NOT Supported\n", __FUNCTION__,
        __LINE__);
    pPlayerHandle->bSendNotSupportedVideoMessageAtPlayState = true;
    m_bSendNotSupportedAudioMessageAtPlayState = true;
    pPlayerHandle->bPendingNotSupportedVideoMessage = false;
    m_bPendingNotSupportedAudioMessage = false;

    LMF_PLYR_ProcessNotSupported (pPlayerHandle->ch);
  }
#endif
}

void
Pipeline::collectTags (const GstTagList * tag_list, const gchar * tag,
    gpointer user_data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(user_data);

  //MEDIA_SOURCE_INFO_T *pSourceInfo = NULL;
  gchar *pStr = NULL;
  gint32 idx = 0;
  gint32 tagSize = 0;

  if (self->m_pipeHandle == NULL)       //(GST_IS_ELEMENT(pPlayerHandle->player) == false))
  {
    std::cout << "[BUS] Error. LMF Player Handle is NULL!!! ";
    return;
  }
  //pSourceInfo = &(pPlayerHandle->sourceInfo);

  /* print tags immediatly when receiving GST_MESSAGE_TAG  */
  tagSize = gst_tag_list_get_tag_size (tag_list, tag);

  for (idx = 0; idx < tagSize; idx++) {
    gchar *pTagStr = NULL;

    if (gst_tag_get_type (tag) == G_TYPE_STRING) {
      if (!gst_tag_list_get_string_index (tag_list, tag, idx, &pTagStr)) {
        //g_assert_not_reached();
        std::cout <<
            "Error!!! ================================================= !!!!  ";
        std::cout <<
            "Error!!! assert gst_tag_list_get_string_index not reached. !!!!  ";
        std::cout <<
            "Error!!! ================================================= !!!!  ";
      }
    } else if (gst_tag_get_type (tag) == GST_TYPE_BUFFER) {
      GstBuffer *pGstBuf = NULL;
#if (GST_VERSION_MAJOR >= 1)

#else
      pGstBuf =
          gst_value_get_buffer (gst_tag_list_get_value_index (tag_list, tag,
              idx));
      if (pGstBuf) {
        gchar *pCapsStr = NULL;

        pCapsStr = ((GST_BUFFER_CAPS (pGstBuf))
            ? (gst_caps_to_string (GST_BUFFER_CAPS (pGstBuf)))
            : (g_strdup ("unknown")));
        pTagStr =
            g_strdup_printf ("buffer of %u bytes, type: %s",
            GST_BUFFER_SIZE (pGstBuf), pCapsStr);

        g_free (pCapsStr);
      } else {
        pTagStr = g_strdup ("NULL buffer");
      }
#endif
    } else {
      pTagStr =
          g_strdup_value_contents (gst_tag_list_get_value_index (tag_list, tag,
              idx));
    }

    if (idx == 0) {
      std::cout << "[BUS]" << gst_tag_get_nick (tag) << ":" << pTagStr;
    } else {
      std::cout << "[BUS]                 :" << pTagStr;
    }

    g_free (pTagStr);
  }
#if 0                           // TODO:CHECK..
  /* save the tag values into pSourceInfo */
  if (strcmp (tag, GST_TAG_TITLE) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      strncpy (m_source_title, pStr, LMF_TITLE_MAX_LEN - 1);
      m_source_title[LMF_TITLE_MAX_LEN - 1] = '\0';
      g_free (pStr);
    }
  } else if (strcmp (tag, GST_TAG_ARTIST) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      strncpy (m_source_artist, pStr, LMF_TITLE_MAX_LEN - 1);
      m_source_artist[LMF_TITLE_MAX_LEN - 1] = '\0';
      g_free (pStr);
    }
  } else if (strcmp (tag, GST_TAG_COPYRIGHT) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      strncpy (m_source_copyright, pStr, LMF_TITLE_MAX_LEN - 1);
      m_source_copyright[LMF_TITLE_MAX_LEN - 1] = '\0';
      g_free (pStr);
    }
  } else if (strcmp (tag, GST_TAG_ALBUM) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      strncpy (m_source_album, pStr, LMF_TITLE_MAX_LEN - 1);
      m_source_album[LMF_TITLE_MAX_LEN - 1] = '\0';
      g_free (pStr);
    }
  } else if (strcmp (tag, GST_TAG_GENRE) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      strncpy (m_source_genre, pStr, LMF_TITLE_MAX_LEN - 1);
      m_source_genre[LMF_TITLE_MAX_LEN - 1] = '\0';
      g_free (pStr);
    }
  } else if (strcmp (tag, GST_TAG_DATE) == 0) {
    GDate *pDate = NULL;
#if 0                           // TODO:: check...
    if (gst_tag_list_get_date (tag_list, GST_TAG_DATE, &pDate)) {
      m_source_date.year = g_date_get_year (pDate);
      m_source_date.month = g_date_get_month (pDate);
      m_source_date.day = g_date_get_day (pDate);
      g_date_free (pDate);
    }

    m_source_date.hour = 0;
    m_source_date.minute = 0;
    m_source_date.second = 0;
#endif
  } else
#endif
  if (strcmp (tag, GST_TAG_CONTAINER_FORMAT) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      guint8 idx = 0;
      gboolean bConvertInfo = false;
#if 0                           //TODO...
      while (_containerTable[idx].pStr != NULL) {
        if (strstr (pStr, _containerTable[idx].pStr) != NULL) {
          bConvertInfo = true;
          MEDIA_FORMAT_T prevFormat = m_source_format;
          m_source_format = _containerTable[idx].format;
          if (prevFormat == MEDIA_FORMAT_RM
              && m_source_format == MEDIA_FORMAT_RA)
            m_source_format = prevFormat;
          break;
        }
        idx++;
      }
#endif
      if (bConvertInfo == false)
        std::cout << "[BUS] Unknown Container Format:" << "tag:" << tag <<
            "format:" << pStr;

      g_free (pStr);
    }
  } else if (strcmp (tag, GST_TAG_CODEC) == 0) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      std::cout << "************** [BUS]" << tag << "(" <<
          gst_tag_get_nick (tag)
          << ")" << pStr;
      g_free (pStr);
    }
  } else if ((strcmp (tag, GST_TAG_AUDIO_CODEC) == 0)
      && ((!(self->m_source_codec & MEDIA_AUDIO_MASK))
          || self->m_bPlaybackStarted == true)) {
    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      guint8 idx = 0;
      gboolean bConvertInfo = false;

      std::cout << "************** [BUS]" << tag << "(" <<
          gst_tag_get_nick (tag)
          << ")" << pStr;
      LMF_TO_LOWER (pStr);
#if 0                           //TODO...
      while (_aCodecTable[idx].pStr != NULL) {
        if (strstr (pStr, _aCodecTable[idx].pStr) != NULL) {
          bConvertInfo = true;
          m_source_codec |= _aCodecTable[idx].codec;
          break;
        }
        idx++;
      }

      if (bConvertInfo == false) {
        LMF_DBG_PRINT ("[BUS][%s][%s] Unknown Audio Codec(%s)\n", __FUNCTION__,
            tag, pStr);
        m_source_codec |= MEDIA_AUDIO_NOT_SUPPORTED;
        // AUDIO_CODEC Tag가 올라오면 UI에 noti 보내주자.
        // 1. DLNA등에서 audio TAG 만 늦게 올라오는 case. 나옴. (보통은 paused state에 올라옴)
        // play state 이후에 TAG 올라오는 case
        if (m_gstPipelineState == PlayingState
            && m_bSendNotSupportedAudioMessageAtPlayState == false) {
          LMF_DBG_PRINT
              ("[BUS][%s][%s] after play state : sending Unknown Audio Codec message.\n",
              __FUNCTION__, tag);
          m_bPendingNotSupportedAudioMessage = false;
          m_bSendNotSupportedAudioMessageAtPlayState = true;
          self->pipelineEventNotify (self,
              MEDIA_CB_MSG_NOT_SUPPORTED_AUDIO_CODEC);
        } else {
          // play state 전이면 pending 하고 play state change 시에 보내자.
          LMF_DBG_PRINT
              ("[BUS][%s][%s] before play state : pending Unknown Audio Codec message.\n",
              __FUNCTION__, tag);
          m_bPendingNotSupportedAudioMessage = true;
        }
      } else {
        // AUDIO_CODEC Tag가 올라오면 UI에 noti 보내주자.
        // 1. DLNA등에서 audio TAG 만 늦게 올라오는 case. 대응.
        // play state에서 뒤늦게 audio TAG가 올라오면 UI에 noti.
        if (m_gstPipelineState == PlayingState) {
          LMF_DBG_PRINT ("[BUS][%s] video TAG updated! noti to UI.\n",
              __FUNCTION__);
          self->pipelineEventNotify (self, MEDIA_CB_MSG_SOURCE_INFO_UPDATED);
        }
      }
#endif
      g_free (pStr);
    }
  } else if ((strcmp (tag, GST_TAG_VIDEO_CODEC) == 0)
      && (!(self->m_source_codec & MEDIA_VIDEO_MASK))) {
    gchar *pVideoCodecInfo = NULL;
    gchar *pTempInfo = NULL;
    gchar *pVerInfo = NULL;

    if (gst_tag_list_get_string (tag_list, tag, &pStr)) {
      //LMF_DBG_PRINT("************** [BUS]%s (%s): %s \n", tag, gst_tag_get_nick(tag), pStr);
      std::cout << "************** [BUS]" << tag << "(" <<
          gst_tag_get_nick (tag)
          << ")" << pStr;

      if (gst_tag_list_get_string (tag_list, GST_TAG_VIDEO_CODEC,
              &pVideoCodecInfo)) {
        guint8 idx = 0;
        gboolean bConvertInfo = false;

        LMF_TO_LOWER (pVideoCodecInfo);
#if 0                           //TODO...
        while (_vCodecTable[idx].pStr != NULL) {
          if ((pTempInfo =
                  strstr (pVideoCodecInfo, _vCodecTable[idx].pStr)) != NULL) {
            bConvertInfo = true;
            if (_vCodecTable[idx].codec == MEDIA_VIDEO_MPEG4) {
              if ((strstr (pTempInfo, ".2") != NULL) || (strstr (pTempInfo, "4.1") != NULL) ||  // M13 에서 MPEG4 4.1 지원 불가.
                  (strstr (pTempInfo, "1.1") != NULL) ||        // M13 의 경우 MPEG4 1.1 지원 불가.
                  (((pVerInfo = strstr (pTempInfo, "version")) != NULL)
                      && (strstr (pVerInfo, "2") != NULL))) {
                LMF_DBG_PRINT ("\n [BUS]Not support video codec : [%s]\n\n",
                    pVideoCodecInfo);
                LMF_DBG_PRINT ("\n [BUS]sub value %s\n\n", pTempInfo);

                if (pVerInfo != NULL)
                  LMF_DBG_PRINT ("\n [BUS]ver value %s\n\n", pVerInfo);
                else
                  LMF_DBG_PRINT ("\n [BUS]ver value is NULL\n\n");

                m_source_codec |= MEDIA_VIDEO_NOT_SUPPORTED;
                bConvertInfo = false;
              } else {
                m_source_codec |= MEDIA_VIDEO_MPEG4;
              }
            } else if (_vCodecTable[idx].codec == MEDIA_VIDEO_WMV) {
              if ((strstr (pTempInfo, "7") != NULL) ||
                  (strstr (pTempInfo, "8") != NULL)) {
                m_source_codec |= MEDIA_VIDEO_NOT_SUPPORTED;
                bConvertInfo = false;
              } else
                m_source_codec |= MEDIA_VIDEO_WMV;
            } else {
              m_source_codec |= _vCodecTable[idx].codec;
            }

            break;
          }

          idx++;
        }
#endif
        if (bConvertInfo == false) {
          std::cout << "[BUS] Unknown Video Codec" << "tag:" << tag << "codec:"
              << pStr;
          // TODO m_source_codec |= MEDIA_VIDEO_NOT_SUPPORTED;
          // AUDIO_CODEC Tag가 올라오면 UI에 noti 보내주자.
          // 1. avi 등에서 video TAG 만 늦게 올라오는 case. 나옴. (보통은 paused state에 올라옴)
          // play state 이후에 TAG 올라오는 case
          if (self->m_gstPipelineState == PlayingState
              && self->m_bSendNotSupportedVideoMessageAtPlayState == false) {
            std::cout <<
                "[BUS]after play state : sending Unknown Video Codec message:"
                << tag;
            self->m_bPendingNotSupportedVideoMessage = false;
            //pPlayerHandle->bSendNotSupportedVideoMessageAtPlayState = true;

            //TODO _LMF_PLYR_BUS_ProcessNotSupportVideoCodec(pPlayerHandle);

          } else {
            // play state 전이면 pending 하고 play state change 시에 보내자.
            std::cout <<
                "[BUS]before play state : pending Unknown Video Codec message:"
                << tag;
            self->m_bPendingNotSupportedVideoMessage = true;
          }
        } else {
          // VIDEO_CODEC Tag가 올라오면 UI에 noti 보내주자.
          // 1. DLNA등에서 video TAG 만 늦게 올라오는 case. 대응.
          // play state에서 뒤늦게 video TAG가 올라오면 UI에 noti.
          if (self->m_gstPipelineState == PlayingState) {
            std::cout << "[BUS] video TAG updated! noti to UI.";
            self->pipelineEventNotify (self, MEDIA_CB_MSG_SOURCE_INFO_UPDATED);
          }
        }

        if (pVideoCodecInfo != NULL)
          g_free (pVideoCodecInfo);
      }

      g_free (pStr);
    }
  } else if ((strcmp (tag, GST_TAG_BITRATE) == 0)
      || (strcmp (tag, GST_TAG_MAXIMUM_BITRATE) == 0)) {
    guint bitrate = 0;

    if (gst_tag_list_get_uint (tag_list, tag, &bitrate)) {
      if (self->m_MaxBitrate == 0) {
        self->m_MaxBitrate = bitrate;
        std::cout << "[BUS]tag bitrate:" << bitrate << "bits/s";
      } else if (self->m_MaxBitrate < bitrate) {
        self->m_MaxBitrate = bitrate;
        std::cout << "[BUS]tag bitrate:" << bitrate << " bits/s updated ";
      } else {
        std::cout <<
            "[BUS]tag bitrate: %u bits/s ignored (MaxBitrate = %u bits/s)\n",
            bitrate, self->m_MaxBitrate;
      }
    }
  } else {
    std::cout << "[BUS]" << tag << gst_tag_get_nick (tag);
  }

}

void
Pipeline::handleBusTag (gpointer data, GstMessage * pMessage)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  GstTagList *pTagList = NULL;

  std::cout << "+ TAG +";
  gst_message_parse_tag (pMessage, &pTagList);

  if (pTagList != NULL) {
    gst_tag_list_foreach (pTagList, collectTags, self);
    gst_tag_list_free (pTagList);
  }
  checkSupported (self);
}

void
Pipeline::handleStateMsgPlay (gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  std::cout << "[BUS]" << __FUNCTION__ << __LINE__ << endl;

  if (self->m_gstPipelineState != PlayingState) {
    self->m_gstPipelineState = PlayingState;

    if (!self->m_bPlaybackStarted) {
      self->m_bPlaybackStarted = TRUE;
#if 0                           //TODO.. play exception handling must add.
      // handle invalid video file, not supported audio codec . // check at start time.
      if (_LMF_PLYR_BUS_CheckNotSupportedVideoMessage (pPlayerHandle) == TRUE)  // if sending CB done
      {
        LMF_DBG_PRINT
            ("[BUS][CB] send pending_not_support_message done. skip send MEDIA_CB_MSG_PLAYSTART !!! \n");
        //pPlayerHandle->bPlaybackStarted = TRUE; //skip send MEDIA_CB_MSG_PLAYSTART
        self->isCheckPendingMultiTrackSetting = TRUE;   // skip pending multi audio check.
      } else
#endif
      {
        std::cout << "[BUS][->NOTI] MEDIA_CB_MSG_PLAYSTART" << endl;
        self->pipelineEventNotify (self, MEDIA_CB_MSG_PLAYSTART);
      }
    } else {
      std::cout << "[BUS][->NOTI] MEDIA_CB_MSG_RESUME_DONE" << endl;
      self->pipelineEventNotify (self, MEDIA_CB_MSG_RESUME_DONE);
    }

    if (self->m_bPlayStatePreProcessingDone == false) {
#if 0                           //TDODO.. play excetion handling must add. - 기존 UI에서 대응하지 못하여 noti timing을 이동. 5.0에서 clear 필요.
      if (_LMF_PLAYBIN2_checkPendingEOS (pPlayerHandle) == true) {
        LMF_DBG_PRINT ("[BUS][CB] processed pending EOS, quick return. !!! \n");
        return;
      }

      if (LMF_PLAYBIN2_CheckPendingMultiTrackSetting (pPlayerHandle) == false) {
        std::cout << "[BUS][CB]error set pending_multi_track_setting!!! " <<
            endl;
      }
#endif
      self->m_bPlayStatePreProcessingDone = true;

#if 0                           //TDODO.. play excetion handling must add. - 기존 UI에서 대응하지 못하여 noti timing을 이동. 5.0에서 clear 필요.
      // started time에 not supported도 보낸다. UI에서 play start msg 이후에 CB 받을 수 있다는 요청에 대응.
      if (_LMF_PLYR_BUS_CheckPendingNotSupportedAudioMessage (pPlayerHandle) ==
          false) {
        std::cout << "[BUS][CB]error send pending_not_support_message!!!" <<
            endl;
      }
      if (_LMF_PLYR_BUS_CheckPendingAdditionalVideoMessage (pPlayerHandle) ==
          false) {
        std::cout << "[BUS][CB]error send PendingAdditionalVideoMessage!!!" <<
            endl;
      }
#endif
    } else {
      std::cout <<
          "[BUS]LMF_PLAYBIN2_CheckPendingMultiTrackSetting already done!" <<
          endl;
    }
    if (isSeekable (self)) {
      //TODO LMF_PLYR_ApplyPendingSeek(pPlayerHandle->ch);
    } else {
      self->stateChanged (self->m_gstPipelineState);
      std::
          cout << "[BUS][-> NOTI] stateChanged: " << self->m_gstPipelineState <<
          endl;
    }
    std::cout << "[BUS][CB - GST_STATE_PLAYING]state changed - PlayingState  "
        << endl;
  } else {
    std::cout << "[BUS][CB - GST_STATE_PLAYING]state - PlayingState  " << endl;
  }

  return;
}

void
Pipeline::handleStateMsgPause (gpointer data, GstState oldState)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  Pipeline::State prevState = self->m_gstPipelineState;

  std::cout << "[BUS] handleStateMsgPause   " << endl;

  //if (!LMF_PLAYBIN2_IsUsing(pPlayerHandle))
  //      LMF_STATIC_COMM_CheckTimeToDecode(pPlayerHandle->ch); // TODO : only custom pipeline.

  self->m_gstPipelineState = PausedState;

  //check for seekable
  if (oldState == GST_STATE_READY) {
    std::cout << "[BUS][CB - GST_STATE_PAUSED] oldstate - GST_STATE_READY";

    //TODO LMF_PLAYBIN2_GetStreamsInfo(pPlayerHandle); /// genericpipeline only...

    std::cout << "[BUS][%s]update duration at PAUSE STATS " << endl;
    //TODO _LMF_PLYR_BUS_GetStreamLength(pPlayerHandle);

    if (self->m_playbinVersion != -1)   /// genericpipeline only...
    {
      std::cout << "[BUS][P2] Update Video Info & Register Unerrun Cb " << endl;
      //TODO LMF_PLAYBIN2_UpdateVideoInfo(self);
      //TODO LMF_PLAYBIN2_RegisterUnderrunSignalHandler(self);
    }

    if (self->m_seekable == FALSE) {
      /*
         flash format 파일에 대해 push based 에서 seek 불가한 경우에 대한 처리 추가
         flash format에서 header에 index data가 없는 경우 push based(PLEX, etc...) 에서 seek 수행이 안됨. seek 시도 시 beginning position으로 돌아감.
         문제 case의 경우 seekable = FALSE로 정보 전달하여 Smartshare UI에서 "This function is not available" popup 띄우도록 처리함
       */
      if ((self->m_source_format == MEDIA_FORMAT_FLV) && (self->isSeekable (self) == FALSE))    //only FLV case.
        self->setSeekable (self, FALSE);
      else
        self->setSeekable (self, TRUE);
    }

    if (!self->compareDouble (self->m_playbackRate, 1.0)) {
      double rate = self->m_playbackRate;
      self->m_playbackRate = 1.0;
      if (self->setPlaybackRate (self, rate) == false) {
        std::cout << "[BUS][CB - set playbackRate] FAIL!!!!. error!!!" << endl;
      }
    }

    std::cout << "[BUS][CB - GST_STATE_PAUSED] oldstate - GST_STATE_READY 2" <<
        endl;
  }

  if (self->m_gstPipelineState != prevState) {
    self->stateChanged (self->m_gstPipelineState);
    std::
        cout << "[BUS][-> NOTI] stateChanged: " << self->m_gstPipelineState <<
        endl;
  }
  return;
}

void
Pipeline::handleBusStateMsg (gpointer data, GstMessage * pMessage)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  GstState oldState = GST_STATE_NULL;
  GstState newState = GST_STATE_NULL;
  GstState pending = GST_STATE_NULL;

  gst_message_parse_state_changed (pMessage, &oldState, &newState, &pending);

  // playback started 가 늦게 체크되어 0sec file 재생시 꼬이는 문제 발생. playbin case는 안그리도록 함.
  // DEBUG : to draw the dot graph (in static case)
  if (self->m_playbinVersion == -1) {
    gchar *pStateName = NULL;

    pStateName = g_strdup_printf ("%s_%s",
        gst_element_state_get_name (oldState),
        gst_element_state_get_name (newState));

    std::cout << "[BUS] Get STATE Changed message..." <<
        gst_element_state_get_name (oldState) << " -> " <<
        gst_element_state_get_name (newState);

    GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (self->m_pipeHandle),
        GST_DEBUG_GRAPH_SHOW_ALL, pStateName);

    g_free (pStateName);
  }

  switch (newState) {
    case GST_STATE_VOID_PENDING:       /* next */
    case GST_STATE_NULL:
    {
      self->setSeekable (false);
      self->m_bPlaybackStarted = false;

      if (self->m_gstPipelineState != StoppedState) {
        self->m_gstPipelineState = StoppedState;
        self->stateChanged (self->m_gstPipelineState);
        std::cout << "[BUS][CB GST_STATE_NULL]state changed - StoppedState " <<
            endl;
      } else {
        std::cout << "[BUS][%s][%d][CB GST_STATE_NULL] state - StoppedState " <<
            endl;
      }

      break;
    }
    case GST_STATE_READY:
    {
      std::cout << "[BUS][ch:%d][CB - GST_STATE_READY] start " << endl;

      self->setSeekable (false);
      self->m_bPlaybackStarted = false;

      if (self->m_gstPipelineState != StoppedState) {
        self->m_gstPipelineState = StoppedState;
        self->stateChanged (self->m_gstPipelineState);
        std::cout << "[BUS][CB - GST_STATE_READY]state changed - StoppedState "
            << endl;
      } else {
        std::cout << "[BUS][CB - GST_STATE_READY]state - StoppedState \n" <<
            endl;
      }
      break;
    }
    case GST_STATE_PAUSED:
    {
      self->handleStateMsgPause (self, oldState);
      break;
    }
    case GST_STATE_PLAYING:
    {
      self->handleStateMsgPlay (self);
      break;
    }
  }                             // end switch
}

void
Pipeline::handleBusEOS (gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  if (self->m_bPlaybackStarted) {
    if ((self->m_duration > 0) && isSeekable (self))    // add for 1sec mp3 playback cur-position update.
    {
      // add for 1sec mp3 playback cur-position update.
      if (self->m_playbackRate >= 0) {  /* forward play */
        self->m_currentPosition = self->m_duration;
      } else {                  /* rewind play */

        self->m_currentPosition = 0;
      }
    }
    if (self->m_bNeedToRestart == false) {
      std::cout << "[BUS EOS] send MEDIA_CB_MSG_PLAYEND. " << endl;
      self->pipelineEventNotify (self, MEDIA_CB_MSG_PLAYEND);
    } else                      // ASF live case.
    {
      if (self->m_bAsfLive == true && self->m_duration > 0)     // local file ASF live play case.
      {
        std::cout <<
            "[BUS EOS] send MEDIA_CB_MSG_PLAYEND at (ASF live + local file play case)."
            << endl;
        self->pipelineEventNotify (self, MEDIA_CB_MSG_PLAYEND);
        // asf demuxer 에서 file play 인 경우만 duration 제공. 이외는 0.
      } else if (self->m_isDLNA == true) {      // WMP(win8) DLNA DMC 재생 시, ASF live stream 으로 재생.
        std::cout <<
            "[BUS EOS] send MEDIA_CB_MSG_PLAYEND at (ASF live + DLNA play case). "
            << endl;
        self->pipelineEventNotify (self, MEDIA_CB_MSG_PLAYEND);
      } else {                  /* live asf - protocol module에서 eos detect 시 재생 retry (requirement of ORANGE pl sport) */
        std::cout <<
            "[BUS EOS] send MEDIA_CB_MSG_REQ_ONLY_PLAY_AGAIN at (ASF live stream)."
            << endl;
        self->pipelineEventNotify (self, MEDIA_CB_MSG_REQ_ONLY_PLAY_AGAIN);
      }
    }
  } else {
    std::cout << "[BUS EOS][%s:%d] EOS callback came so early. >>> pending EOS."
        << endl;
    self->m_bPendingEOS = true;
  }
}

gboolean Pipeline::handleBusPlayerMessage (gpointer data, GstMessage * pMessage)
{
  Pipeline *
      self = reinterpret_cast < Pipeline * >(data);

  switch (GST_MESSAGE_TYPE (pMessage)) {
    case GST_MESSAGE_STATE_CHANGED:
    {
      self->handleBusStateMsg (self, pMessage);
      break;
    }
    case GST_MESSAGE_EOS:
    {
      std::cout << "[BUS] GST_MESSAGE_EOS ^0^; " << endl;
      self->handleBusEOS (self);
      break;
    }
    case GST_MESSAGE_ERROR:
    {
      GError *
          pErr = NULL;
      gchar *
          pDebug = NULL;

      gst_message_parse_error (pMessage, &pErr, &pDebug);
      if ((pErr->domain == GST_STREAM_ERROR)
          && (pErr->code == GST_STREAM_ERROR_CODEC_NOT_FOUND)) {
        self->pipelineEventNotify (self, MEDIA_CB_MSG_ERR_CODEC_NOT_SUPPORTED);
        std::cout <<
            "[BUS][format error] Cannot play stream of type: <unknown>";
      } else {
        self->pipelineEventNotify (self, MEDIA_CB_MSG_ERR_PLAYING);
        std::cout <<
            "[BUS][resource error] Cannot play stream of type: <unknown>";
      }

      std::cout << "[BUS] Error: " << pErr->message;

      g_error_free (pErr);
      g_free (pDebug);

      self->m_bPlaybackStopped = true;
      return false;             // means no more bus msg

      break;
    }
    case GST_MESSAGE_WARNING:
    {
      GError *
          pErr = NULL;
      gchar *
          pDebug = NULL;

      gst_message_parse_warning (pMessage, &pErr, &pDebug);
      std::cout << "Warning:" << pErr->message;
      g_error_free (pErr);
      g_free (pDebug);

      break;
    }
#ifdef DEBUG_PLAYBIN
    case GST_MESSAGE_INFO:
    {
      GError *
          err;
      gchar *
          debug;

      gst_message_parse_info (pMessage, &err, &debug);
      std::cout << "Info:" << err->message;
      g_error_free (err);
      g_free (debug);

      break;
    }
#endif
    case GST_MESSAGE_SEGMENT_START:
    {
      const GstStructure *
          pStructure = gst_message_get_structure (pMessage);
      gint64
          position =
          g_value_get_int64 (gst_structure_get_value (pStructure, "position"));

      self->m_currentPosition = position;
      break;
    }
#if (GST_VERSION_MAJOR >= 0) &&  (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 13)
    case GST_MESSAGE_ASYNC_DONE:
    {
      // TODO _LMF_PLYR_BUS_PlayerMsgAsyncDone(pPlayerHandle);
      break;
    }
    case GST_MESSAGE_ASYNC_START:      /* next */
#if GST_VERSION_MICRO >= 23
    case GST_MESSAGE_REQUEST_STATE:    /* next */
#endif
#endif // END GST VERSION
    case GST_MESSAGE_ANY:      /* next */
    case GST_MESSAGE_STREAM_STATUS:    /* next */
    case GST_MESSAGE_TAG:      /* next */
    case GST_MESSAGE_UNKNOWN:  /* next */
    case GST_MESSAGE_BUFFERING:        /* next */
    {
      int
          progress = 0;
      gst_message_parse_buffering (pMessage, &progress);
      //TODO notify bufferingProgressChanged(progress);
    }
      break;
    case GST_MESSAGE_STATE_DIRTY:      /* next */
    case GST_MESSAGE_STEP_DONE:        /* next */
    case GST_MESSAGE_CLOCK_PROVIDE:    /* next */
    case GST_MESSAGE_CLOCK_LOST:       /* next */
    case GST_MESSAGE_NEW_CLOCK:        /* next */
    case GST_MESSAGE_STRUCTURE_CHANGE: /* next */
    case GST_MESSAGE_APPLICATION:      /* next */
    case GST_MESSAGE_SEGMENT_DONE:     /* next */
    case GST_MESSAGE_LATENCY:  /* next */
    default:
    {
      break;
    }
  }

  return true;
}

gboolean
    Pipeline::handleBusElementMessage (gpointer data, GstMessage * pMessage)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  GError *pErr = NULL;
  gchar *pDebug = NULL;

  if (GST_MESSAGE_TYPE (pMessage) != GST_MESSAGE_ERROR)
    return true;

  gst_message_parse_error (pMessage, &pErr, &pDebug);

  std::cout << "[BUS]GST_MESSAGE_ERROR: " << (pErr->domain ==
      GST_STREAM_ERROR ? "GST_STREAM_ERROR" : (pErr->domain ==
          GST_CORE_ERROR ? "GST_CORE_ERROR" : (pErr->domain ==
              GST_RESOURCE_ERROR ? "GST_RESOURCE_ERROR" : (pErr->domain ==
                  GST_LIBRARY_ERROR ? "GST_LIBRARY_ERROR" : "UNKNOWN"))))
      << " - " << pErr->code << " " <<
      (GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage))) << " " << pErr->
      message << " " << pDebug << endl;

  /* error 중 무시되어야 하는 경우는 여기에 삽입 */
  if ((strncmp (GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage)), "queue2",
              strlen ("queue2")) == 0) && (pErr->domain == GST_STREAM_ERROR)
      && (pErr->code == GST_STREAM_ERROR_FAILED)
      && (strstr (pDebug, "streaming task paused, reason error (-5)") != NULL)) {
    std::cout << "[BUS][%s:%d] error from gstqueue2, ignored!!!";
  } else if (((strstr (GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage)), "aacdec") != 0) || //MTK: omxaacdec0
          (strstr (GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage)), "adec") != 0)) &&      //L9: adecsink0
      // 추후 codec 추가 경우를 대비하려면 debug 에서 adecsink 로 검색 (MTK: mtkadecsink0 / L9: gstadecsink0)
      // & side effect check 필요. 일단 위와 같이 문제 케이스만 커버하도록 수정함.
      (pErr->domain == GST_STREAM_ERROR) &&
      (pErr->code == GST_STREAM_ERROR_DECODE)) {
    std::cout << "[BUS] decode error from adecsink!!!";
    self->pipelineEventNotify (self, MEDIA_CB_MSG_ERR_AUDIO_DECODING_FAILED);   //(MSG_MF2UI_AUDIO_DECODING_FAILED);
  } else if ((strncmp (GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage)), "source",
              strlen ("source")) == 0) && (self->m_bNeedToRestart == true)) {
    /* live asf - protocol module want to play againg at error detect case (requirement of ORANGE pl sport) */
    std::cout <<
        "[BUS][%s:%d] error from soup http (asf live) -> play again!!!";
    self->pipelineEventNotify (self, MEDIA_CB_MSG_REQ_ONLY_PLAY_AGAIN);
  } else if ((strncmp (GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage)), "source",
              strlen ("source")) == 0)
      && (strncmp (G_OBJECT_TYPE_NAME (GST_MESSAGE_SRC (pMessage)),
              "GstSoupHTTPSrc", strlen ("GstSoupHTTPSrc")) == 0)
      && (pErr->domain == GST_STREAM_ERROR)
      && (pErr->code == GST_STREAM_ERROR_WRONG_TYPE)) {
    // http url to mms url redirection CASE.
    std::cout << "[BUS]....... " <<
        g_type_name (G_OBJECT_TYPE (GST_MESSAGE_SRC (pMessage)));
    std::cout << "[BUS]....... " <<
        G_OBJECT_TYPE_NAME (GST_MESSAGE_SRC (pMessage));

    //[message to upper layer]FormatError  "Cannot play stream of type: <unknown>"
    std::cout << "[BUS] error from source with new url -> play again!!";
    std::cout << "[BUS] msg:" << pErr->message;
    std::cout << "[BUS] debug:" << pDebug;
#if 0                           //TODO... for uri redirection..
    if ((strstr (pErr->message, "souphttpsrc:") != NULL) &&
        (strlen (pDebug) > strlen ("mms://"))) {
      LMF_PLYR_CTRL_Stop (pPlayerHandle, false);

      pPlayerHandle->pNewUri = (char *) malloc (strlen (pDebug) + 1);
      if (pPlayerHandle->pNewUri == NULL) {
        LMF_ERR_PRINT ("[BUS][%s:%d] pNewUri MF_MAIN_Malloc failed.\n",
            __FUNCTION__, __LINE__);
      } else {
        memcpy (pPlayerHandle->pNewUri, pDebug, strlen (pDebug));
        pPlayerHandle->pNewUri[strlen (pDebug)] = '\0';

        //TODO std::cout << "[BUS][%s] new uri :" << __FUNCTION__ << self->m_pNewUri);

        self->pipelineEventNotify (self, MEDIA_CB_MSG_REQ_ONLY_PLAY_AGAIN);
      }
    }
#endif
  } else {                      /* 위에 해당되지 않는 경우는 error 올려주면  MF 단에서 Stop 명령 내려옴 */

    //TODO _LMF_PLYR_BUS_ConvertGstMsgToLmf(pPlayerHandle, pErr->domain, pErr->code);
    //TODO _LMF_PLYR_BUS_EmitErrorLogToBSI(pMessage, pErr, pDebug);
  }

  g_error_free (pErr);
  g_free (pDebug);

  return true;
}

gint64 Pipeline::getStreamLength (gpointer data)
{
  Pipeline *
      self = reinterpret_cast < Pipeline * >(data);

  if (self == NULL || self->m_pipeHandle == NULL) {
    std::cout << "[BUS][%s:%d] Error. Player Handle is NULL!!!" << endl;
    return false;
  }

  std::cout << "[BUS][%s] try to get duration." << endl;

  if (self->m_duration == 0) {
    GstFormat
        queryFormat = GST_FORMAT_TIME;
    gint64
        len = -1;
#if (GST_VERSION_MAJOR >= 1)
    if (gst_element_query_duration (self->m_pipeHandle, queryFormat, &len)
        && (len != -1))
#else
    if (gst_element_query_duration (self->m_pipeHandle, &queryFormat, &len)
        && (len != -1))
#endif
    {
      self->m_duration = len;
      std::cout << "[BUS][%s] duration update: " << self->m_duration << endl;
    } else {
      std::cout << "[BUS][%s] try to get duration - fail. ";
    }
  } else {
    std::cout <<
        "[BUS][%s] try to get duration - element NULL or streamLength get already done. ";
  }

  return self->m_duration;
}

void
Pipeline::handleBusDuration (gpointer data, GstMessage * pMessage)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  GstFormat fmt = GST_FORMAT_UNDEFINED;
  gint64 duration = 0;

  // query 값은 0 이면서 따로 GST_MESSAGE_DURATION 올려주는 경우의 처리 들어감.
  if (getStreamLength (self) >= 0) {
    std::cout <<
        "[BUS] update duration at GST_MESSAGE_DURATION [_gSessionDuration:" <<
        (gint64) self->m_duration << endl;
  } else {
    std::cout << "[BUS] Failed to query duration after DURATION message?! ";
  }

  gst_message_parse_duration (pMessage, &fmt, &duration);
  if ((duration >= 0) && (fmt == GST_FORMAT_BYTES)) {
    // Sometimes GST_MESSAGE_DURATION comes twice, once with -1; (http, mp3 play)
    //DBG_PRINT("[BUS] filesize: %"G_GINT64_FORMAT" (%s)\n", duration, gst_format_get_name(fmt));
    std::cout << "[BUS] filesize:" << duration << "G_GINT64_FORMAT:" <<
        gst_format_get_name (fmt) << endl;
    self->m_source_dataSize = (guint64) duration;
  }
}


void
Pipeline::handleBusApplication (gpointer data, GstMessage * pMessage)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);

  const GstStructure *pStructure = NULL;
  pStructure = gst_message_get_structure (pMessage);

  if (gst_structure_has_name (pStructure, "GstMessageLive")) {
    gboolean bIsLive = false;

    // live asf - error(protocol)/eos detect 시 재생 retry 를 하기 위해 live 인지 check.
    bIsLive =
        g_value_get_boolean (gst_structure_get_value (pStructure, "LIVE"));
    self->m_bNeedToRestart = bIsLive;
    self->m_bAsfLive = bIsLive;

    if (bIsLive == true) {
      self->m_bLiveStreaming = true;
      self->m_source_bIsValidDuration = false;
      self->m_bSeekableDuration = false;
    }

    std::
        cout << "[BUS][GST_MESSAGE_APPLICATION] - is live of ASF:" <<
        ((self->m_bNeedToRestart == true) ? "OO" : "XX") << endl;
  } else if (gst_structure_has_name (pStructure, "GstMessageSeekable")) {
    gboolean bIsSeekable = true;

    bIsSeekable =
        g_value_get_boolean (gst_structure_get_value (pStructure, "SEEKABLE"));
    self->m_bSeekableIndex = bIsSeekable;

    std::cout << "[BUS][GST_MESSAGE_APPLICATION] - seekable: " << ((bIsSeekable
            == true) ? "OO" : "XX") << endl;
  } else if (gst_structure_has_name (pStructure, "GstMessageAudio"))    // demuxer가 invalid audio 로 판단
  {
    gboolean bValidAudio = true;

    bValidAudio =
        g_value_get_boolean (gst_structure_get_value (pStructure, "AUDIO"));

    std::cout << "[BUS][GST_MESSAGE_APPLICATION] - bValiadAudio:" <<
        ((bValidAudio == true) ? "OO" : "XX") << endl;
    // NULL to READY 에 올라온다. recv at demuxer probing time.
    if (bValidAudio == false)
      self->m_bPendingNotSupportedAudioMessage = true;  // save. msg.
  } else if (gst_structure_has_name (pStructure, "GstMessageVideo"))    // demuxer가 invalid video 로 판단
  {
    gboolean bValidVideo = true;
    const GValue *pValidVideo = NULL;

    pValidVideo = gst_structure_get_value (pStructure, "VIDEO");
    if (pValidVideo)            // VIDEO message case. //(G_VALUE_TYPE (value) == G_TYPE_BOOLEAN)
    {
      bValidVideo = g_value_get_boolean (pValidVideo);

      std::cout << "[BUS][GST_MESSAGE_APPLICATION] - bValidVideo:" <<
          ((bValidVideo == true) ? "OO" : "XX") << endl;
      if (bValidVideo == false)
        self->m_bPendingNotSupportedVideoMessage = true;        // save. msg.
    }
  }
}

gboolean
    Pipeline::gstBusCallbackHandle (GstBus * pBus, GstMessage * pMessage,
    gpointer data)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  //TODO: change PERI LOG print.
  std::cout << "Pipeline] bus callback msg - element:" <<
      GST_OBJECT_NAME (GST_MESSAGE_SRC (pMessage)) << " | name:" <<
      gst_message_type_get_name (GST_MESSAGE_TYPE (pMessage)) << endl;

  if (self->m_pipeHandle == NULL) {     //(GST_IS_ELEMENT(pPlayerHandle->player) == false))
    std::cout << "[BUS]Error. Player Handle is NULL!!! " << endl;
    return false;
  }
  if (GST_MESSAGE_TYPE (pMessage) == GST_MESSAGE_TAG) {
    self->handleBusTag (self, pMessage);
  } else if (GST_MESSAGE_TYPE (pMessage) == GST_MESSAGE_DURATION) {
    self->handleBusDuration (self, pMessage);
  } else if (GST_MESSAGE_TYPE (pMessage) == GST_MESSAGE_APPLICATION) {  //extra application message handling
    self->handleBusApplication (self, pMessage);
  }

  if (GST_MESSAGE_SRC (pMessage) == GST_OBJECT_CAST (self->m_pipeHandle)) {     // playbin case message handling
    if (self->handleBusPlayerMessage (self, pMessage) == false)
      return false;             // false means no more bus msg
  } else {
    if (self->handleBusElementMessage (self, pMessage) == false)
      return false;             // false means no more bus msg
  }
  return true;
}

/*
* signal connect for gstreamer bus callback.
*/
bool Pipeline::connectGstBusCallback ()
{
  if (m_pipeHandle == NULL) {
    std::cout << " Error. Gstreamer Player Handle is NULL!!!" << endl;
    return false;
  }
  m_busHandler = gst_pipeline_get_bus (GST_PIPELINE (m_pipeHandle));
  if (m_busHandler) {
    gst_bus_add_signal_watch (m_busHandler);
    m_sigBusAsync =
        g_signal_connect (m_busHandler, "message",
        G_CALLBACK (gstBusCallbackHandle), this);
  } else
    return false;

  return true;
}

//end buscallback handle

//start seek, trick

void
Pipeline::setSeekable (bool seekable)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  if (seekable != m_seekable) {
    m_seekable = seekable;
    seekableStateChanged (m_seekable);  // notify seekable state changed.
  }
}

void
Pipeline::setSeekable (gpointer data, bool seekable)
{
  Pipeline *self = reinterpret_cast < Pipeline * >(data);
  LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
  if (seekable != self->m_seekable) {
    self->m_seekable = seekable;
    self->seekableStateChanged (self->m_seekable);      // notify seekable state changed.
  }
}

     gfloat Pipeline::playbackRate () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("Pipeline");
       return
           m_playbackRate;
     }

gboolean
Pipeline::setPlaybackRate (gfloat rate)
{
  return setPlaybackRate (this, rate);
}

gboolean Pipeline::setPlaybackRate (gpointer data, gfloat rate)
{
  Pipeline *
      self = reinterpret_cast < Pipeline * >(data);

  if (!self->compareDouble (self->m_playbackRate, rate)) {
    self->m_playbackRate = rate;
    if (self->m_playbackRate) {
      if (gst_element_seek (self->m_pipeHandle, rate, GST_FORMAT_TIME,
              GstSeekFlags (GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
              GST_SEEK_TYPE_NONE, 0, GST_SEEK_TYPE_NONE, 0) != true)
        return false;
    }
    self->playbackRateChanged (self->m_playbackRate);   //notify playback rate changed.
  }
  return true;
}

bool Pipeline::compareDouble (const double num1, const double num2)
{
  const double
      precision = 0.00001;

  if (((num1 - precision) < num2) && ((num1 + precision) > num2))
    return true;
  else
    return false;
}

// end seek, trick.









BufferController::BufferController ()
{

}

BufferController::~BufferController ()
{

}
