/*
 * dlnaplayer.cpp
 *
 *  Created on: 2013. 5. 7.
 *      Author: mf-dev part
 */

#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif
#include <pipeline/generic/genericplayer.hpp>
#include <pipeline/generic/genericpipeline.hpp>
#include <pipeline/generic/dlnaplayer.hpp>
#include <pipeline/generic/dlnapipeline.hpp>

using namespace mediapipeline;

DlnaPlayer::DlnaPlayer ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("DlnaPlayer");

}

DlnaPlayer::~DlnaPlayer ()
{
  
  LOG_FUNCTION_SCOPE_NORMAL_D ("DlnaPlayer");
}

gboolean DlnaPlayer::loadSpi (const std::string optionString)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("DlnaPlayer");

  Pipeline::bsp_t pipeline = getPipeline ();
  gboolean result = false;
  g_print("> create dlna pipeline \r\n");
  pipeline.reset(new DlnaPipeline()); //moved to create time..
  setPipeline(pipeline);
  pipeline = getPipeline ();
  
  result = pipeline->load (optionString);
  if(result == true)
      result = pipeline->informationMonitorStart(200);
  
  return result;
}

