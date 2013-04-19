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

GenericPipeline::GenericPipeline() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    
    m_pipeHandle = gst_element_factory_make("playbin2", NULL);
    if(m_pipeHandle)
    {
        m_playbinVersion = 2; //playbin2
        int flags = 0;
        //GST_PLAY_FLAG_NATIVE_VIDEO omits configuration of ffmpegcolorspace and videoscale
        g_object_get(G_OBJECT(m_pipeHandle), "flags", &flags, NULL);
        flags |= GST_PLAY_FLAG_NATIVE_VIDEO;
        g_object_set(G_OBJECT(m_pipeHandle), "flags", flags, NULL);
        cout << "genericpipeline] created playbin2 pipeline" << endl;

    }
	else
	{
        m_pipeHandle = gst_element_factory_make("playbin", NULL);
        if(m_pipeHandle)
            cout << "genericpipeline] created playbin pipeline" << endl;
        else
            cout << "genericpipeline] fail create playbin pipeline" << endl;

        m_playbinVersion = 1; //playbin

	}

    connectGstBusCallback();
    
    cout << "set pipe to NULL";
    
    gst_element_set_state(m_pipeHandle, GST_STATE_NULL);
    
    cout << "set buffer size";
    // default buffer-size (see gstqueue2)
    g_object_set(G_OBJECT(m_pipeHandle), "buffer-size", (gint)(MEDIAPIPE_BUFFER_SIZE), NULL);
    g_object_set(G_OBJECT(m_pipeHandle), "buffer-duration", (gint64)(0), NULL);
    
    // connect source notify. 
    g_signal_connect(G_OBJECT(m_pipeHandle), "notify::source", G_CALLBACK(playbinNotifySource), this);

    // connect volume notify.
    double volume = 1.0;
    g_object_get(G_OBJECT(m_pipeHandle), "volume", &volume, NULL);
    m_volume = int(volume*100);

    g_signal_connect(G_OBJECT(m_pipeHandle), "notify::volume", G_CALLBACK(handleVolumeChange), this);
    if (m_playbinVersion == 2)
        g_signal_connect(G_OBJECT(m_pipeHandle), "notify::mute", G_CALLBACK(handleMutedChange), this);	

    cout << "genericpipeline] create finish " << endl;
}

GenericPipeline::~GenericPipeline() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

bool GenericPipeline::loadFromURI(){
    if(m_pipeHandle)
    {
        GstBus     *pBus = NULL;
        // message를 막기 위해 flushing 한다.
        pBus = gst_element_get_bus(m_pipeHandle);
        gst_bus_set_flushing(pBus, TRUE);
        gst_element_set_state(m_pipeHandle, GST_STATE_READY);
        gst_bus_set_flushing(pBus, FALSE);
        gst_object_unref(pBus);
    
        g_object_set(G_OBJECT(m_pipeHandle), "uri", m_uri.c_str(), NULL); //c-string copy.

        //TODO: notify stream change status. if possible.
    }
    else
        return false;

    return true;
}

bool GenericPipeline::handleURI(MEDIA_CLIPOPT_T *clipOpt){
    if(m_pipeHandle)
    {
        GFile *pTmpFile = NULL;
        char *pNewUri = NULL;
        char *pTmpPath = NULL;

        if (!strncmp(clipOpt->mediafile, "mms://", strlen("mms://")))
        {
            // fix for maxdome FF issue. (set serverside tirck enable at mms streamming.)
            //TODO: BASIC_PLYR_CTRL_SetMmsURI(pPipeContainerHandle, TRUE);
        }
        
        // DQMS 1206-00199 (mlm can't filtering this)
        // not supported known type media filtering.
        if((strstr(clipOpt->mediafile, ".ogm") != NULL)||(strstr(clipOpt->mediafile, ".ogv") != NULL)
        ||(strstr(clipOpt->mediafile, ".OGM") != NULL)||(strstr(clipOpt->mediafile, ".OGV") != NULL))
        {
            //TODO :: API_BASIC_EVENT_Notify(pPipeContainerHandle, MEDIA_CB_MSG_ERR_PLAYING);
            return false;
        }
         /* this allows non-URI type of files in the thumbnailer and so on */
        pTmpFile = g_file_new_for_commandline_arg(clipOpt->mediafile);
        if (pTmpFile == NULL)
        {
            pNewUri = g_strdup(clipOpt->mediafile);
        }
        else
        {
            pTmpPath = g_file_get_path(pTmpFile);
            if (pTmpPath)
            {
                pNewUri = g_filename_to_uri(pTmpPath, NULL, NULL);
                g_free(pTmpPath);
            }
            else
            {
                pNewUri = g_strdup(clipOpt->mediafile);
            }
            g_object_unref(pTmpFile);
        }
        m_uri.clear();
        m_uri.append(pNewUri);
        if (pNewUri != NULL)
            g_free(pNewUri);
    }
    else
        return false;

    return true;
}

bool GenericPipeline::setExtraElementOption(MEDIA_CLIPOPT_T *clipOpt){
    if (m_pipeHandle) 
    {
#if 0 //TODO: 구현..
        // DLNA info setting
        if(pPipeContainerHandle->isDLNA)
        {
            g_object_set(G_OBJECT(pPipeContainerHandle->m_pipeline), "dlna-protocolinfo", (gchar *)(pPipeContainerHandle->pDlnaInfo->pProtocolInfo), NULL);
            g_object_set(G_OBJECT(pPipeContainerHandle->m_pipeline), "dlna-contentlength", (guint64)(pPipeContainerHandle->pDlnaInfo->dContentLength), NULL);
            g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline), "dlna-cleartextsize", (guint64)(pPipeContainerHandle->pDlnaInfo->dCleartextSize), NULL);
            g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline), "dlna-opval", (guint32)(pPipeContainerHandle->pDlnaInfo->opVal), NULL);
            g_object_set (G_OBJECT (pPipeContainerHandle->m_pipeline), "dlna-flagval", (guint32)(pPipeContainerHandle->pDlnaInfo->flagVal), NULL);
        }

        /* DRM type setting */
        //cout << "drm-type:", pPipeContainerHandle->drmType;
        g_object_set (G_OBJECT(pPipeContainerHandle->m_pipeline), "drm-type", (gint)pPipeContainerHandle->drmType, NULL);
#endif
    }
    else
        return false;

    return true;
}

bool GenericPipeline::load(MEDIA_CLIPOPT_T *clipOpt){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    // setting values
    if(handleURI(clipOpt) == false)
        return false;
    
	// connect signals
    //m_eventHelper = new PlayEventListener();
    //connectGstBusCallback(); //move to create time.
	// start timers

	// set media
    cout << "genericpipeline] load file to play: " << m_uri << endl;
    if(loadFromURI() == false)
        return false;
	// etc
    // set DLNA info setting.

    
	// set pause
	if(pause() == false)
        return false;

    return true;
}


bool GenericPipeline::load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType) {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
	//this->loadSpi(clipOpt);
	// not supported API.
}


void GenericPipeline::unload() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    stop();
}

bool GenericPipeline::play(int rate) {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    if (m_pipeHandle) 
	{
        m_pendingState = Pipeline::PlayingState; // pending state //
        if (gst_element_set_state(m_pipeHandle, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
            cout << "genericpipeline] Unable to play : " << m_uri << endl;
            m_pendingState = m_gstPipelineState = Pipeline::StoppedState;

            Pipeline::stateChanged(m_gstPipelineState);
        } else
            return true;
    }
    return false;

}

bool GenericPipeline::pause(){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    if (m_pipeHandle) 
	{
        m_pendingState = Pipeline::PausedState; //pending state.
        if(m_blockByVideoSink != 0) //blocked by videosink.
            return true;
        if (gst_element_set_state(m_pipeHandle, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) 
		{
            std::cout << "genericpipeline] Unable to pause : " << m_uri << endl;
            m_pendingState = m_gstPipelineState = Pipeline::StoppedState;

            Pipeline::stateChanged(m_gstPipelineState); // notify state change.
        } 
		else 
        {
            return true;
        }
    }
    return false;

}

void GenericPipeline::stop(){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    if (m_pipeHandle) {
        //stopping (renderer if existed.) here.

        gst_element_set_state(m_pipeHandle, GST_STATE_NULL);

        m_currentPosition = 0; //update position quickly.
        Pipeline::State oldState = m_gstPipelineState;
        m_pendingState = m_gstPipelineState = Pipeline::StoppedState;

        //finish something like video HW etc..

        //gstreamer will not emit bus messages any more
        setSeekable(false);
        if (oldState != m_gstPipelineState)
            Pipeline::stateChanged(m_gstPipelineState); //notify state change.
    }
}

bool GenericPipeline::seek(gint64 ms)
{
    //seek locks when the video output sink is changing and pad is blocked
    if (m_pipeHandle && !m_blockByVideoSink && m_gstPipelineState != Pipeline::StoppedState) {
        ms = MAX(ms,gint64(0));
        gint64  position = ms * 1000000;
        bool isSeeking = gst_element_seek(m_pipeHandle,
                                          m_playbackRate,
                                          GST_FORMAT_TIME,
                                          GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
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


gint64 GenericPipeline::duration() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    return m_duration;
}
gint64 GenericPipeline::position() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    GstFormat   format = GST_FORMAT_TIME;
    gint64      position = 0;
#if (GST_VERSION_MAJOR >= 1)
    if ( m_pipeHandle && gst_element_query_position(m_pipeHandle, format, &position))
#else
    if ( m_pipeHandle && gst_element_query_position(m_pipeHandle, &format, &position))
#endif
        m_currentPosition = position / 1000000;

    return m_currentPosition;
}
gint GenericPipeline::volume() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isMuted() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isAudioAvailable() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isVideoAvailable() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isSeekable() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
void GenericPipeline::setSeekable(bool seekable){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
	if(seekable != m_seekable){
	    m_seekable = seekable;
        seekableStateChanged(m_seekable); // notify seekable state changed.
	}
}

gfloat GenericPipeline::playbackRate() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    return m_playbackRate;
}

void GenericPipeline::setPlaybackRate(gfloat rate)
{
    if (!compareDouble(m_playbackRate, rate)) {
        m_playbackRate = rate;
        if (m_playbackRate) {
            gst_element_seek(m_pipeHandle, rate, GST_FORMAT_TIME,
                             GstSeekFlags(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH),
                             GST_SEEK_TYPE_NONE,0,
                             GST_SEEK_TYPE_NONE,0 );
        }
        playbackRateChanged(m_playbackRate); //notify playback rate changed.
    }
}

//Error GenericPipeline::error() const;
GString GenericPipeline::errorString() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}

void GenericPipeline::playbinNotifySource(GObject *pObject, GParamSpec *pParam, gpointer u_data){
    GenericPipeline *genericPipeline = reinterpret_cast<GenericPipeline *>(u_data);

    if (g_object_class_find_property(G_OBJECT_GET_CLASS(pObject), "source")){
        GObject             *pSrcElement    = NULL;
        GstBaseSrc          *pBaseSrc       = NULL;
        GstElementFactory   *pFactory       = NULL;
        const gchar         *pName          = NULL;

        // get source element
        g_object_get(pObject, "source", &pSrcElement, NULL);

        //check soup http src
        pFactory = gst_element_get_factory((GstElement *)pSrcElement);
        pName = gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(pFactory));

        if (!strcmp(pName, "souphttpsrc")){ // do only http:
            LOG_FUNCTION_SCOPE_NORMAL_D("[GST_SIGNAL] ##### found soup http src. !!! ##### \n");
            if (g_object_class_find_property(G_OBJECT_GET_CLASS(pSrcElement), "cookies") && g_object_class_find_property(G_OBJECT_GET_CLASS(pSrcElement), "user-agent"))
            {
                //_LMF_PLAYBIN2_SetHttpHeader(pPlayerHandle, (GstElement *)pSrcElement, pPlayerHandle->cookies, pPlayerHandle->userAgent, pPlayerHandle->extraHeader);
            }
            else
            {
                LOG_FUNCTION_SCOPE_NORMAL_D("[GST_SIGNAL]source has no property named \"cookies\" or \"user-agent\"\n");
            }

            if (g_object_class_find_property(G_OBJECT_GET_CLASS(pSrcElement), "timeout"))
            {
                //_LMF_PLAYBIN2_SetSoupHttpSrcTimeOut(pPlayerHandle, (GstElement *)pSrcElement, pPlayerHandle->SoupHttpSrcTimeOut);
            }
            else
            {
                LOG_FUNCTION_SCOPE_NORMAL_D("[GST_SIGNAL]source has no property named \"timeout\" \n");
            }
        }
#if 0 //TODO: ./src/libpf.so: undefined reference to `gst_base_src_get_type'
        pBaseSrc = GST_BASE_SRC(pSrcElement);

        if (g_object_class_find_property(G_OBJECT_GET_CLASS(pBaseSrc), "blocksize"))
        {
            LOG_FUNCTION_SCOPE_NORMAL_D("[GST_SIGNAL] blocksize property found: setting to 24KB\n");
            g_object_set(G_OBJECT(pBaseSrc), "blocksize", (gulong)(24*1024), NULL);
        }
        else
        {
            LOG_FUNCTION_SCOPE_NORMAL_D("[GST_SIGNAL] basesrc has no property named 'blocksize'. \n");
        }
#endif
        gst_object_unref(pSrcElement);
    }

}

bool GenericPipeline::compareDouble(const double num1, const double num2)
{
	const double precision = 0.00001;

	if(((num1 - precision) < num2) && ((num1 + precision) > num2))
		return true;
	else
		return false;
}

void GenericPipeline::handleVolumeChange(GObject *pObject, GParamSpec *pParam, gpointer u_data){


}

void GenericPipeline::handleMutedChange(GObject *pObject, GParamSpec *pParam, gpointer u_data){


}







void GenericPipeline::checkSupported (gpointer data)
{
    GenericPipeline *self = reinterpret_cast<GenericPipeline*>(data);

    if(self->m_pipeHandle == NULL)//(GST_IS_ELEMENT(pPlayerHandle->player) == false))
	{
		std::cout <<"[BUS] Error. LMF Player Handle is NULL!!! ";
		return;
	}
#if 0 //TODO::
	if (((pPlayerHandle->sourceInfo).codec >> 8) == (MEDIA_VIDEO_NOT_SUPPORTED >> 8))
	{
		LMF_DBG_PRINT("[BUS][%s:%d] Codec NOT Supported\n", __FUNCTION__, __LINE__);
		pPlayerHandle->bSendNotSupportedVideoMessageAtPlayState = TRUE;
		m_bSendNotSupportedAudioMessageAtPlayState = TRUE;
		pPlayerHandle->bPendingNotSupportedVideoMessage = FALSE;
		m_bPendingNotSupportedAudioMessage = FALSE;

		LMF_PLYR_ProcessNotSupported(pPlayerHandle->ch);
	}
 #endif   
}

void GenericPipeline::collectTags (const GstTagList *tag_list, const gchar *tag, gpointer user_data)
{
    GenericPipeline *self = reinterpret_cast<GenericPipeline*>(user_data);

	//MEDIA_SOURCE_INFO_T *pSourceInfo = NULL;
	gchar *pStr 	= NULL;
	gint32 idx 		= 0;
	gint32 tagSize 	= 0;

    if(self->m_pipeHandle == NULL)//(GST_IS_ELEMENT(pPlayerHandle->player) == false))
    {
        std::cout <<"[BUS] Error. LMF Player Handle is NULL!!! ";
        return;
    }

	//pSourceInfo = &(pPlayerHandle->sourceInfo);

	/* print tags immediatly when receiving GST_MESSAGE_TAG  */
	tagSize = gst_tag_list_get_tag_size(tag_list, tag);

	for (idx=0 ; idx<tagSize ; idx++)
	{
		gchar *pTagStr = NULL;

		if (gst_tag_get_type(tag) == G_TYPE_STRING)
		{
			if (!gst_tag_list_get_string_index(tag_list, tag, idx, &pTagStr))
			{
				//g_assert_not_reached();
				std::cout <<"Error!!! ================================================= !!!!  ";
				std::cout <<"Error!!! assert gst_tag_list_get_string_index not reached. !!!!  ";
				std::cout <<"Error!!! ================================================= !!!!  ";
			}
		}
		else if (gst_tag_get_type(tag) == GST_TYPE_BUFFER)
		{
			GstBuffer *pGstBuf = NULL;
#if (GST_VERSION_MAJOR >= 1)

#else
			pGstBuf = gst_value_get_buffer(gst_tag_list_get_value_index(tag_list, tag, idx));
			if(pGstBuf)
			{
				gchar *pCapsStr = NULL;

				pCapsStr = ((GST_BUFFER_CAPS(pGstBuf)) ? (gst_caps_to_string(GST_BUFFER_CAPS(pGstBuf))) : (g_strdup ("unknown")));
				pTagStr = g_strdup_printf("buffer of %u bytes, type: %s", GST_BUFFER_SIZE(pGstBuf), pCapsStr);

				g_free (pCapsStr);
			}
			else
			{
				pTagStr = g_strdup("NULL buffer");
			}
#endif           
		}
		else
		{
			pTagStr = g_strdup_value_contents(gst_tag_list_get_value_index(tag_list, tag, idx));
		}

		if (idx == 0)
		{
			std::cout <<"[BUS]" << gst_tag_get_nick(tag) << ":" << pTagStr;
		}
		else
		{
			std::cout <<"[BUS]                 :" << pTagStr;
		}

		g_free(pTagStr);
	}
#if 0 // TODO:CHECK..
	/* save the tag values into pSourceInfo */
	if (strcmp(tag, GST_TAG_TITLE) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			strncpy(m_source_title, pStr, LMF_TITLE_MAX_LEN-1);
			m_source_title[LMF_TITLE_MAX_LEN-1] = '\0';
			g_free(pStr);
		}
	}
	else if (strcmp(tag, GST_TAG_ARTIST) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			strncpy(m_source_artist, pStr, LMF_TITLE_MAX_LEN-1);
			m_source_artist[LMF_TITLE_MAX_LEN-1] = '\0';
			g_free(pStr);
		}
	}
	else if (strcmp(tag, GST_TAG_COPYRIGHT) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			strncpy(m_source_copyright, pStr, LMF_TITLE_MAX_LEN-1);
			m_source_copyright[LMF_TITLE_MAX_LEN-1] = '\0';
			g_free(pStr);
		}
	}
	else if (strcmp(tag, GST_TAG_ALBUM) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			strncpy(m_source_album, pStr, LMF_TITLE_MAX_LEN-1);
			m_source_album[LMF_TITLE_MAX_LEN-1] = '\0';
			g_free(pStr);
		}
	}
	else if (strcmp(tag, GST_TAG_GENRE) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			strncpy(m_source_genre, pStr, LMF_TITLE_MAX_LEN-1);
			m_source_genre[LMF_TITLE_MAX_LEN-1] = '\0';
			g_free(pStr);
		}
	}
	else if (strcmp(tag, GST_TAG_DATE) == 0)
	{
		GDate *pDate = NULL;
#if 0 // TODO:: check...
		if (gst_tag_list_get_date(tag_list, GST_TAG_DATE, &pDate))
		{
			m_source_date.year	= g_date_get_year(pDate);
			m_source_date.month	= g_date_get_month(pDate);
			m_source_date.day	= g_date_get_day(pDate);
			g_date_free(pDate);
		}

		m_source_date.hour		= 0;
		m_source_date.minute	= 0;
		m_source_date.second	= 0;
#endif        
	}
	else 
#endif     
    if (strcmp(tag, GST_TAG_CONTAINER_FORMAT) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			guint8 idx = 0;
			gboolean bConvertInfo = false;
#if 0 //TODO...
			while(_containerTable[idx].pStr != NULL)
			{
				if (strstr(pStr, _containerTable[idx].pStr) != NULL)
				{
					bConvertInfo = TRUE;
					MEDIA_FORMAT_T	prevFormat = m_source_format;
					m_source_format = _containerTable[idx].format;
					if(prevFormat == MEDIA_FORMAT_RM && m_source_format == MEDIA_FORMAT_RA)
						m_source_format = prevFormat;
					break;
				}
				idx++;
			}
#endif
			if(bConvertInfo == false)
				std::cout <<"[BUS] Unknown Container Format:" << "tag:" << tag << "format:" << pStr;

			g_free(pStr);
		}
	}
	else if (strcmp(tag, GST_TAG_CODEC) == 0)
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			std::cout <<"************** [BUS]" << tag <<"("<< gst_tag_get_nick(tag) << ")"<< pStr;
			g_free(pStr);
		}
	}
	else if ((strcmp(tag, GST_TAG_AUDIO_CODEC) == 0) && ((!(self->m_source_codec & MEDIA_AUDIO_MASK))|| self->m_bPlaybackStarted==true))
	{
		if (gst_tag_list_get_string(tag_list, tag, &pStr))
		{
			guint8 idx = 0;
			gboolean bConvertInfo = false;

			std::cout <<"************** [BUS]" << tag << "(" << gst_tag_get_nick(tag) << ")" << pStr;
			LMF_TO_LOWER(pStr);
#if 0 //TODO...
			while(_aCodecTable[idx].pStr != NULL)
			{
				if (strstr(pStr, _aCodecTable[idx].pStr) != NULL)
				{
					bConvertInfo = TRUE;
					m_source_codec |= _aCodecTable[idx].codec;
					break;
				}
				idx++;
			}

			if(bConvertInfo == FALSE)
			{
				LMF_DBG_PRINT("[BUS][%s][%s] Unknown Audio Codec(%s)\n", __FUNCTION__, tag, pStr);
				m_source_codec |= MEDIA_AUDIO_NOT_SUPPORTED;
				// AUDIO_CODEC Tag가 올라오면 UI에 noti 보내주자.
				// 1. DLNA등에서 audio TAG 만 늦게 올라오는 case. 나옴. (보통은 paused state에 올라옴)
				// play state 이후에 TAG 올라오는 case
				if(m_gstPipelineState == PlayingState && m_bSendNotSupportedAudioMessageAtPlayState == FALSE)
				{
					LMF_DBG_PRINT("[BUS][%s][%s] after play state : sending Unknown Audio Codec message.\n", __FUNCTION__, tag);
					m_bPendingNotSupportedAudioMessage = FALSE;
					m_bSendNotSupportedAudioMessageAtPlayState = TRUE;
					API_LMF_EVENT_Notify(pPlayerHandle->ch, MEDIA_CB_MSG_NOT_SUPPORTED_AUDIO_CODEC);
				}
				else
				{
					// play state 전이면 pending 하고 play state change 시에 보내자.
					LMF_DBG_PRINT("[BUS][%s][%s] before play state : pending Unknown Audio Codec message.\n", __FUNCTION__, tag);
					m_bPendingNotSupportedAudioMessage = TRUE;
				}
			}
			else
			{
				// AUDIO_CODEC Tag가 올라오면 UI에 noti 보내주자.
				// 1. DLNA등에서 audio TAG 만 늦게 올라오는 case. 대응.
				// play state에서 뒤늦게 audio TAG가 올라오면 UI에 noti.
				if(m_gstPipelineState == PlayingState)
				{
					LMF_DBG_PRINT("[BUS][%s] video TAG updated! noti to UI.\n", __FUNCTION__);
					//TODO API_LMF_EVENT_Notify(pPlayerHandle->ch, MEDIA_CB_MSG_SOURCE_INFO_UPDATED);
				}
			}
#endif
			g_free(pStr);
		}
	}
	else if ((strcmp(tag, GST_TAG_VIDEO_CODEC) == 0) && (!(self->m_source_codec & MEDIA_VIDEO_MASK)))
	{
		gchar *pVideoCodecInfo = NULL;
		gchar *pTempInfo	= NULL;
		gchar *pVerInfo 	= NULL;

		if (gst_tag_list_get_string (tag_list, tag, &pStr))
		{
			//LMF_DBG_PRINT("************** [BUS]%s (%s): %s \n", tag, gst_tag_get_nick(tag), pStr);
			std::cout <<"************** [BUS]" << tag << "(" << gst_tag_get_nick(tag) << ")" << pStr;

			if (gst_tag_list_get_string (tag_list, GST_TAG_VIDEO_CODEC, &pVideoCodecInfo))
			{
				guint8 idx = 0;
				gboolean bConvertInfo = false;

				LMF_TO_LOWER(pVideoCodecInfo);
#if 0 //TODO...
				while(_vCodecTable[idx].pStr != NULL)
				{
					if ((pTempInfo=strstr(pVideoCodecInfo, _vCodecTable[idx].pStr)) != NULL)
					{
						bConvertInfo = TRUE;
						if(_vCodecTable[idx].codec == MEDIA_VIDEO_MPEG4)
						{
							if ((strstr(pTempInfo, ".2")!=NULL) ||
								(strstr(pTempInfo, "4.1")!=NULL) || // M13 에서 MPEG4 4.1 지원 불가.
								(strstr(pTempInfo, "1.1")!=NULL) ||	// M13 의 경우 MPEG4 1.1 지원 불가.
								(((pVerInfo = strstr(pTempInfo, "version"))!=NULL) && (strstr(pVerInfo, "2")!=NULL)))
							{
								LMF_DBG_PRINT("\n [BUS]Not support video codec : [%s]\n\n", pVideoCodecInfo);
								LMF_DBG_PRINT("\n [BUS]sub value %s\n\n", pTempInfo);

								if (pVerInfo != NULL)
									LMF_DBG_PRINT("\n [BUS]ver value %s\n\n",pVerInfo);
								else
									LMF_DBG_PRINT("\n [BUS]ver value is NULL\n\n");

								m_source_codec |= MEDIA_VIDEO_NOT_SUPPORTED;
								bConvertInfo = FALSE;
							}
							else
							{
								m_source_codec |= MEDIA_VIDEO_MPEG4;
							}
						}
						else if(_vCodecTable[idx].codec == MEDIA_VIDEO_WMV)
						{
							if ((strstr(pTempInfo, "7") != NULL) ||
								(strstr(pTempInfo, "8") != NULL))
							{
								m_source_codec |= MEDIA_VIDEO_NOT_SUPPORTED;
								bConvertInfo = FALSE;
							}
							else
								m_source_codec |= MEDIA_VIDEO_WMV;
						}
						else
						{
							m_source_codec |= _vCodecTable[idx].codec;
						}

						break;
					}

					idx++;
				}
#endif
				if (bConvertInfo == FALSE)
				{
					std::cout << "[BUS] Unknown Video Codec" << "tag:" << tag << "codec:" << pStr;
					// TODO m_source_codec |= MEDIA_VIDEO_NOT_SUPPORTED;
					// AUDIO_CODEC Tag가 올라오면 UI에 noti 보내주자.
					// 1. avi 등에서 video TAG 만 늦게 올라오는 case. 나옴. (보통은 paused state에 올라옴)
					// play state 이후에 TAG 올라오는 case
					if(self->m_gstPipelineState == PlayingState && self->m_bSendNotSupportedVideoMessageAtPlayState == FALSE)
					{
						std::cout << "[BUS]after play state : sending Unknown Video Codec message:" << tag;
						self->m_bPendingNotSupportedVideoMessage = false;
						//pPlayerHandle->bSendNotSupportedVideoMessageAtPlayState = TRUE;

						//TODO _LMF_PLYR_BUS_ProcessNotSupportVideoCodec(pPlayerHandle);

					}
					else
					{
						// play state 전이면 pending 하고 play state change 시에 보내자.
						std::cout <<"[BUS]before play state : pending Unknown Video Codec message:" <<tag;
						self->m_bPendingNotSupportedVideoMessage = true;
					}
				}
				else
				{
					// VIDEO_CODEC Tag가 올라오면 UI에 noti 보내주자.
					// 1. DLNA등에서 video TAG 만 늦게 올라오는 case. 대응.
					// play state에서 뒤늦게 video TAG가 올라오면 UI에 noti.
					if(self->m_gstPipelineState == PlayingState)
					{
						std::cout <<"[BUS] video TAG updated! noti to UI.";
						//TODO API_LMF_EVENT_Notify(pPlayerHandle->ch, MEDIA_CB_MSG_SOURCE_INFO_UPDATED);
					}
				}

				if (pVideoCodecInfo != NULL)
					g_free(pVideoCodecInfo);
			}

			g_free(pStr);
		}
	}
	else if ((strcmp(tag, GST_TAG_BITRATE) == 0) || (strcmp(tag, GST_TAG_MAXIMUM_BITRATE) == 0))
	{
		guint bitrate = 0;

		if (gst_tag_list_get_uint(tag_list, tag, &bitrate))
		{
			if (self->m_MaxBitrate == 0)
			{
				self->m_MaxBitrate = bitrate;
				std::cout <<"[BUS]tag bitrate:" << bitrate <<  "bits/s";
			}
			else if (self->m_MaxBitrate < bitrate)
			{
				self->m_MaxBitrate = bitrate;
				std::cout << "[BUS]tag bitrate:"<< bitrate <<" bits/s updated ";
			}
			else
			{
				std::cout << "[BUS]tag bitrate: %u bits/s ignored (MaxBitrate = %u bits/s)\n", bitrate, self->m_MaxBitrate;
			}
		}
	}
	else
	{
		std::cout << "[BUS]" << tag << gst_tag_get_nick(tag);
	}

}

void GenericPipeline::handleBusTag(gpointer data, GstMessage *pMessage)
{
    GenericPipeline *self = reinterpret_cast<GenericPipeline*>(data);

	GstTagList *pTagList = NULL;

	std::cout << "+ TAG +";
	gst_message_parse_tag(pMessage, &pTagList);

	if (pTagList != NULL)
	{
		gst_tag_list_foreach(pTagList, collectTags, self);
		gst_tag_list_free(pTagList);
	}
	checkSupported(self);
}





gboolean GenericPipeline::gstBusCallbackHandle(GstBus *pBus, GstMessage *pMessage, gpointer data){
    GenericPipeline *self = reinterpret_cast<GenericPipeline*>(data);

	//LMF_PERI_PRINT("[BUS] >>> %s has sent Message: %s\n", GST_OBJECT_NAME(GST_MESSAGE_SRC(pMessage)), gst_message_type_get_name(GST_MESSAGE_TYPE(pMessage)));
    std::cout << "genericpipeline] bus callback msg - " << GST_OBJECT_NAME(GST_MESSAGE_SRC(pMessage)) << "name-" << gst_message_type_get_name(GST_MESSAGE_TYPE(pMessage)) << endl;

    if(self->m_pipeHandle == NULL)//(GST_IS_ELEMENT(pPlayerHandle->player) == false))
    {
        std::cout << "[BUS]Error. Player Handle is NULL!!! " << endl;
        return false;
    }

	if (GST_MESSAGE_TYPE(pMessage) == GST_MESSAGE_TAG)
	{
		self->handleBusTag(self, pMessage);
	}
	else if (GST_MESSAGE_TYPE(pMessage) == GST_MESSAGE_DURATION)
	{
		//_LMF_PLYR_BUS_HandleBusDuration(pPlayerHandle, pMessage);
	}
	else if (GST_MESSAGE_TYPE(pMessage) == GST_MESSAGE_APPLICATION)
	{
		//_LMF_PLYR_BUS_HandleBusApplication(pPlayerHandle, pMessage);
	}

	if (GST_MESSAGE_SRC(pMessage) == GST_OBJECT_CAST(self->m_pipeHandle))
	{
		//if(_LMF_PLYR_BUS_HandleBusPlayerMessage(pPlayerHandle, pMessage) == false)
		//	return false;	// false means no more bus msg
	}
	else
	{
		//if(_LMF_PLYR_BUS_HandleBusElementMessage(pPlayerHandle, pMessage) == false)
		//	return false;	// false means no more bus msg
	}

	return true;
}

bool GenericPipeline::connectGstBusCallback()
{
    if(m_pipeHandle == NULL)
    {
        std::cout << " Error. Gstreamer Player Handle is NULL!!!" << endl;
        return false;
    }

	m_busHandler = gst_pipeline_get_bus(GST_PIPELINE(m_pipeHandle));
    //m_tag = gst_bus_add_watch_full(m_busHandler, 0, gstBusCallbackHandle, this, NULL);
	gst_bus_add_signal_watch(m_busHandler);
	m_sigBusAsync = g_signal_connect(m_busHandler, "message", G_CALLBACK(gstBusCallbackHandle), this);

    return true;
}

bool GenericPipeline::setGstreamerDebugLevel(guint select, gchar *category, GstDebugLevel level)
{
    GError *err;
    if (!gst_init_check(NULL, NULL, &err))
    {
        std::cout <<"Error:" << err->message << endl;
        g_error_free (err);
        return false;
    }
    if (select == 1)
    {
        if(category != NULL)
            gst_debug_set_threshold_for_name((const char*)category, level);
    }
    else
    {
        gst_debug_set_default_threshold (level);
    }
    return true;
}



