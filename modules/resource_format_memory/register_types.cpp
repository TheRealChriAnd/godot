#include "register_types.h"

#include "core/class_db.h"
#include "resource_format_memory.h"

#include "core/io/resource_saver.h"
#include "core/io/resource_loader.h"

static Ref<ResourceFormatSaverMemory> resource_saver_memory;
static Ref<ResourceFormatLoaderMemory> resource_loader_memory;

void register_resource_format_memory_types()
{
	ClassDB::register_class<ResourceInteractiveLoaderMemory>();
	ClassDB::register_class<ResourceFormatSaverMemory>();
	ClassDB::register_class<ResourceFormatLoaderMemory>();

	resource_saver_memory.instance();
	ResourceSaver::add_resource_format_saver(resource_saver_memory);
	resource_loader_memory.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_memory);
}

void unregister_resource_format_memory_types()
{
	ResourceSaver::remove_resource_format_saver(resource_saver_memory);
	resource_saver_memory.unref();

	ResourceLoader::remove_resource_format_loader(resource_loader_memory);
	resource_loader_memory.unref();
}
