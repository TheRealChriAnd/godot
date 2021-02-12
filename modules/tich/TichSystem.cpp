#include "TichSystem.h"

#include "main/input_default.h"

#include "core/os/keyboard.h"

#include "scene/main/scene_tree.h"

#include "scene/main/viewport.h"

#include "packed_scene.h"
#include "TichInfo.h"

TichSystem* TichSystem::s_Instance = nullptr;

TichSystem::TichSystem()
{
	s_Instance = this;
}

void TichSystem::Update(float dts)
{
	InputDefault *input = (InputDefault *)Input::get_singleton();

	bool buttonStateF1 = input->is_key_pressed(KeyList::KEY_F1);
	bool buttonStateF2 = input->is_key_pressed(KeyList::KEY_F2);

	if (buttonStateF1)
	{
		if (!lastButtonStateF1)
		{
			Save();
		}	
	}
	else if (buttonStateF2)
	{
		if (!lastButtonStateF2)
		{
			Load();
		}	
	}

	lastButtonStateF1 = buttonStateF1;
	lastButtonStateF2 = buttonStateF2;
}

void TichSystem::Save()
{
	TichInfo::s_IsSaving = true;
	WARN_PRINT("Saving");

	MakeSceneOwner();

	Ref<PackedScene> packedScene;
	packedScene.instance();

	Node* scene = SceneTree::get_singleton()->get_current_scene();
	Error result = packedScene->pack(scene);

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to pack scene, Error: " + result);
		TichInfo::s_IsSaving = false;
		return;
	}

	result = ResourceSaver::save("res://saved.tich", packedScene);

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to save scene, Error: " + result);
		TichInfo::s_IsSaving = false;
		return;
	}

	WARN_PRINT("Scene Saved Successfully");

	TichInfo::s_IsSaving = false;
}

void TichSystem::Load()
{
	WARN_PRINT("Loading");

	Error result = SceneTree::get_singleton()->change_scene("res://saved.tich");

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to load scene, Error: " + result);
		return;
	}

	WARN_PRINT("Scene Loaded Successfully");
}

void TichSystem::MakeSceneOwner()
{
	Node* scene = SceneTree::get_singleton()->get_current_scene();
	SetOwnerRecursively(scene, scene);
}

void TichSystem::SetOwnerRecursively(Node* node, Node* owner)
{
	if (node != owner)
		node->set_owner(owner);

	for (int i = 0; i < node->get_child_count(); i++)
		SetOwnerRecursively(node->get_child(i), owner);
}

TichSystem *TichSystem::GetInstance()
{
	return s_Instance;
}
