/*
 * abstractplayer.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef ABSTRACTPLAYER_HPP_
#define ABSTRACTPLAYER_HPP_

#include <vector>
#include <pipeline/player.hpp>
#include <pipeline/event/playeventlistener.hpp>

namespace mediapipeline
{

class AbstractPlayer:public Player
{
private:
  std::vector < PlayEventListener::bsp_t > _listeners;
  Pipeline::bsp_t _pipeline;

protected:
  AbstractPlayer ();
  ~AbstractPlayer ();

  void setPipeline (Pipeline::bsp_t pipeline);

public:
  void addEventListener (PlayEventListener::bsp_t listener);

  Pipeline::bsp_t getPipeline ();

  gboolean load (const std::string optionString);
  gboolean unload ();
  gboolean play (int rate);
  gboolean pause ();
  gboolean setPlaybackRate (gfloat rate);
  gboolean seek (guint64 posMsec);
  gboolean seekSpiCommon (guint64 posMsec);
  virtual gboolean seekSpi (guint64 posMsec)
  {
    return seekSpiCommon(posMsec);
  }

  gint64 duration () const;
  gint64 position () const;
  gint volume () const;
  gboolean isMuted () const;
  gboolean isAudioAvailable () const;
  gboolean isVideoAvailable () const;
  gboolean isSeekable () const;
  gfloat playbackRate () const;

  GString errorString () const;

  //Pipeline::State m_playertState;   /* user input control status */ //move down pipeline layer.
  gboolean m_bFeedPossible;
  void updateState (Pipeline::State newState);
  Pipeline::State getPlayerState();
  void setPlayerState(Pipeline::State state);
    
  bool setGstreamerDebugLevel (guint select, gchar * category,
                               GstDebugLevel level);

public:                      // for derived players
  //virtual void loadSpi() = 0;
  virtual gboolean loadSpi (const std::string optionString) = 0;
  virtual gboolean unloadSpi () = 0;
  virtual gboolean playSpi (int rate) = 0;
  virtual gboolean pauseSpi () = 0;
  virtual gboolean setPlaybackRateSpi (gfloat rate) = 0;
  virtual gboolean isReadyToPlay () = 0;
  virtual Pipeline::State getPendingPipelineState () = 0;

// multi
  gboolean getAudioLanguagesList(gchar **ppLangList, gint *pLangListSize, gint *pTotalLangNum);
  gboolean setAudioLanguage(char *pAudioLang);
  gboolean setAudioTrack(gint AudioTrackNum);
  gboolean getCurAudioLanguage(char **ppAudioLang);
  gboolean getCurAudioTrack(gint *pCurAudioTrackNum);
  gboolean getTotalVideoAngle(gint *pTotalVideoAngleNum);
  gboolean setVideoAngle(gint VideoAngleNum);
  gboolean getCurrentVideoAngle(gint *pCurrentVideoAngleNum);

  virtual gint64 durationSpi () const = 0;
  virtual gint64 positionSpi () const = 0;
  virtual gint volumeSpi () const = 0;
  virtual gboolean isMutedSpi () const = 0;
  virtual gboolean isAudioAvailableSpi () const = 0;
  virtual gboolean isVideoAvailableSpi () const = 0;
  virtual gboolean isSeekableSpi () const = 0;
  virtual gfloat playbackRateSpi () const = 0;

  //Error error() const = 0;
  virtual GString errorStringSpi () const = 0;

};
}

#endif                          /* ABSTRACTPLAYER_HPP_ */
