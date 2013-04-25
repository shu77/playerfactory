/*
 * options.cpp

 *
 *  Created on: 2013. 4. 25.
 *      Author: jeongseok.kim
 */
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <glibmm-utils/glibmm-utils.h>

#include <pipeline/utils/options.hpp>

using namespace
    mediapipeline::utils;

Options::Options ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
}

Options::~Options ()
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
}

void
Options::loadJSON (const std::string json)
{
  std::stringstream ss (json);
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");

  boost::property_tree::json_parser::read_json (ss, this->pt);
}

int
Options::getInt (const std::string key)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  return this->pt.get < int >(key);
}

std::string Options::getString (const std::string key)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  return this->pt.get < std::string > (key);
}

bool
Options::getBoolean (const std::string key)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  return this->pt.get < bool > (key);
}
