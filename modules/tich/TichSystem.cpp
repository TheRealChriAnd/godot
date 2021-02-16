#include "TichSystem.h"

#include "main/input_default.h"

#include "core/os/keyboard.h"

#include "scene_tree.h"
#include "packed_scene.h"
#include "TichInfo.h"

//#define SAVE_FILE "res://saved.tich"
#define SAVE_FILE "res://saved.tscn"

TichSystem* TichSystem::s_Instance = nullptr;

TichSystem::TichSystem()
{
	s_Instance = this;
	currentTreeVersion = 1;
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

	if (currentTreeVersion != SceneTree::get_singleton()->get_tree_version())
	{
		MakeSceneOwner();

		currentTreeVersion = SceneTree::get_singleton()->get_tree_version();

		WARN_PRINT("TreeVersion Changed")
	}

}

void TichSystem::Save()
{
	TichInfo::s_IsSaving = true;
	WARN_PRINT("Saving");

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

	result = ResourceSaver::save(SAVE_FILE, packedScene);

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
	if (TichInfo::s_IsLoading)
		return;

	TichInfo::s_IsLoading = true;
	WARN_PRINT("Loading");

	Error result = SceneTree::get_singleton()->change_scene(SAVE_FILE);

	currentTreeVersion = SceneTree::get_singleton()->get_tree_version();

	if (result != Error::OK)
	{
		ERR_PRINT("Failed to load scene, Error: " + result);
		return;
	}

	WARN_PRINT("Scene Loaded Successfully");
}

void TichSystem::OnReadyPost()
{
	TichInfo::s_IsLoading = false;
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
