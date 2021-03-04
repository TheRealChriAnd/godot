#ifndef TICHPROFILER_H
#define TICHPROFILER_H


#include "core/os/os.h"
#include "main/Performance.h"

class TichProfiler
{
public:

	TichProfiler();
	~TichProfiler();

	void Update(uint64_t frameTime);

	void Start(uint64_t samples, uint16_t executionInterval);

private:

	struct ProfilerData {

		uint64_t frameTime;
		uint64_t executionTime;
		uint64_t nodes;
		uint64_t objects;
		uint64_t memory;

	};


private:

	Vector<ProfilerData> profilingData;
	uint64_t sample;
	uint16_t executionInterval;
};



#endif // !TICHPROFILER_H
