/*
 * genericpipeline.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef GENERICPIPELINE_HPP_
#define GENERICPIPELINE_HPP_

#include <pipeline/pipeline.hpp>

namespace mediapipeline {

class GenericPipeline: public Pipeline {
	friend class GenericPlayer;

private:
	GenericPipeline();

public:
	~GenericPipeline();

	void load();
	void unload();
	void play(int rate);
};
}

#endif /* GENERICPIPELINE_HPP_ */
