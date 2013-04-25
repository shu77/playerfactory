/*
 * playerfactory.hpp
 *
 *  Created on: 2013. 4. 2.
 *      Author: jeongseok.kim
 */

#ifndef PLAYERFACTORY_HPP_
#define PLAYERFACTORY_HPP_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/utility.hpp>
#include <pipeline/player.hpp>

namespace mediapipeline
{

  class PlayerFactory:private boost::noncopyable
  {

  public:
    typedef boost::shared_ptr < PlayerFactory > bsp_t;

  private:
      PlayerFactory ();

    static boost::weak_ptr < PlayerFactory > _internal_ptr;

  public:
     ~PlayerFactory ();

    static bsp_t getFactory ();
    const std::string getName ();
      Player::bsp_t create (const std::string transport_type);
      Player::bsp_t create (unsigned int transport_type);
  };

}

#endif                          /* PLAYERFACTORY_HPP_ */
