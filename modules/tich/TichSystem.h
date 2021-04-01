#ifndef TICH_SYSTEM_H
#define TICH_SYSTEM_H

#include "core/reference.h"
#include "core/vector.h"

class ParallaxBackground;


enum Complexity : uint16_t {
	LEVEL_1,
	LEVEL_2,
	LEVEL_3
};

class TichSystem : public Reference
{
	friend class SceneTree;

public:
	TichSystem();

	void Update(uint64_t frameTime);
	void ChangeComplexity();
	bool Save(const String &file);
	bool Load(const String &file);

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
	bool lastButtonStateF7;
	uint64_t currentTreeVersion;

	uint16_t currentComplexity;

	bool manualLoad;
	int8_t screenshotCountDown;
	String screenshotFileName;

private:
	Vector<ParallaxBackground*> parallaxBackgrounds;

private:
	static TichSystem* s_Instance;
};

#endif
