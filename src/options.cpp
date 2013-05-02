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

#ifdef USE_GLIB_MMUTILS
#include <glibmm-utils/glibmm-utils.h>
#else
#define LOG_FUNCTION_SCOPE_NORMAL_D g_print
#define LOG_D g_print
#endif

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
    for(; iter != iterEnd; ++iter)
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

bool Options::checkKeyExistance(const std::string key)
{
  // bool optional
  boost::optional<bool> v = pt.get_optional<bool>(key);
  if (v) // key exists
  {
    //bool bool_value = v.get();
    std::cout << "found key with boolean type." << std::endl;
    return true;
  }
  // string optional
  boost::optional<std::string> v2 = pt.get_optional<std::string>(key);
  if(v2)
  {
    std::cout << "found key with string type." << std::endl;
    return true;
  }
  // int optional
  boost::optional<int> v3 = pt.get_optional<int>(key);
  if(v3)
  {
    std::cout << "found key with int type." << std::endl;
    return true;
  }
  std::cout << "error. not found property value." << std::endl;
  return false;
}


bool
Options::getInt (const std::string key, int *value)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  boost::optional<int> v3 = pt.get_optional<int>(key);
  if(v3)
  {
    std::cout << "found key with int type." << std::endl;
    *value = this->pt.get < int >(key);
    return true;
  }
  std::cout << "not found key with int type." << std::endl;
  return false;
}

bool
Options::getString (const std::string key, std::string *value)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  boost::optional<std::string> v2 = pt.get_optional<std::string>(key);
  if(v2)
  {
    std::cout << "found key with string type." << std::endl;
    *value = this->pt.get < std::string > (key);
    return true;
  }
  std::cout << "not found key with string type." << std::endl;
  return false;
}

bool
Options::getBoolean (const std::string key, bool *value)
{
  LOG_FUNCTION_SCOPE_NORMAL_D ("Options");
  boost::optional<bool> v = pt.get_optional<bool>(key);
  if (v) // key exists
  {
    //bool bool_value = v.get();
    std::cout << "found key with boolean type." << std::endl;
    *value = this->pt.get < bool > (key);
    return true;
  }
  std::cout << "not found key with boolean type." << std::endl;
  return false;
}
