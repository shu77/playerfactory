

/*
 * customplayer.hpp
 *
 *  Created on: 2013. 4. 19.
 *
 */

#ifndef CUSTOMPLAYER_HPP_
#define CUSTOMPLAYER_HPP_

#include <pipeline/abstractplayer.hpp>

namespace mediapipeline
{

class CustomPlayer:public AbstractPlayer
{

  friend class PlayerFactory;

private:
  CustomPlayer ();

public:
  ~CustomPlayer ();

  gboolean playSpi (int rate);
  gboolean loadSpi (const std::string optionString);
  gboolean seekSpi (guint64 posMsec);

  gboolean unloadSpi ();
  gboolean pauseSpi ();
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
#endif                          /* CUSTOMPLAYER_HPP_ */

//end of file
