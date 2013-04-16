/*
 * genericpipeline.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef GENERICPIPELINE_HPP_
#define GENERICPIPELINE_HPP_

#include <pipeline/pipeline.hpp>

namespace mediapipeline {

class GenericPipeline: public Pipeline {
	friend class GenericPlayer;

private:
	GenericPipeline();
    static void playbinNotifySource(GObject *o, GParamSpec *p, gpointer d);
    static void handleVolumeChange(GObject *o, GParamSpec *p, gpointer d);
    static void handleMutedChange(GObject *o, GParamSpec *p, gpointer d);
public:
	~GenericPipeline();

	//void load();
	void load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType);
	void load(MEDIA_CLIPOPT_T *clipOpt);
	void unload();
	void play(int rate);

    gint64 duration() const;
    gint64 position() const;
    gint volume() const;
	gboolean isMuted() const;
	gboolean isAudioAvailable() const;
	gboolean isVideoAvailable() const;
	gboolean isSeekable() const;
	gfloat playbackRate() const;

	//Error error() const;
	GString errorString() const;

    
};
}

#endif /* GENERICPIPELINE_HPP_ */
