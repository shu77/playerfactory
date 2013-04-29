/*
 * genericpipeline.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include <pthread.h>
#include <gio/gio.h>

//#include <gst/app/gstappbuffer.h>//playbin+appsrc not support at NC5.0
#include <glibmm-utils/glibmm-utils.h>
#include <pipeline/generic/genericpipeline.hpp>

using namespace mediapipeline;

GenericPipeline::GenericPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");


}

gboolean GenericPipeline::initSpi_pre ()
{
  m_pipeHandle = gst_element_factory_make ("playbin2", NULL);
  if (m_pipeHandle) {
    m_playbinVersion = 2;       //playbin2
    int
        flags = 0;
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
}

gboolean GenericPipeline::initSpi_post ()
{
  // connect source notify. 
  g_signal_connect (G_OBJECT (m_pipeHandle), "notify::source",
      G_CALLBACK (playbinNotifySource), this);
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

bool GenericPipeline::handleURI (MEDIA_CLIPOPT_T * clipOpt)
{
  if (m_pipeHandle) {
    GFile *
        pTmpFile = NULL;
    char *
        pNewUri = NULL;
    char *
        pTmpPath = NULL;

    if (!strncmp (clipOpt->mediafile, "mms://", strlen ("mms://"))) {
      // fix for maxdome FF issue. (set serverside tirck enable at mms streamming.)
      //TODO: BASIC_PLYR_CTRL_SetMmsURI(pPipeContainerHandle, true);
    }
    // DQMS 1206-00199 (mlm can't filtering this)
    // not supported known type media filtering.
    if ((strstr (clipOpt->mediafile, ".ogm") != NULL)
        || (strstr (clipOpt->mediafile, ".ogv") != NULL)
        || (strstr (clipOpt->mediafile, ".OGM") != NULL)
        || (strstr (clipOpt->mediafile, ".OGV") != NULL)) {
      //TODO :: API_BASIC_EVENT_Notify(pPipeContainerHandle, MEDIA_CB_MSG_ERR_PLAYING);
      return false;
    }
    /* this allows non-URI type of files in the thumbnailer and so on */
    pTmpFile = g_file_new_for_commandline_arg (clipOpt->mediafile);
    if (pTmpFile == NULL) {
      pNewUri = g_strdup (clipOpt->mediafile);
    } else {
      pTmpPath = g_file_get_path (pTmpFile);
      if (pTmpPath) {
        pNewUri = g_filename_to_uri (pTmpPath, NULL, NULL);
        g_free (pTmpPath);
      } else {
        pNewUri = g_strdup (clipOpt->mediafile);
      }
      g_object_unref (pTmpFile);
    }
    m_uri.clear ();
    m_uri.append (pNewUri);
    if (pNewUri != NULL)
      g_free (pNewUri);
  } else
    return false;

  return true;
}

bool GenericPipeline::setExtraElementOption (MEDIA_CLIPOPT_T * clipOpt)
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

gboolean GenericPipeline::load (MEDIA_CLIPOPT_T * clipOpt)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  // setting values
  if (handleURI (clipOpt) == false)
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


gboolean
    GenericPipeline::load (MEDIA_STREAMOPT_T * streamOpt,
    MEDIA_FORMAT_T mediaFormatType)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
  //this->loadSpi(clipOpt);
  // not supported API.
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

#if 0                           //TODO custiom player...
gboolean::isReadyToPlaySpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");

  // play already done -> Resume Case
  if (m_bPlaybackStarted == true) {
    cout << " Resume Case" << endl;
    return true;
  }
  // playbin player case
  LMF_DBG_PRINT ("[%s:%d][ch:%d] Check Playbin2 Case\n", __FUNCTION__, __LINE__,
      ch);
  return FALSE;                 // prebuffering 중...
}
#endif





//Error GenericPipeline::error() const;
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



/*
* set gstreamer debug LOG level.
*/
bool GenericPipeline::setGstreamerDebugLevel (guint select, gchar * category,
    GstDebugLevel level)
{
  GError *
      err;
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
