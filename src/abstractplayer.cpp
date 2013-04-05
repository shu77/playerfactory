/*
 * abstractplayer.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#include <glibmm-utils/glibmm-utils.h>

#include <pipeline/abstractplayer.hpp>
#include <pipeline/event/playeventlistener.hpp>

using namespace mediapipeline;

AbstractPlayer::AbstractPlayer() {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
}
AbstractPlayer::~AbstractPlayer() {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->_listeners.clear();
}
void AbstractPlayer::addEventListener(PlayEventListener::bsp_t listener) {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->_listeners.push_back(listener);
}
void AbstractPlayer::setPipeline(Pipeline::bsp_t pipeline) {
	this->_pipeline = pipeline;
}

Pipeline::bsp_t AbstractPlayer::getPipeline() {
	return this->_pipeline;
}

void AbstractPlayer::play(int rate) {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->playSpi(rate);
}

void AbstractPlayer::load() {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->loadSpi();
}

void AbstractPlayer::unload() {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->unloadSpi();
}
