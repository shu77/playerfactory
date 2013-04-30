
/*
 *  customplayer.cpp
 *
 *    Created on: 2013. 4. 19.
 *
 */

#include <pipeline/custom/custom_comm_types.hpp>
#include <glibmm-utils/glibmm-utils.h>

#include <pipeline/custom/customplayer.hpp>
#include <pipeline/generic/genericpipeline.hpp>

using namespace mediapipeline;
/**
 * test for writing
 *
 *
 */
CustomPlayer::CustomPlayer ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
}

CustomPlayer::~CustomPlayer ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
}

gboolean
CustomPlayer::playSpi (int rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  LOG_D ("playing", "CustomPlayer");
//      Pipeline::bsp_t pipeline = getPipeline();
//      pipeline->play(rate);
}

gboolean CustomPlayer::pauseSpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  LOG_D ("pause", "CustomPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  return pipeline->pause ();
}

gboolean
CustomPlayer::loadSpi (const std::string optionString)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");

//      Pipeline::bsp_t pipeline = getPipeline();
//      pipeline.reset(new GenericPipeline());
//      setPipeline(pipeline);
//      pipeline->load();
}


gboolean CustomPlayer::isReadyToPlay ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  return pipeline->isReadyToPlay ();
}

Pipeline::State CustomPlayer::getPendingPipelineState ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  return pipeline->getPendingPipelineState ();
}


gboolean CustomPlayer::setPlaybackRateSpi (gfloat rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  Pipeline::bsp_t pipeline = getPipeline ();
  pipeline->setPlaybackRate (rate);
}

gboolean
CustomPlayer::unloadSpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
//      Pipeline::bsp_t pipeline = getPipeline();
//      pipeline->unload();
}


gint64 CustomPlayer::durationSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->duration();
}

gint64
CustomPlayer::positionSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->position();
}

gint
CustomPlayer::volumeSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->volume();
}

gboolean
CustomPlayer::isMutedSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isMuted();
}

gboolean
CustomPlayer::isAudioAvailableSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isAudioAvailable();
}

gboolean
CustomPlayer::isVideoAvailableSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isVideoAvailable();
}

gboolean
CustomPlayer::isSeekableSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isSeekable();
}

gfloat
CustomPlayer::playbackRateSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->playbackRate();
}


//Error error() const = 0;
GString
CustomPlayer::errorStringSpi () const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->errorString();
}

//end of file
