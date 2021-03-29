#include "FunctionProfiler.h"

FunctionProfiler *FunctionProfiler::singleton = NULL;

FunctionProfiler::FunctionProfiler()
{
	singleton = this;
}

FunctionProfiler::~FunctionProfiler()
{

}

void FunctionProfiler::Begin(const StringName &functionName)
{
	Map<StringName, Data>::Element* e = profilingData.find(functionName);
	if (!e)
		e = profilingData.insert(functionName, Data());

	Data &data = e->get();
	if (data.IsProfiling)
	{
		ERR_PRINT("Profiling of function [" + functionName + "] already started!");
		return;
	}
	data.IsProfiling = true;
	data.executionTimes.push_back(OS::get_singleton()->get_ticks_usec());
}

void FunctionProfiler::End(const StringName &functionName)
{
	Map<StringName, Data>::Element *e = profilingData.find(functionName);
	if (!e)
	{
		ERR_PRINT("Profiling of function [" + functionName + "] does not exist!");
		return;
	}

	Data &data = e->get();
	if (!data.IsProfiling)
	{
		ERR_PRINT("Profiling of function [" + functionName + "] has not started yet!");
		return;
	}
	data.IsProfiling = false;
	int lastIndex = data.executionTimes.size() - 1;
	data.executionTimes.set(lastIndex, OS::get_singleton()->get_ticks_usec() - data.executionTimes.get(lastIndex));
}

void FunctionProfiler::SaveToFile()
{
	Error err;
	FileAccess *file = FileAccess::open("data/function_time.csv", FileAccess::WRITE, &err);

	String content;

	for (Map<StringName, Data>::Element *e = profilingData.front(); e; e = e->next())
	{
		Data &data = e->get();
		content += String(e->key()) + ";";
		for (int i = 0; i < data.executionTimes.size(); i++)
		{
			content += itos(data.executionTimes.get(i)) + (i == data.executionTimes.size() - 1 ? "\n" : ";");
		}
	}

	file->store_string(content);
	file->close();
	memdelete(file);
}
