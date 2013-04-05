/*
 * player.hpp
 *
 *  Created on: 2013. 4. 2.
 *      Author: jeongseok.kim
 */

#ifndef PLAYER_HPP_
#define PLAYER_HPP_

#include <boost/shared_ptr.hpp>

#include <pipeline/pipeline.hpp>
#include <pipeline/event/playeventlistener.hpp>

namespace mediapipeline {
class Player {

protected:
	Player() {};


public:
	typedef boost::shared_ptr<Player> bsp_t;

	virtual ~Player() {} ;

	virtual void addEventListener(PlayEventListener::bsp_t listener) = 0;

	virtual void load() = 0;
	virtual void unload() = 0;
	virtual void play(int rate = 1) = 0;
	/*
	virtual void stop(Pipeline::bsp_t pipeline) = 0;
	virtual void seek(Pipeline::bsp_t pipeline) = 0;
	virtual void setProperty(Pipeline::bsp_t pipeline) = 0;
	virtual void getProperty(Pipeline::bsp_t pipeline) = 0;
	*/
};
}

#endif /* PLAYER_HPP_ */
