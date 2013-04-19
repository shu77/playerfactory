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
	Pipeline::bsp_t pipeline = getPipeline();
	pipeline.reset(new GenericPipeline());
	setPipeline(pipeline);
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
/*
void GenericPlayer::loadSpi() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");

	Pipeline::bsp_t pipeline = getPipeline();
	pipeline.reset(new GenericPipeline());
	setPipeline(pipeline);
	pipeline->load();
}
*/
void GenericPlayer::loadSpi(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");

	std::cout << "not supported API at genericPlayer. ";

	//Pipeline::bsp_t pipeline = getPipeline();
	//pipeline.reset(new GenericPipeline());
	//setPipeline(pipeline);
	//pipeline->load();
}

void GenericPlayer::loadSpi(MEDIA_CLIPOPT_T *clipOpt){
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");

	Pipeline::bsp_t pipeline = getPipeline();
//	pipeline.reset(new GenericPipeline()); //moved to create time..
//	setPipeline(pipeline);
	pipeline->load(clipOpt);
}

void GenericPlayer::unloadSpi() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	Pipeline::bsp_t pipeline = getPipeline();
	pipeline->unload();
}

gint64 GenericPlayer::durationSpi() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->duration();
}

gint64 GenericPlayer::positionSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->position();
}

gint GenericPlayer::volumeSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->volume();
}

gboolean GenericPlayer::isMutedSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->isMuted();
}

gboolean GenericPlayer::isAudioAvailableSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->isAudioAvailable();
}

gboolean GenericPlayer::isVideoAvailableSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->isVideoAvailable();
}

gboolean GenericPlayer::isSeekableSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->isSeekable();
}

gfloat GenericPlayer::playbackRateSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->playbackRate();
}


//Error error() const = 0;
GString GenericPlayer::errorStringSpi() const{
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPlayer");
	//Pipeline::bsp_t pipeline = getPipeline();
	//return pipeline->errorString();
}


