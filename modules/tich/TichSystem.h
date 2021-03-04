#ifndef TICH_SYSTEM_H
#define TICH_SYSTEM_H

#include "core/reference.h"
#include "core/vector.h"

class ParallaxBackground;

class TichSystem : public Reference
{
	friend class SceneTree;

public:
	TichSystem();

	void Update(uint64_t frameTime);
	bool Save();
	bool Load();

private:

	void OnReadyPost();
	void MakeSceneOwner();
	void SetOwnerRecursively(Node* node, Node* owner);
	void GetParallaxBackgrounds(Vector<ParallaxBackground*>* vector, Node* node = nullptr);

	void OnPreSave();
	void OnPostSave();

public:
	static TichSystem* GetInstance();

private:
	bool lastButtonStateF1;
	bool lastButtonStateF2;
	bool lastButtonStateF3;
	bool lastButtonStateF4;
	bool lastButtonStateF5;
	bool lastButtonStateF6;
	uint64_t currentTreeVersion;

private:
	Vector<ParallaxBackground*> parallaxBackgrounds;

private:
	static TichSystem* s_Instance;
};

#endif
