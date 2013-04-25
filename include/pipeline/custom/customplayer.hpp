

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

  public: ~CustomPlayer ();

    void playSpi (int rate);
    void loadSpi ();
//      void loadSpi(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType);
//      void loadSpi(MEDIA_CLIPOPT_T *clipOpt);

    void unloadSpi ();

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
