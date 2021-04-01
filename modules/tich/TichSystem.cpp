#include "TichSystem.h"

#include "scene/2d/canvas_item.h"
#include "scene/main/scene_tree.h"
#include "scene/2d/parallax_layer.h"
#include "scene/resources/packed_scene.h"
#include "TichInfo.h"

#include "main/input_default.h"
#include "core/os/keyboard.h"
#include "scene/2d/parallax_background.h"
#include "core/engine.h"

#include "core/os/os.h"
#include "main/Performance.h"

#include "resource_format_memory.h"
#include "TichProfiler.h"

#include "FunctionProfiler.h"

#define SAVE_FILE "res://saved.tich"
//#define SAVE_FILE "res://saved.tscn"

TichSystem* TichSystem::s_Instance = nullptr;

TichSystem::TichSystem()
{
	s_Instance = this;
	currentTreeVersion = 1;
	currentComplexity = Complexity::LEVEL_2;
	lastButtonStateF1 = false;
	lastButtonStateF2 = false;
	screenshotCountDown = -1;
}

void TichSystem::Update(uint64_t frameTime)
{
	if (Engine::get_singleton()->is_editor_hint())
		return;

	InputDefault *input = (InputDefault *)Input::get_singleton();

	//Manual
	bool buttonStateF1 = input->is_key_pressed(KeyList::KEY_F1);
	bool buttonStateF2 = input->is_key_pressed(KeyList::KEY_F2);

	//Auto Gs
	bool buttonStateF3 = input->is_key_pressed(KeyList::KEY_F3);
	bool buttonStateF4 = input->is_key_pressed(KeyList::KEY_F4);

	//Auto Ga
	bool buttonStateF5 = input->is_key_pressed(KeyList::KEY_F5);
	bool buttonStateF6 = input->is_key_pressed(KeyList::KEY_F6);

	//Change Level Complexity
	bool buttonStateF7 = input->is_key_pressed(KeyList::KEY_F7);

	if (buttonStateF1)
	{
		if (!lastButtonStateF1)
		{
			OS* os = OS::get_singleton();
			uint64_t time = os->get_ticks_usec();

			if (Save())
			{
				screenshotFileName = "_save.png";
				screenshotCountDown = 1;
				time = os->get_ticks_usec() - time;
				os->print("Save Time %llu\n", time);
				os->print("Memory %llu\n", Memory::get_mem_usage());
				os->print("Frame Time %llu\n", frameTime);
				os->print("State Size %llu\n", ResourceFormatSaverMemory::get_singleton()->get_state_size());
			}
		}	
	}
	else if (buttonStateF2)
	{
		if (!lastButtonStateF2)
		{
			OS *os = OS::get_singleton();
			uint64_t time = os->get_ticks_usec();

			Ref<Image> image = SceneTree::get_singleton()->get_root()->get_texture()->get_data();
			image->flip_y();
			Variant var = ProjectSettings::get_singleton()->get_setting("application/config/name");
			image->save_png("screenshots/" + (String)var + "_preload.png");

			if(Load())
			{
				screenshotFileName = "_load.png";
				manualLoad = true;
				time = os->get_ticks_usec() - time;
				os->print("Load Time %llu\n", time);
				os->print("Memory %llu\n", Memory::get_mem_usage());
				os->print("Frame Time %llu\n", frameTime);
			}
		}	
	}
	else if (buttonStateF3) //Gs Save
	{
		if (!lastButtonStateF3)
		{
			TichProfiler::get_singleton()->Start(600, 60, currentComplexity, true, false);
		}
	}
	else if (buttonStateF4) //Gs Load
	{
		if (!lastButtonStateF4)
		{
			TichProfiler::get_singleton()->Start(600, 60, currentComplexity, false, false);
		}
	}
	else if (buttonStateF5) //Ga Save
	{
		if (!lastButtonStateF5)
		{
			TichProfiler::get_singleton()->Start(600, 60, currentComplexity, true, true);
		}
	}
	else if (buttonStateF6) //GaLoad
	{
		if (!lastButtonStateF6)
		{
			TichProfiler::get_singleton()->Start(600, 60, currentComplexity, false, true);
		}
	}

	if (buttonStateF7) // change level
	{
		if (!lastButtonStateF7)
		{
			currentComplexity = (currentComplexity + 1) % 3;

			switch (currentComplexity) {
				case Complexity::LEVEL_1:
					TichProfiler::get_singleton()->emit_signal("_change_level");
					break;
				case Complexity::LEVEL_2:
					TichProfiler::get_singleton()->emit_signal("_change_level");
					break;
				case Complexity::LEVEL_3:
					break;
				default:
					break;
			}	
		}
	}

	lastButtonStateF1 = buttonStateF1;
	lastButtonStateF2 = buttonStateF2;
	lastButtonStateF3 = buttonStateF3;
	lastButtonStateF4 = buttonStateF4;
	lastButtonStateF5 = buttonStateF5;
	lastButtonStateF6 = buttonStateF6;
	lastButtonStateF7 = buttonStateF7;

	if (currentTreeVersion != SceneTree::get_singleton()->get_tree_version())
	{
		MakeSceneOwner();

		currentTreeVersion = SceneTree::get_singleton()->get_tree_version();
	}

	if (screenshotCountDown >= 0)
	{
		if (screenshotCountDown == 0)
		{
			Ref<Image> image = SceneTree::get_singleton()->get_root()->get_texture()->get_data();
			image->flip_y();
			Variant var = ProjectSettings::get_singleton()->get_setting("application/config/name");
			image->save_png("screenshots/" + (String)var + screenshotFileName);
		}
		screenshotCountDown--;
	}

	TichProfiler::get_singleton()->Update(frameTime);
}

void TichSystem::ChangeComplexity()
{
}

bool TichSystem::Save()
{
	if (TichInfo::s_IsLoading)
		return false;

	TichInfo::s_IsSaving = true;
	//WARN_PRINT("Saving");

	FUNCTION_PROFILER_BEGIN("OnPreSave()");
	OnPreSave();
	FUNCTION_PROFILER_END("OnPreSave()");

	Ref<PackedScene> packedScene;
	packedScene.instance();

	Node *scene = SceneTree::get_singleton()->get_root();

	FUNCTION_PROFILER_BEGIN("packedScene->pack()");
	Error result = packedScene->pack(scene);
	FUNCTION_PROFILER_END("packedScene->pack()");

	FUNCTION_PROFILER_BEGIN("OnPostSave()");
	OnPostSave();
	FUNCTION_PROFILER_END("OnPostSave()");

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to pack scene, Error: " + result);
		TichInfo::s_IsSaving = false;
		return false;
	}

	FUNCTION_PROFILER_BEGIN("ResourceSaver::save()");
	result = ResourceSaver::save(SAVE_FILE, packedScene);
	FUNCTION_PROFILER_END("ResourceSaver::save()");

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to save scene, Error: " + result);
		TichInfo::s_IsSaving = false;
		return false;
	}

	//WARN_PRINT("Scene Saved Successfully");

	TichInfo::s_IsSaving = false;

	FUNCTION_PROFILER_SAVE();

	return true;
}

bool TichSystem::Load()
{
	if (TichInfo::s_IsLoading)
		return false;

	TichInfo::s_IsLoading = true;
	//WARN_PRINT("Loading");

	Error result = SceneTree::get_singleton()->change_scene(SAVE_FILE);

	currentTreeVersion = SceneTree::get_singleton()->get_tree_version();

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to load scene, Error: " + result);
		return false;
	}

	//WARN_PRINT("Scene Loaded Successfully");
	return true;
}

void TichSystem::OnReadyPost()
{
	TichInfo::s_IsLoading = false;

	MakeSceneOwner();

	if (manualLoad)
	{
		manualLoad = false;
		screenshotCountDown = 0;
	}
}

void TichSystem::MakeSceneOwner()
{
	Node* scene = SceneTree::get_singleton()->get_root();
	if (scene)
		SetOwnerRecursively(scene, scene);
}

void TichSystem::SetOwnerRecursively(Node* node, Node* owner)
{
	if (node != owner)
		node->set_owner(owner);

	for (int i = 0; i < node->get_child_count(); i++)
		SetOwnerRecursively(node->get_child(i), owner);
}

void TichSystem::GetParallaxBackgrounds(Vector<ParallaxBackground*>* vector, Node* node)
{
	if (!node)
	{
		node = SceneTree::get_singleton()->get_root();
		if (!node)
			return;
	}

	ParallaxBackground* parallaxBackground = dynamic_cast<ParallaxBackground*>(node);
	if (parallaxBackground)
	{
		vector->push_back(parallaxBackground);
		return;
	}
		

	for (int i = 0; i < node->get_child_count(); i++)
	{
		GetParallaxBackgrounds(vector, node->get_child(i));
	}
}

void TichSystem::OnPreSave()
{
	GetParallaxBackgrounds(&parallaxBackgrounds);
	for (int i = 0; i < parallaxBackgrounds.size(); i++)
	{
		ParallaxBackground* parallaxBackground = parallaxBackgrounds.get(i);
		for (int j = 0; j < parallaxBackground->get_child_count(); j++)
		{
			ParallaxLayer *parallaxLayer = (ParallaxLayer *)parallaxBackground->get_child(j);
			parallaxLayer->pre_save();
		}
	}
}

void TichSystem::OnPostSave()
{
	for (int i = 0; i < parallaxBackgrounds.size(); i++)
	{
		ParallaxBackground* parallaxBackground = parallaxBackgrounds.get(i);
		for (int j = 0; j < parallaxBackground->get_child_count(); j++)
		{
			ParallaxLayer* parallaxLayer = (ParallaxLayer*)parallaxBackground->get_child(j);
			parallaxLayer->post_save();
		}
	}

	parallaxBackgrounds.clear();
}

TichSystem *TichSystem::GetInstance()
{
	return s_Instance;
}
