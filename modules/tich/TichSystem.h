#ifndef TICH_SYSTEM_H
#define TICH_SYSTEM_H

#include "core/reference.h"

class ParallaxBackground;

class TichSystem : public Reference
{
	friend class Node;

public:
	TichSystem();

	void Update(float dts);

private:
	void Save();
	void Load();
	void OnReadyPost();
	void MakeSceneOwner();
	void SetOwnerRecursively(Node* node, Node* owner);
	void GetParallaxBackgrounds(Vector<ParallaxBackground*>& vector, Node* node = nullptr);

public:
	static TichSystem* GetInstance();

private:
	bool lastButtonStateF1;
	bool lastButtonStateF2;
	uint64_t currentTreeVersion;

private:
	static TichSystem* s_Instance;
};

#endif
