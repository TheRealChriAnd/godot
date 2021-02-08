#include "register_types.h"

#include "core/class_db.h"
#include "resource_format_memory.h"

#include "core/io/resource_saver.h"
#include "core/io/resource_loader.h"

#include "core/io/file_access_memory.h"

static Ref<ResourceFormatSaverMemory> resource_saver_memory;
static Ref<ResourceFormatLoaderMemory> resource_loader_memory;

void register_resource_format_memory_types()
{
	Vector<uint8_t> data;
	data.resize(4096);

	FileAccessMemory::register_file("data", data);

	resource_saver_memory.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_memory);

	resource_loader_memory.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_memory);
}

void unregister_resource_format_memory_types()
{
	FileAccessMemory::cleanup();

	ResourceSaver::remove_resource_format_saver(resource_saver_memory);
	resource_saver_memory.unref();

	ResourceLoader::remove_resource_format_loader(resource_loader_memory);
	resource_loader_memory.unref();
}
