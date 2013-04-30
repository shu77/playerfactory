/*
 * player.hpp
 *
 *  Created on: 2013. 4. 2.
 *      Author: jeongseok.kim
 */

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <boost/shared_ptr.hpp>

//#include <gst/video/video.h>
//#include <gst/app/gstappsrc.h>
//#include <gst/app/gstappbuffer.h>
#include <glib.h>
#include <gst/gst.h>

#include <pipeline/pipeline.hpp>
#include <pipeline/event/playeventlistener.hpp>

namespace mediapipeline
{
class Player
{

protected:
  Player ()
  {
  };


public:
  typedef boost::shared_ptr < Player > bsp_t;

  virtual ~ Player ()
  {
  };

  virtual void addEventListener (PlayEventListener::bsp_t listener) = 0;
  virtual gboolean load (const std::string optionString) = 0;
  virtual gboolean unload () = 0;
  virtual gboolean pause () = 0;

  virtual gboolean play (int rate = 1) = 0;
  virtual gboolean setPlaybackRate (gfloat rate) = 0;

  /*
     virtual void stop(Pipeline::bsp_t pipeline) = 0;
     virtual void seek(Pipeline::bsp_t pipeline) = 0;
     virtual void setProperty(Pipeline::bsp_t pipeline) = 0;
     virtual void getProperty(Pipeline::bsp_t pipeline) = 0;
   */

  virtual gint64 duration () const = 0;       /* get duration */
  virtual gint64 position () const = 0;       /* get current position */
  virtual gint volume () const = 0;   /* get current volume */
  virtual gboolean isMuted () const = 0;      /* get mute info */
  virtual gboolean isAudioAvailable () const = 0;     /* get audio track avilable */
  virtual gboolean isVideoAvailable () const = 0;     /* get video track avilable */
  virtual gboolean isSeekable () const = 0;   /* get seekable status */
  virtual gfloat playbackRate () const = 0;   /* get current playback rate */

  //virtual Error error() const;
  virtual GString errorString () const = 0;
  virtual bool setGstreamerDebugLevel (guint select, gchar * category,
                                       GstDebugLevel level) = 0;

};
}

#endif /* PLAYER_HPP_ */
