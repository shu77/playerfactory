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
/*
void AbstractPlayer::load() {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->loadSpi();
}
*/
void AbstractPlayer::load(MEDIA_STREAMOPT_T *streamOpt, MEDIA_FORMAT_T mediaFormatType) {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->loadSpi(streamOpt, mediaFormatType);
}
	
void AbstractPlayer::load(MEDIA_CLIPOPT_T *clipOpt) {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->loadSpi(clipOpt);
}




void AbstractPlayer::unload() {
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	this->unloadSpi();
}

gint64 AbstractPlayer::duration() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->durationSpi();
}
gint64 AbstractPlayer::position() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->positionSpi();
}
gint AbstractPlayer::volume() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->volumeSpi();
}
gboolean AbstractPlayer::isMuted() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->isMutedSpi();
}
gboolean AbstractPlayer::isAudioAvailable() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->isAudioAvailableSpi();
}
gboolean AbstractPlayer::isVideoAvailable() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->isVideoAvailableSpi();
}
gboolean AbstractPlayer::isSeekable() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->isSeekableSpi();
}
gfloat AbstractPlayer::playbackRate() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->playbackRateSpi();
}

//Error AbstractPlayer::error() const;
GString AbstractPlayer::errorString() const
{
	LOG_FUNCTION_SCOPE_NORMAL_D("AbstractPlayer");
	return this->errorStringSpi();
}
