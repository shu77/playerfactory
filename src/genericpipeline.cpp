/*
 * genericpipeline.cpp
 *
 *  Created on: 2013. 4. 5.
 *      Author: jeongseok.kim
 */

#include <glibmm-utils/glibmm-utils.h>
#include <pipeline/generic/genericpipeline.hpp>

using namespace mediapipeline;

GenericPipeline::GenericPipeline() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

GenericPipeline::~GenericPipeline() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

void GenericPipeline::load() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

void GenericPipeline::unload() {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}

void GenericPipeline::play(int rate) {
	LOG_FUNCTION_SCOPE_NORMAL_D("GenericPipeline");
}
