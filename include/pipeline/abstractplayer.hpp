/*
 * abstractplayer.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef ABSTRACTPLAYER_HPP_
#define ABSTRACTPLAYER_HPP_

#include <vector>
#include <pipeline/player.hpp>
#include <pipeline/event/playeventlistener.hpp>

namespace mediapipeline {

class AbstractPlayer: public Player {
private:
	std::vector<PlayEventListener::bsp_t> _listeners;
	Pipeline::bsp_t _pipeline;

protected:
	AbstractPlayer();
	~AbstractPlayer();

	void setPipeline(Pipeline::bsp_t pipeline);

public:
	void addEventListener(PlayEventListener::bsp_t listener);

	Pipeline::bsp_t getPipeline();

	void load();
	void unload();
	void play(int rate);

public: // for derived players
	virtual void loadSpi() = 0;
	virtual void unloadSpi() = 0;
	virtual void playSpi(int rate) = 0;
};
}

#endif /* ABSTRACTPLAYER_HPP_ */
