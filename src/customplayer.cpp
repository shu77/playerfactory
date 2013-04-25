
/*
 * 	customplayer.cpp
 *
 *  	Created on: 2013. 4. 19.
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

void
CustomPlayer::playSpi (int rate)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  LOG_D ("playing", "CustomPlayer");
//      Pipeline::bsp_t pipeline = getPipeline();
//      pipeline->play(rate);
}

void
CustomPlayer::loadSpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");

//      Pipeline::bsp_t pipeline = getPipeline();
//      pipeline.reset(new GenericPipeline());
//      setPipeline(pipeline);
//      pipeline->load();
}

#if 0
void
CustomPlayer::loadSpi (MEDIA_STREAMOPT_T * streamOpt,
    MEDIA_FORMAT_T mediaFormatType)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");

  std::cout << "not supported API at genericPlayer. ";

  //Pipeline::bsp_t pipeline = getPipeline();
  //pipeline.reset(new GenericPipeline());
  //setPipeline(pipeline);
  //pipeline->load();
}

void
CustomPlayer::loadSpi (MEDIA_CLIPOPT_T * clipOpt)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");

  Pipeline::bsp_t pipeline = getPipeline ();

  /* TODO : change GenericPipeline -> CustomPipeline */
  pipeline.reset (new GenericPipeline ());
  setPipeline (pipeline);
  pipeline->load (clipOpt);
}

#endif
void
CustomPlayer::unloadSpi ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
//      Pipeline::bsp_t pipeline = getPipeline();
//      pipeline->unload();
}


gint64
CustomPlayer::durationSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->duration();
}

gint64
CustomPlayer::positionSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->position();
}

gint
CustomPlayer::volumeSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->volume();
}

gboolean
CustomPlayer::isMutedSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isMuted();
}

gboolean
CustomPlayer::isAudioAvailableSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isAudioAvailable();
}

gboolean
CustomPlayer::isVideoAvailableSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isVideoAvailable();
}

gboolean
CustomPlayer::isSeekableSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->isSeekable();
}

gfloat
CustomPlayer::playbackRateSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->playbackRate();
}


//Error error() const = 0;
GString
CustomPlayer::errorStringSpi () const const
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("CustomPlayer");
  //Pipeline::bsp_t pipeline = getPipeline();
  //return pipeline->errorString();
}

//end of file 
