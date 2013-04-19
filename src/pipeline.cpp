/*
 * pipeline.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */


#include <pipeline/pipeline.hpp>
using namespace mediapipeline;


Pipeline::Pipeline() {
    
    m_gstPipelineState = StoppedState;
    m_pendingState = StoppedState;
    m_busHandler = NULL;
    m_playbinVersion = 0;
    m_audioAvailable = false; 
    m_videoAvailable = false;
    m_playbackRate = 1.0; 
    m_volume = 100; 
    m_muted = false;
    m_seekable = false;
    m_duration = -1;
    m_currentPosition = 0;
    m_durationQueryCount = 0;
    m_blockByVideoSink = 0;
    m_uri.clear();
    m_subtitle_uri.clear();
}

Pipeline::~Pipeline() {

}


void Pipeline::stateChanged(Pipeline::State newState)
{



}

void Pipeline::seekableStateChanged(bool seekable)
{



}

void Pipeline::playbackRateChanged(gfloat rate)
{


}

