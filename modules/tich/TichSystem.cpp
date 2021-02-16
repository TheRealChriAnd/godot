#include "TichSystem.h"

#include "main/input_default.h"

#include "core/os/keyboard.h"

#include "scene_tree.h"
#include "packed_scene.h"
#include "TichInfo.h"

#include "scene/2d/parallax_background.h"

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


	Vector<ParallaxBackground *> vector;
	Vector<bool> values;
	GetParallaxBackgrounds(vector);
	for (int i = 0; i < vector.size(); i++)
	{
		ParallaxBackground* pb = vector.get(i);
		values.push_back(pb->is_ignore_camera_zoom());
		pb->set_ignore_camera_zoom(true);
	}


	Ref<PackedScene> packedScene;
	packedScene.instance();

	Node* scene = SceneTree::get_singleton()->get_current_scene();
	Error result = packedScene->pack(scene);


	for (int i = 0; i < vector.size(); i++)
		vector.get(i)->set_ignore_camera_zoom(values.get(i));


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

void TichSystem::GetParallaxBackgrounds(Vector<ParallaxBackground*>& vector, Node* node)
{
	if (!node)
	{
		node = SceneTree::get_singleton()->get_current_scene();
		if (!node)
			return;
	}

	ParallaxBackground* parallaxBackground = dynamic_cast<ParallaxBackground*>(node);
	if (parallaxBackground)
		vector.push_back(parallaxBackground);

	for (int i = 0; i < node->get_child_count(); i++)
	{
		GetParallaxBackgrounds(vector, node->get_child(i));
	}
}

TichSystem *TichSystem::GetInstance()
{
	return s_Instance;
}
