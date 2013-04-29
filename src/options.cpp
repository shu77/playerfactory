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

gboolean
Options::loadJSON (const std::string json)
{
  std::stringstream ss (json);
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  gboolean result = false;


  try
  {
    boost::property_tree::json_parser::read_json (ss, this->pt);
    
    boost::property_tree::basic_ptree<std::string,std::string>::const_iterator iter = this->pt.begin(),iterEnd = this->pt.end();
        for(;iter != iterEnd;++iter)
        {
            std::cout << iter->first << " " << iter->second.get_value<std::string>() << std::endl;
        }
        result = true;
        
    }
    catch(boost::property_tree::json_parser::json_parser_error &je)
    {
        std::cout << "Error parsing: " << je.filename() << " on line: " << je.line() << std::endl;
        std::cout << je.message() << std::endl;
        result = false;
    }
    return result;
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

bool Options::getBoolean (const std::string key)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  return this->pt.get < bool > (key);
}
