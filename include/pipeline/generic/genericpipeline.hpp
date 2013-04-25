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

namespace mediapipeline {

class GenericPipeline: public Pipeline {
	friend class GenericPlayer;

private:
	GenericPipeline();
    static void playbinNotifySource(GObject *o, GParamSpec *p, gpointer d);

public:
	~GenericPipeline();

    gboolean initSpi_pre();
    gboolean initSpi_post();


	//void load();
	gboolean load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType);
	gboolean load(MEDIA_CLIPOPT_T *clipOpt);
	//void unload();
	//bool play(int rate);
	//gboolean pause();
	//void stop();
    //bool seek(gint64 ms);
    bool loadFromURI();
    bool handleURI(MEDIA_CLIPOPT_T *clipOpt);
    gboolean isReadyToPlaySpi();
    //gint64 duration() const;
    //gint64 position() const;
    //gint volume() const;
	//gboolean isMuted() const;
	//gboolean isAudioAvailable() const;
	//gboolean isVideoAvailable() const;
	//gboolean isSeekable();
    //gboolean isSeekable(gpointer data);

    bool setExtraElementOption(MEDIA_CLIPOPT_T *clipOpt);

	//Error error() const;
	GString errorString() const;

    bool setGstreamerDebugLevel(guint select, gchar *category, GstDebugLevel level);


    
};
}

#endif /* GENERICPIPELINE_HPP_ */
