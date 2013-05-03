/*
 * genericplayer.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef GENERICPLAYER_HPP_
#define GENERICPLAYER_HPP_

#include <pipeline/abstractplayer.hpp>

namespace mediapipeline
{

class GenericPlayer:public AbstractPlayer
{

  friend class PlayerFactory;

private:
  GenericPlayer ();

public:
  ~GenericPlayer ();

  gboolean playSpi (int rate);
  //void loadSpi();
  gboolean loadSpi (const std::string optionString);
  gboolean unloadSpi ();
  gboolean pauseSpi ();
  //gboolean seekSpi (guint64 ms); // playbin using common seek method.

  gboolean setPlaybackRateSpi (gfloat rate);
  gboolean isReadyToPlay ();
  Pipeline::State getPendingPipelineState ();

  gint64 durationSpi () const;
  gint64 positionSpi () const;
  gint volumeSpi () const;
  gboolean isMutedSpi () const;
  gboolean isAudioAvailableSpi () const;
  gboolean isVideoAvailableSpi () const;
  gboolean isSeekableSpi () const;
  gfloat playbackRateSpi () const;

  //Error error() const = 0;
  GString errorStringSpi () const;



};

}
#endif                          /* GENERICPLAYER_HPP_ */
