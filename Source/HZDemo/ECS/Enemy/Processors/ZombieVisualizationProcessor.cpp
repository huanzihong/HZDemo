#include "ZombieVisualizationProcessor.h"

UZombieVisualizationProcessor::UZombieVisualizationProcessor()
{
	ExecutionFlags = (int32)EProcessorExecutionFlags::AllNetModes;
	bAutoRegisterWithProcessingPhases = true;
}