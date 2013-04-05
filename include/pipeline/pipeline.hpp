/*
 * pipeline.hpp
 *
 *  Created on: 2013. 4. 2.
 *      Author: jeongseok.kim
 */

#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <boost/shared_ptr.hpp>

namespace mediapipeline {

class Pipeline {

protected:
	Pipeline();

public:
	typedef boost::shared_ptr<Pipeline> bsp_t;

	virtual ~Pipeline();

	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void play(int rate = 1) = 0;
	/*
	virtual void feed(unsigned char * data);
	virtual void stop();
	virtual void seek();
	virtual void pause();
	virtual void setProperty();
	virtual void getProperty();
	*/
};
}

#endif /* PIPELINE_HPP_ */
