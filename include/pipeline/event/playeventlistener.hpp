/*
 * playeventlistener.hpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#ifndef PLAYEVENTLISTENER_HPP_
#define PLAYEVENTLISTENER_HPP_

#include <boost/shared_ptr.hpp>

namespace mediapipeline {

class PlayEventListener {
public :
	typedef boost::shared_ptr<PlayEventListener> bsp_t;

	PlayEventListener() {};
	~PlayEventListener() {};

};

}

#endif /* PLAYEVENTLISTENER_HPP_ */
