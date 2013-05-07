/*
 * genericpipeline.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef DLNAPIPELINE_HPP_
#define DLNAPIPELINE_HPP_

#include <pipeline/pipeline.hpp>
#include <pipeline/generic/genericpipeline.hpp>
#include <string>

using namespace std;

namespace mediapipeline
{
  class DlnaPipeline:public GenericPipeline
  {
    friend class DlnaPlayer;

  private:
    DlnaPipeline ();

  public:
    ~DlnaPipeline ();



  };
}

#endif                          /* DLNAPIPELINE_HPP_ */
