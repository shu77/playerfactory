/*
 * genericpipeline.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef GENERICPIPELINE_HPP_
#define GENERICPIPELINE_HPP_

#include <pipeline/pipeline.hpp>
#include <string>

using namespace std;

namespace mediapipeline
{
  class GenericPipeline:public Pipeline
  {
    friend class GenericPlayer;

  private:
    static void playbinNotifySource (GObject * o, GParamSpec * p, gpointer d);

  public:
    GenericPipeline ();
    ~GenericPipeline ();

    /* basic playback control */
    gboolean loadSpi_pre ();
    gboolean loadSpi_post ();
    gboolean unloadSpi ();
    /* for trick, seek */
    gboolean setPlaybackRateSpi_pre (gpointer data, gfloat rate);
    gboolean setServerSideTrick(gpointer data, gboolean serverSideTrick);
    gboolean seekSpi (gint64 ms);
    bool loadFromURI ();
    bool handleURI ();
    gboolean isReadyToPlaySpi ();
    gboolean informationMonitorStartSpi(guint32 timeInterval);
    gboolean positionSpi(gpointer data, gint64 *pos);
    void setInterleavingTypeSpi(gpointer data, GstObject *pObj, gint stream, gpointer user_data);
    void getUndecodedSizeSpi(gpointer data, guint64* pVdecBufferedSize, guint64* pAdecBufferedSize);
    static void correctBufferedBytesSpi(gpointer data);
    static void videoDecodeUnderrunCbSpi(GstElement *pObj, gpointer data);
    static void audioDecodeUnderrunCbSpi(GstElement *pObj, gpointer data);
    /* for information handling */
    void getStreamsInfoSpi(gpointer data);
    gboolean updateVideoInfoSpi(gpointer data);
    
    void handleStateMsgPauseSpi_pre(gpointer data);
    void registerUnderrunSignalHandlerSpi(gpointer data);
    //gboolean checkTimeToDecodeSpi(gpointer data);
    void handlePlayEndSpi(gpointer data);
    void handlePlayErrorSpi(gpointer data);


    //multi track audio, multi angle video APIs
    gboolean freeGList(GList *pList);
    GList* getLangListForType(gpointer data, gint numOfTrack, const gchar *pSignalStr);
    gboolean getAudioLanguagesListSpi(gpointer data, gchar **ppLangList, gint *pLangListSize, gint *pTotalLangNum);
    gboolean setAudioLanguageSpi(gpointer data, char *pAudioLang);
    gboolean setAudioTrackSpi(gpointer data, gint AudioTrackNum);
    gboolean getCurAudioLanguageSpi(gpointer data, char **ppAudioLang);
    gboolean getCurAudioTrackSpi(gpointer data, gint *pCurAudioTrackNum);
    gboolean getTotalVideoAngleSpi(gpointer data,  gint *pTotalVideoAngleNum);
    gboolean setVideoAngleSpi(gpointer data, gint VideoAngleNum);
    gboolean getCurrentVideoAngleSpi(gpointer data, gint *pCurrentVideoAngleNum);
    // end 
    void S_videoChanged (GstElement* element, gpointer data);
    static gboolean videoChangeTimeoutCallback(gpointer data);
    gint greatestCommonDivisor(int a, int b);
    void S_audioChanged (GstElement* element, gpointer data);
    static gboolean audioChangeTimeoutCallback(gpointer data);


    
    gboolean setExtraElementOption ();
    GString errorString () const;

  };
}

#endif                          /* GENERICPIPELINE_HPP_ */
