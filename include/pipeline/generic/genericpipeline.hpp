/*
 * genericpipeline.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef GENERICPIPELINE_HPP_
#define GENERICPIPELINE_HPP_

#include <pipeline/pipeline.hpp>
#include <string>

using namespace std;

namespace mediapipeline
{

class GenericPipeline:public Pipeline
{
  friend class GenericPlayer;

private:
  GenericPipeline ();
  static void playbinNotifySource (GObject * o, GParamSpec * p, gpointer d);

public:
  ~GenericPipeline ();

  gboolean loadSpi_pre ();
  gboolean loadSpi_post ();

  bool loadFromURI ();
  bool handleURI ();
  gboolean isReadyToPlaySpi ();
  gboolean informationMonitorStartSpi(guint32 timeInterval);
  gboolean positionSpi(gpointer data, gint64 *pos);

  gboolean setExtraElementOption ();

  GString errorString () const;

  bool setGstreamerDebugLevel (guint select, gchar * category,
                               GstDebugLevel level);



};
}

#endif                          /* GENERICPIPELINE_HPP_ */
