/*
 * playerfactory.cpp
 *
 *  Created on: 2013. 4. 3.
 *      Author: jeongseok.kim
 */

#include <glibmm-utils/glibmm-utils.h>
#include <pipeline/playerfactory.hpp>
#include <pipeline/generic/genericplayer.hpp>

using namespace std;
using namespace mediapipeline;

boost::weak_ptr<PlayerFactory> PlayerFactory::_internal_ptr;

PlayerFactory::PlayerFactory() {
	LOG_FUNCTION_SCOPE_NORMAL_D("PlayerFactory");
	LOG_D("created", "PlayerFactory");
}

PlayerFactory::~PlayerFactory() {
	LOG_FUNCTION_SCOPE_NORMAL_D("PlayerFactory");
	LOG_D("removed", "PlayerFactory");
}

const string PlayerFactory::getName() {
	return "mediapipeline::PlayerFactory";
}

PlayerFactory::bsp_t PlayerFactory::getFactory() {

	PlayerFactory::bsp_t instance = _internal_ptr.lock();

	LOG_FUNCTION_SCOPE_NORMAL_D("PlayerFactory");

	if (!instance) {
		LOG_D("an instance will be created.", "PlayerFactory");
		instance.reset(new PlayerFactory());
		_internal_ptr = instance;
	}
	return instance;
}

Player::bsp_t PlayerFactory::create(const std::string transport_type) {
	LOG_FUNCTION_SCOPE_NORMAL_D("PlayerFactory");
	return this->create(1);
}

Player::bsp_t PlayerFactory::create(unsigned int transport_type) {
	LOG_FUNCTION_SCOPE_NORMAL_D("PlayerFactory");
	Player::bsp_t player;

	// TODO: refine selection alghorithm
  player.reset (new GenericPlayer());

	return player;
}

