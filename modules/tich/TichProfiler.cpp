#include "TichProfiler.h"

#include "TichSystem.h"

#include "resource_format_memory.h"

TichProfiler *TichProfiler::singleton = NULL;

TichProfiler::TichProfiler() :
	profilingData(),
	sample(0),
	executionInterval(0),
	save(false),
	gaImplementation(true)
{
	singleton = this;
}

TichProfiler::~TichProfiler()
{

}

void TichProfiler::Update(uint64_t frameTime)
{
	if (sample)
	{
		ProfilerData data;

		Performance *perf = Performance::get_singleton();

		if ((sample % executionInterval) == 0)
		{
			OS *os = OS::get_singleton();
			uint64_t time = os->get_ticks_usec();

			if (gaImplementation)
			{
				if (save)
					TichSystem::GetInstance()->Save();
				else
					TichSystem::GetInstance()->Load();
			}
			else
			{
				if (save)
					emit_signal("_save");
				else
					emit_signal("_load");
			}

			data.executionTime = os->get_ticks_usec() - time;
		}
		else
		{
			data.executionTime = 0;
		}

		data.frameTime = frameTime;
		data.memory = Memory::get_mem_usage();
		data.nodes = perf->get_monitor(Performance::Monitor::OBJECT_NODE_COUNT);
		data.objects = perf->get_monitor(Performance::Monitor::OBJECT_COUNT);
		data.stateSize = ResourceFormatSaverMemory::get_singleton()->get_state_size();

		profilingData.push_back(data);

		sample = sample - 1;

		if (sample == 0)
		{
			Error err;
			FileAccess *file = FileAccess::open(dataPath, FileAccess::WRITE, &err);

			String headers = "Frame Time(us);Execution Time(us);Memory(bytes);State Size(bytes);Nodes;Objects\n";

			file->store_string(headers);

			String content;
			for (int i = 0; i < profilingData.size(); i++)
			{
				const ProfilerData &data = profilingData[i];

				content += itos(data.frameTime) + ";" + itos(data.executionTime) + ";" + itos(data.memory) + ";" + itos(data.stateSize) + ";" + itos(data.nodes) + ";" + itos(data.objects) + "\n";
			}

			file->store_string(content);

			file->close();
			memdelete(file);
		}
	}
}

void TichProfiler::Start(uint64_t samples, uint16_t executionInterval, bool save, bool gaImplementation)
{
	this->sample = samples;
	this->save = save;
	this->gaImplementation = gaImplementation;
	this->executionInterval = executionInterval;
	this->dataPath = "data_" + String(gaImplementation ? "ga" : "gs") + "_" + String(save ? "save" : "load") + ".csv";

	profilingData.clear();
}

void TichProfiler::StartGs(uint64_t samples, uint16_t executionInterval, bool save)
{
	Start(samples, executionInterval, save, false);
}

void TichProfiler::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("Start", "samples", "executionInterval", "save"), &TichProfiler::StartGs);

	ADD_SIGNAL(MethodInfo("_save"));
	ADD_SIGNAL(MethodInfo("_load"));
}
