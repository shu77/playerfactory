/*
 * abstractplayer.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */
#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif
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
  Pipeline::State oldState = getPlayerState();

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

  //m_playertState = newState;
  setPlayerState(newState);
  if (getPlayerState() != oldState) {
    cout << "[AbstractPlayer::play] player state changed (to :" <<
         getPlayerState() << ")" << endl;
    updateState (getPlayerState());
  }
  return result;
}

gboolean AbstractPlayer::pause ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::State newState = Pipeline::PausedState;
  Pipeline::State oldState = getPlayerState();
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

  //m_playertState = newState;
  setPlayerState(newState);
  if (getPlayerState() != oldState) {
    cout << "[AbstractPlayer::play] player state changed (to :" <<
         getPlayerState() << ")" << endl;
    updateState (getPlayerState());
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
  if (getPlayerState() != Pipeline::StoppedState) {
    //m_playertState = Pipeline::StoppedState;
    setPlayerState(Pipeline::StoppedState);
    return this->unloadSpi ();
  }
  return true;
}

gboolean AbstractPlayer::seek (guint64 posMsec)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  return this->seekSpi(posMsec);
}

gboolean AbstractPlayer::seekSpiCommon (guint64 posMsec)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  gboolean retVal = false;
  Pipeline::bsp_t pipeline = getPipeline ();

  g_print("[%s] check SEEKABLE \n ", __FUNCTION__);
  if ((pipeline->m_bPlaybackStarted == TRUE) && pipeline->isSeekable())
  {
    g_print("[%s] seekable ok -> try to seek \n ", __FUNCTION__);

    retVal = pipeline->seek(posMsec);
    if(retVal == true)
    {
      g_print("[%s] - seek DONE. \n ", __FUNCTION__);
      //pipeline->pendingSeekPosition = -1;
      retVal = true;
    }
    else
    {
      g_print("[%s] - seek not ready or failed. \n ", __FUNCTION__);
      //pipeline->pendingSeekPosition = -1;
    }
#if 0 // deleted with 재구조화.
    else if(retVal == LMF_NOT_READY)
    {
      g_print("[%s] - seek not ready. \n ", __FUNCTION__);
      pipeline->pendingSeekPosition = -1;
    }
    else
    {
      g_print("[%s] - seek faild -> pending seek position\n", __FUNCTION__);
      pipeline->pendingSeekPosition = posMsec;
    }
#endif
  }
  else
  {
    g_print("[%s] not SEEKABLE or play not started yet. \n", __FUNCTION__);
#if 0
    if(pipeline->m_bPlaybackStarted != TRUE)
    {
      g_print("[%s] seek command. before play start. \n", __FUNCTION__);
      g_print("[%s] pendingSeekPosition = %d \n", __FUNCTION__, posMs);
      pipeline->pendingSeekPosition = posMsec;
    }
#endif
  }
  return retVal;  // meaningful 한 return 값을 전달하도록 수정.
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
  Pipeline::State saveState = getPlayerState();

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

Pipeline::State AbstractPlayer::getPlayerState(){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->getPlayerState();
  }
}

void AbstractPlayer::setPlayerState(Pipeline::State state){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    pipeline->setPlayerState(state);
  }
}

/* multi audio track, multi video angle */
gboolean AbstractPlayer::getAudioLanguagesList(gchar **ppLangList, gint *pLangListSize, gint *pTotalLangNum){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->getAudioLanguagesList(ppLangList, pLangListSize, pTotalLangNum);
  }
}
gboolean AbstractPlayer::setAudioLanguage(char *pAudioLang){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->setAudioLanguage(pAudioLang);
  }
}
gboolean AbstractPlayer::setAudioTrack(gint AudioTrackNum){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->setAudioTrack(AudioTrackNum);
  }
}
gboolean AbstractPlayer::getCurAudioLanguage(char **ppAudioLang){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->getCurAudioLanguage(ppAudioLang);
  }
}
gboolean AbstractPlayer::getCurAudioTrack(gint *pCurAudioTrackNum){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->getCurAudioTrack(pCurAudioTrackNum);
  }
}
gboolean AbstractPlayer::getTotalVideoAngle(gint *pTotalVideoAngleNum){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->getTotalVideoAngle(pTotalVideoAngleNum);
  }
}
gboolean AbstractPlayer::setVideoAngle(gint VideoAngleNum){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->setVideoAngle(VideoAngleNum);
  }
}
gboolean AbstractPlayer::getCurrentVideoAngle(gint *pCurrentVideoAngleNum){
  LOG_FUNCTION_SCOPE_NORMAL_D ("AbstractPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  if (pipeline) {
    return pipeline->getCurrentVideoAngle(pCurrentVideoAngleNum);
  }
}

