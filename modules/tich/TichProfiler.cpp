#include "TichProfiler.h"

#include "TichSystem.h"
#include "TichInfo.h"
#include "core/os/os.h"

#include "resource_format_memory.h"

#include <thread>
#include <windows.h>

TichProfiler *TichProfiler::singleton = NULL;

TichProfiler::TichProfiler() :
	profilingData(),
	timeStamp(0),
	sample(0),
	executionInterval(0),
	save(false),
	gaImplementation(true)
{
	singleton = this;

	InitCPUCounter();
}

TichProfiler::~TichProfiler()
{

}

void TichProfiler::InitCPUCounter()
{
	OS *os = OS::get_singleton();

	handle = GetCurrentProcess();

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numProcessors = sysinfo.dwNumberOfProcessors / 2;

	int cpuInfo[4] = { 0, 0, 0, 0 };
	__cpuid(cpuInfo, 0);
	if (cpuInfo[0] >= 0x16)
	{
		__cpuid(cpuInfo, 0x16);

		os->print("EAX: 0x%08x EBX: 0x%08x ECX: %08x\r\n", cpuInfo[0], cpuInfo[1], cpuInfo[2]);
		os->print("Processor Base Frequency:  %04d MHz\r\n", cpuInfo[0]);
		os->print("Maximum Frequency:         %04d MHz\r\n", cpuInfo[1]);
		os->print("Bus (Reference) Frequency: %04d MHz\r\n", cpuInfo[2]);
		os->print("Cores %d\r\n", numProcessors);

		cpuFrequency = cpuInfo[1];
		cpuFrequency *= numProcessors;
		cpuFrequency *= 1000000;
	}
	else
	{
		os->print("CPUID level 16h unsupported\r\n");
	}
}

double TichProfiler::getCPUUsage(uint64_t deltaTime)
{
	uint64_t cycles;
	QueryProcessCycleTime(handle, &cycles);
	uint64_t deltaCycles = cycles - cyclesLast;
	cyclesLast = cycles;

	double deltaSeconds = deltaTime / (double)1000000;
	double cyclesPerSecond = deltaCycles / deltaSeconds;
	double percentage = cyclesPerSecond / (double)cpuFrequency;

	return percentage * 100;
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
				{
					TichInfo::s_IsSaving = true;
					emit_signal("_save");
					TichInfo::s_IsSaving = false;
				}
				else
				{
					TichInfo::s_IsLoading = true;
					emit_signal("_load");
					TichInfo::s_IsLoading = false;
				}	
			}

			data.executionTime = os->get_ticks_usec() - time;
		}
		else
		{
			data.executionTime = 0;
		}

		data.frameTime	= frameTime;
		data.cpu		= getCPUUsage(frameTime);
		data.memory		= Memory::get_mem_usage();
		data.nodes		= perf->get_monitor(Performance::Monitor::OBJECT_NODE_COUNT);
		data.objects	= perf->get_monitor(Performance::Monitor::OBJECT_COUNT);
		data.stateSize	= ResourceFormatSaverMemory::get_singleton()->get_state_size();

		profilingData.push_back(data);

		sample = sample - 1;

		if (sample == 0)
		{
			Error err;
			FileAccess *file = FileAccess::open(dataPath, FileAccess::WRITE, &err);

			String headers = "Frame Time(us);Execution Time(us);CPU(%);Memory(bytes);State Size(bytes);Nodes;Objects;Execution Time(us)\n";

			file->store_string(headers);

			int executionStack = 0;
			String content;
			for (int i = 0; i < profilingData.size(); i++)
			{
				const ProfilerData &data = profilingData[i];

				int exeutionTime = data.executionTime;
				if (exeutionTime == 0)
				{
					for (int j = executionStack; j < profilingData.size(); j++)
					{
						if (profilingData[j].executionTime != 0)
						{
							exeutionTime = profilingData[j].executionTime;
							executionStack = j;
							break;
						}
					}
				}
				executionStack++;

				content += itos(data.frameTime) + ";" + itos(data.executionTime) + ";" + rtos(data.cpu) + ";" + itos(data.memory) + ";" + itos(data.stateSize) + ";" + itos(data.nodes) + ";" + itos(data.objects) + ";" + (exeutionTime ? itos(exeutionTime) : "") + "\n";
			}

			file->store_string(content);

			file->close();
			memdelete(file);

			OS *os = OS::get_singleton();
			os->print("Profiling finished in %f s\n", (os->get_ticks_usec() - timeStamp) / 1000.0F / 1000.0F);
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

	getCPUUsage(0);

	OS *os = OS::get_singleton();
	timeStamp = os->get_ticks_usec();
	os->print("Profiling started [Samples: %llu, Interval: %llu, Implementation: %s, Mode: %s]\n",
		samples,
		executionInterval,
		(gaImplementation ? "GA" : "GS"),
		(save ? "Save" : "Load"));
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
