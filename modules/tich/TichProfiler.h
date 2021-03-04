#ifndef TICHPROFILER_H
#define TICHPROFILER_H


#include "core/os/os.h"
#include "main/Performance.h"

class TichProfiler : public Reference
{
	GDCLASS(TichProfiler, Reference);

	static TichProfiler *singleton;
	static void _bind_methods();

private:

	struct ProfilerData {

		uint64_t frameTime;
		uint64_t executionTime;
		uint64_t nodes;
		uint64_t objects;
		uint64_t memory;

	};

	void StartGs(uint64_t samples, uint16_t executionInterval, bool save);

private:

	Vector<ProfilerData> profilingData;
	uint64_t sample;
	uint16_t executionInterval;
	String dataPath;
	bool save;
	bool gaImplementation;

public:
	TichProfiler();
	~TichProfiler();

	void Update(uint64_t frameTime);


	void Start(uint64_t samples, uint16_t executionInterval, bool save, bool gaImplementation = true);

	static TichProfiler *get_singleton() { return singleton; }

};



#endif // !TICHPROFILER_H
