/*
 * dlnaplayer.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef DLNAPLAYER_HPP_
#define DLNAPLAYER_HPP_

#include <pipeline/abstractplayer.hpp>
#include <pipeline/generic/genericplayer.hpp>

namespace mediapipeline
{

class DlnaPlayer : public GenericPlayer
{

  friend class PlayerFactory;

private:
  DlnaPlayer ();

public:

  ~DlnaPlayer ();
  gboolean loadSpi (const std::string optionString); //overriding.

};

}
#endif                          /* DLNAPLAYER_HPP_ */
