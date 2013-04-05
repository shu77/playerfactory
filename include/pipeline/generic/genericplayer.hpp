/*
 * genericplayer.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef GENERICPLAYER_HPP_
#define GENERICPLAYER_HPP_

#include <pipeline/abstractplayer.hpp>

namespace mediapipeline
{

class GenericPlayer: public AbstractPlayer {

	friend class PlayerFactory;

private:
	GenericPlayer();

public :
	~GenericPlayer();

	void playSpi(int rate);
	void loadSpi();
	void unloadSpi();

};

}
#endif /* GENERICPLAYER_HPP_ */
