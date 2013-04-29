/*
 * options.hpp
 *
 *  Created on: 2013. 4. 25.
 *      Author: jeongseok.kim
 */

#ifndef OPTIONS_HPP_
#define OPTIONS_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <glib.h>
#include <string>


namespace mediapipeline
{
  namespace utils
  {

    class Options
    {
    public:
      typedef boost::shared_ptr < Options > bsp_t;

    private:
        boost::property_tree::ptree pt;

    public:
        Options ();
       ~Options ();

      gboolean loadJSON (const std::string);

      int getInt (const std::string);
        std::string getString (const std::string);
      bool getBoolean (const std::string);
    };
  }

}

#endif                          /* OPTIONS_HPP_ */
