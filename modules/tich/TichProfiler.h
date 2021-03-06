#ifndef TICHPROFILER_H
#define TICHPROFILER_H

#include "core/os/os.h"
#include "main/Performance.h"

class TichProfiler : public Reference
{
	GDCLASS(TichProfiler, Reference);

	static TichProfiler *singleton;
	static void _bind_methods();

	void InitCPUCounter();

private:

	struct ProfilerData {

		uint64_t frameTime;
		uint64_t executionTime;
		double	 cpu;
		uint64_t nodes;
		uint64_t objects;
		uint64_t memory;
		uint64_t stateSize;
	};

	void StartGs(const String &saveFileExtention, uint64_t samples, uint16_t executionInterval, bool save);

private:
	Vector<ProfilerData> profilingData;
	uint64_t timeStamp;
	uint64_t sample;
	uint16_t executionInterval;
	String dataPath;
	String saveFileExtention;
	bool save;
	uint64_t index;
	uint64_t exectionCounter;
	uint64_t dataFileSize;
	uint64_t timelineSize;
	void* handle;
	uint64_t cyclesLast;
	uint64_t cpuFrequency;

public:
	TichProfiler();
	~TichProfiler();

	void Update(uint64_t frameTime);

	void Start(const String& saveFileExtention, uint64_t samples, uint16_t executionInterval, uint16_t complexityLevel, bool save, bool gaImplementation = true);

	double getCPUUsage(uint64_t deltaTime);

	static TichProfiler *get_singleton() { return singleton; }
};
#endif // !TICHPROFILER_H
