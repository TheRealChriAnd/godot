#ifndef FUNCTION_PROFILER_H
#define FUNCTION_PROFILER_H

#include "core/os/os.h"
#include "main/Performance.h"

#include "core/map.h"

//#define DEBUG_FUNCTION_PROFILER

#ifdef DEBUG_FUNCTION_PROFILER
#define FUNCTION_PROFILER_BEGIN(x) FunctionProfiler::get_singleton()->Begin(x)
#define FUNCTION_PROFILER_END(x) FunctionProfiler::get_singleton()->End(x)
#define FUNCTION_PROFILER_SAVE() SaveToFile()
#else
#define FUNCTION_PROFILER_BEGIN(x)
#define FUNCTION_PROFILER_END(x)
#define FUNCTION_PROFILER_SAVE()
#endif


class FunctionProfiler : public Reference
{
	GDCLASS(FunctionProfiler, Reference);

	static FunctionProfiler *singleton;

	struct Data
	{
		bool IsProfiling;
		Vector<uint64_t> executionTimes;
	};

private:
	Map<StringName, Data> profilingData;

public:
	FunctionProfiler();
	~FunctionProfiler();

	void Begin(const StringName& functionName);
	void End(const StringName& functionName);
	void SaveToFile();

	static FunctionProfiler *get_singleton() { return singleton; }
};
#endif
