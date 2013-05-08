/*
 * genericpipeline.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 *  Modified on: 2013. 5. 5.
 *      Modifier: mf-dev
 */

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include <pthread.h>
#include <gio/gio.h>
#include <cstring>
#if (GST_VERSION_MAJOR >= 1)
#include <gst/video/video.h>
#else
//#include <gst/app/gstappbuffer.h>//playbin+appsrc not support at NC5.0
#endif
#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif
#include <pipeline/generic/genericpipeline.hpp>
#include <pipeline/generic/dlnapipeline.hpp> 

using namespace mediapipeline;

DlnaPipeline::DlnaPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");


}

DlnaPipeline::~DlnaPipeline ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("GenericPipeline");
}

