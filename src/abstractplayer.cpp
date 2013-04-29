/*
 * abstractplayer.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#include <glibmm-utils/glibmm-utils.h>

#include <pipeline/abstractplayer.hpp>
#include <pipeline/event/playeventlistener.hpp>

using namespace mediapipeline;

AbstractPlayer::AbstractPlayer ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
}

AbstractPlayer::~AbstractPlayer ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  this->_listeners.clear ();
}

void
AbstractPlayer::addEventListener (PlayEventListener::bsp_t listener)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  this->_listeners.push_back (listener);
}

void
AbstractPlayer::setPipeline (Pipeline::bsp_t pipeline)
{
  this->_pipeline = pipeline;
}

Pipeline::bsp_t AbstractPlayer::getPipeline ()
{
  return this->_pipeline;
}

gboolean AbstractPlayer::play (int rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::State newState = Pipeline::PlayingState;
  Pipeline::State oldState = m_playertState;

  gboolean
      result = false;

  m_bFeedPossible = true;
  if (this->isReadyToPlay () == true) {
    result = this->playSpi (rate);
  } else {
    cout << " It's a prerolling status. (play command pending now.)" << endl;
    result = true;
  }
  if (!result) {
    cout << "[AbstractPlayer::play] Error [stopped state]" << endl;
    newState = Pipeline::StoppedState;
  }

  m_playertState = newState;
  if (m_playertState != oldState) {
    cout << "[AbstractPlayer::play] player state changed (to :" <<
        m_playertState << ")" << endl;
    updateState (m_playertState);
  }
  return result;
}

gboolean AbstractPlayer::pause ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::State newState = Pipeline::PausedState;
  Pipeline::State oldState = m_playertState;
  gboolean
      result = false;

  if (this->getPendingPipelineState () != Pipeline::PausedState) {
    result = this->pauseSpi ();
  } else {
    // excetion for duplicated pause command.
    cout <<
        "[AbstractPlayer::play] player pending state : pause or stop , skipped pause command. "
        << endl;
    result = true;
  }

  if (!result) {
    cout << "[AbstractPlayer::play] Error [stopped state]" << endl;
    newState = Pipeline::StoppedState;
  }

  m_playertState = newState;
  if (m_playertState != oldState) {
    cout << "[AbstractPlayer::play] player state changed (to :" <<
        m_playertState << ")" << endl;
    updateState (m_playertState);
  }
  return result;
}

gboolean AbstractPlayer::load (const std::string optionString)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  if(optionString.empty())
  {
    std::cout << "ERROR!!!!! option string empty!!!!!"<<endl;
    return false;
  }

  /* json format validation check heare. */

  //TODO: add json format validation check.



  return this->loadSpi (optionString);
}

gboolean AbstractPlayer::unload ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  if (m_playertState != Pipeline::StoppedState) {
    m_playertState = Pipeline::StoppedState;
    return this->unloadSpi ();
  }
  return true;
}

gboolean AbstractPlayer::setPlaybackRate (gfloat rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  return this->setPlaybackRateSpi (rate);
}


     gint64 AbstractPlayer::duration () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return
           this->
       durationSpi ();
     }

gint64
     AbstractPlayer::position () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->positionSpi ();
     }

gint
     AbstractPlayer::volume () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->volumeSpi ();
     }

gboolean
     AbstractPlayer::isMuted () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->isMutedSpi ();
     }

gboolean
     AbstractPlayer::isAudioAvailable () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->isAudioAvailableSpi ();
     }

gboolean
     AbstractPlayer::isVideoAvailable () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->isVideoAvailableSpi ();
     }

gboolean
     AbstractPlayer::isSeekable () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->isSeekableSpi ();
     }

gfloat
     AbstractPlayer::playbackRate () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->playbackRateSpi ();
     }

//Error AbstractPlayer::error() const;
GString
     AbstractPlayer::errorString () const
     {
       LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
       return this->errorStringSpi ();
     }

     void AbstractPlayer::updateState (Pipeline::State newState)
{
  //Pipeline::MediaStatus oldStatus = m_mediaStatus;
  Pipeline::State saveState = m_playertState;

  //TODO :: notify upper layer.

}

bool AbstractPlayer::setGstreamerDebugLevel (guint select, gchar * category,
    GstDebugLevel level)
{

  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    pipeline->setGstreamerDebugLevel (select, category, level);
  }
}
