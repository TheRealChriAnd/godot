#include "TichProfiler.h"


TichProfiler::TichProfiler():
	profilingData(),
	sample(0) {

}

TichProfiler::~TichProfiler() {
}

void TichProfiler::Update(uint64_t frameTime)
{
	if (sample)
	{
		ProfilerData data;

		Performance *perf = Performance::get_singleton();

		data.frameTime = frameTime;
		data.memory = Memory::get_mem_usage();
		data.nodes = perf->get_monitor(Performance::Monitor::OBJECT_NODE_COUNT);
		data.objects = perf->get_monitor(Performance::Monitor::OBJECT_COUNT);

		if ((sample % executionInterval) == 0)
		{
			OS *os = OS::get_singleton();

			uint64_t time = os->get_ticks_usec();

			data.executionTime = time;
		}
		else
		{
			data.executionTime = 0;
		}

		sample = sample - 1;
	}

}

void TichProfiler::Start(uint64_t samples, uint16_t executionInterval)
{
	this->sample = samples;
	this->executionInterval = executionInterval;
}
