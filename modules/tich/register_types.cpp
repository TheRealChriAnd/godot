#include "register_types.h"

#include "resource_format_memory.h"
#include "TichSystem.h"
#include "TichProfiler.h"
#include "FunctionProfiler.h"

#include "core/class_db.h"

#include "core/io/resource_saver.h"
#include "core/io/resource_loader.h"

#include "core/io/file_access_memory.h"

static Ref<ResourceFormatSaverMemory> resource_saver_memory;
static Ref<ResourceFormatLoaderMemory> resource_loader_memory;

static Ref<TichSystem> tichSystem;
static Ref<TichProfiler> tichProfiler;
static Ref<FunctionProfiler> functionProfiler;

void register_tich_types()
{
	Vector<uint8_t> data;
	data.resize(1024 * 1024 * 64);

	FileAccessMemory::register_file("data", data);

	resource_saver_memory.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_memory);

	resource_loader_memory.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_memory);

	ClassDB::register_class<TichSystem>();
	ClassDB::register_class<TichProfiler>();

	tichSystem.instance();
	tichProfiler.instance();
	functionProfiler.instance();

	Engine::get_singleton()->add_singleton(Engine::Singleton("TichProfiler", tichProfiler.ptr()));
}

void unregister_tich_types()
{
	FileAccessMemory::cleanup();

	ResourceSaver::remove_resource_format_saver(resource_saver_memory);
	resource_saver_memory.unref();

	ResourceLoader::remove_resource_format_loader(resource_loader_memory);
	resource_loader_memory.unref();

	tichSystem.unref();
	tichProfiler.unref();
	functionProfiler.unref();
}
