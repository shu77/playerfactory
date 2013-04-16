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
//#include <gst/app/gstappbuffer.h>
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
    }
	else
	{
        m_pipeHandle = gst_element_factory_make("playbin", NULL);
        m_playbinVersion = 1; //playbin

	}

    // connect source notify. 
    g_signal_connect(G_OBJECT(m_pipeHandle), "notify::source", G_CALLBACK(playbinNotifySource), this);

    // connect volume notify.
    double volume = 1.0;
    g_object_get(G_OBJECT(m_pipeHandle), "volume", &volume, NULL);
    m_volume = int(volume*100);

    g_signal_connect(G_OBJECT(m_pipeHandle), "notify::volume", G_CALLBACK(handleVolumeChange), this);
    if (m_playbinVersion == 2)
        g_signal_connect(G_OBJECT(m_pipeHandle), "notify::mute", G_CALLBACK(handleMutedChange), this);	
}

GenericPipeline::~GenericPipeline() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

void GenericPipeline::load(MEDIA_CLIPOPT_T *clipOpt){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
    
}


void GenericPipeline::load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType) {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
	//this->loadSpi(clipOpt);
	// not supported API.
}


void GenericPipeline::unload() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

void GenericPipeline::play(int rate) {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}


gint64 GenericPipeline::duration() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gint64 GenericPipeline::position() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gint GenericPipeline::volume() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isMuted() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isAudioAvailable() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isVideoAvailable() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gboolean GenericPipeline::isSeekable() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}
gfloat GenericPipeline::playbackRate() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}

//Error GenericPipeline::error() const;
GString GenericPipeline::errorString() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");

}

void GenericPipeline::playbinNotifySource(GObject *pObject, GParamSpec *pParam, gpointer u_data)
{
    GenericPipeline *genericPipeline = reinterpret_cast<GenericPipeline *>(u_data);

    if (g_object_class_find_property(G_OBJECT_GET_CLASS(pObject), "source"))
    {
        GObject             *pSrcElement    = NULL;
        GstBaseSrc          *pBaseSrc       = NULL;
        GstElementFactory   *pFactory       = NULL;
        const gchar         *pName          = NULL;

        // get source element
        g_object_get(pObject, "source", &pSrcElement, NULL);

        //check soup http src
        pFactory = gst_element_get_factory((GstElement *)pSrcElement);
        pName = gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(pFactory));

        if (!strcmp(pName, "souphttpsrc")) // do only http:
        {
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
void GenericPipeline::handleVolumeChange(GObject *pObject, GParamSpec *pParam, gpointer u_data)
{


}
void GenericPipeline::handleMutedChange(GObject *pObject, GParamSpec *pParam, gpointer u_data)
{


}
