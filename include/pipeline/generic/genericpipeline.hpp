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

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MEDIAPIPE_BUFFER_SIZE (24*1024*1024)
#define LMF_TO_LOWER(src)						\
	do{										\
		 		for(int i = 0; i < strlen(src); i++) 	\
					src[i] = tolower(src[i]);	\
		 }while(FALSE)
#define LMF_TITLE_MAX_LEN 512

namespace mediapipeline {

class GenericPipeline: public Pipeline {
	friend class GenericPlayer;

private:
	GenericPipeline();
    static void playbinNotifySource(GObject *o, GParamSpec *p, gpointer d);
    static void handleVolumeChange(GObject *o, GParamSpec *p, gpointer d);
    static void handleMutedChange(GObject *o, GParamSpec *p, gpointer d);
    static gboolean gstBusCallbackHandle(GstBus *pBus, GstMessage *pMessage, gpointer data);
	
public:
	~GenericPipeline();

	//void load();
	bool load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType);
	bool load(MEDIA_CLIPOPT_T *clipOpt);
	void unload();
	bool play(int rate);
	bool pause();
	void stop();
    bool seek(gint64 ms);
    bool loadFromURI();
    bool handleURI(MEDIA_CLIPOPT_T *clipOpt);

    gint64 duration() const;
    gint64 position() const;
    gint volume() const;
	gboolean isMuted() const;
	gboolean isAudioAvailable() const;
	gboolean isVideoAvailable() const;
	gboolean isSeekable() const;
    void setSeekable(bool seekable);
	gfloat playbackRate() const;
    void setPlaybackRate(gfloat rate);

	//Error error() const;
	GString errorString() const;

    bool compareDouble(const double num1, const double num2);

    bool connectGstBusCallback();
    bool setGstreamerDebugLevel(guint select, gchar *category, GstDebugLevel level);
    void checkSupported (gpointer data);
    bool setExtraElementOption(MEDIA_CLIPOPT_T *clipOpt);
    static void collectTags (const GstTagList *tag_list, const gchar *tag, gpointer user_data);
    void handleBusTag(gpointer data, GstMessage *pMessage);

    //gchar            m_source_title[512];         /**< Title of source */
    //gchar            m_source_artist[512];        /**< artist of source */
    //gchar            m_source_copyright[512];     /**< copyright of source */
    //gchar            m_source_album[512];         /**< album of source */
    //gchar            m_source_genre[512];         /**< genre of source */
    //guint16          m_source_titleSize;          /**< Size of title string */
    //TIME_T           m_source_date;               /**< Creation date of source */
    guint64          m_source_dataSize;           /**< Total length of source */
    MEDIA_FORMAT_T   m_source_format;             /**< Media format (container type) of source */
    MEDIA_CODEC_T    m_source_codec;              /**< Media Codec of source */
    guint16          m_source_width;              /**< Width of source. (Not valid when audio ony) */
    guint16          m_source_height;             /**< Height of source. (Not valid when audio ony) */
    gint32           m_source_durationMS;         /**< Total duration in millisecond */
    gint32           m_source_targetBitrateBps;   /**< Needed average bitrate in bps (bits per second) */
    gboolean         m_source_bIsValidDuration;   /**< durationMS가 유효한 값인지. (FALSE면 duration이 없는 경우 ex)live ) */
    gboolean         m_source_bIsSeekable;        /**< HOA_MEDIA_SeekClip is available(TRUE) or not*/
    gboolean         m_source_bIsScannable;       /**< HOA_MEDIA_SetPlaySpeed is available(TRUE) or not */


};
}

#endif /* GENERICPIPELINE_HPP_ */
