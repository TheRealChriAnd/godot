/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "TestSummator.h"

void register_testSummator_types() {
	ClassDB::register_class<Summator>();
}

void unregister_testSummator_types() {
	// Nothing to do here in this example.
}
