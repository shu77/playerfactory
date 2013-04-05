/*
 * genericplayer.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#include <glibmm-utils/glibmm-utils.h>
#include <pipeline/generic/genericplayer.hpp>
#include <pipeline/generic/genericpipeline.hpp>

using namespace mediapipeline;

GenericPlayer::GenericPlayer() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
}

GenericPlayer::~GenericPlayer() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
}

void GenericPlayer::playSpi(int rate) {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	LOG_D("playing", "GenericPlayer");
	Pipeline::bsp_t pipeline = getPipeline();
	pipeline->play(rate);
}

void GenericPlayer::loadSpi() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	Pipeline::bsp_t pipeline = getPipeline();
	pipeline.reset(new GenericPipeline());
	setPipeline(pipeline);
	pipeline->load();
}

void GenericPlayer::unloadSpi() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	Pipeline::bsp_t pipeline = getPipeline();
	pipeline->unload();
}
