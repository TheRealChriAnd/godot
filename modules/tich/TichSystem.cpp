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

#define SAVE_FILE "res://saved.tich"
//#define SAVE_FILE "res://saved.tscn"

TichSystem* TichSystem::s_Instance = nullptr;

TichSystem::TichSystem()
{
	s_Instance = this;
	currentTreeVersion = 1;
	lastButtonStateF1 = false;
	lastButtonStateF2 = false;
}

void TichSystem::Update(uint64_t frameTime)
{
	if (Engine::get_singleton()->is_editor_hint())
		return;

	InputDefault *input = (InputDefault *)Input::get_singleton();

	bool buttonStateF1 = input->is_key_pressed(KeyList::KEY_F1);
	bool buttonStateF2 = input->is_key_pressed(KeyList::KEY_F2);

	if (buttonStateF1)
	{
		if (!lastButtonStateF1)
		{
			OS* os = OS::get_singleton();
			uint64_t time = os->get_ticks_usec();
			if (Save())
			{
				time = os->get_ticks_usec() - time;
				os->print("Save Time %llu\n", time);
				os->print("Memory %llu\n", Memory::get_mem_usage());
				os->print("Frame Time %llu\n", frameTime);
				//os->print("Memory %f\n", Performance::get_singleton()->get_monitor(Performance::Monitor::MEMORY_DYNAMIC));
			}
		}	
	}
	else if (buttonStateF2)
	{
		if (!lastButtonStateF2)
		{
			OS *os = OS::get_singleton();
			uint64_t time = os->get_ticks_usec();
			if(Load())
			{
				time = os->get_ticks_usec() - time;
				os->print("Load Time %llu\n", time);
				os->print("Memory %llu\n", Memory::get_mem_usage());
				os->print("Frame Time %llu\n", frameTime);
			}
		}	
	}

	lastButtonStateF1 = buttonStateF1;
	lastButtonStateF2 = buttonStateF2;

	if (currentTreeVersion != SceneTree::get_singleton()->get_tree_version())
	{
		MakeSceneOwner();

		currentTreeVersion = SceneTree::get_singleton()->get_tree_version();
	}
}

bool TichSystem::Save()
{
	if (TichInfo::s_IsLoading)
		return false;

	TichInfo::s_IsSaving = true;
	//WARN_PRINT("Saving");

	OnPreSave();

	Ref<PackedScene> packedScene;
	packedScene.instance();

	Node *scene = SceneTree::get_singleton()->get_root();
	Error result = packedScene->pack(scene);

	OnPostSave();

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to pack scene, Error: " + result);
		TichInfo::s_IsSaving = false;
		return false;
	}

	result = ResourceSaver::save(SAVE_FILE, packedScene);

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to save scene, Error: " + result);
		TichInfo::s_IsSaving = false;
		return false;
	}

	//WARN_PRINT("Scene Saved Successfully");

	TichInfo::s_IsSaving = false;
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
