#ifndef TICH_SYSTEM_H
#define TICH_SYSTEM_H

#include "core/reference.h"

class TichSystem : public Reference
{
public:
	TichSystem();

	void Update(float dts);

private:
	void Save();
	void Load();
	void MakeSceneOwner();
	void SetOwnerRecursively(Node* node, Node* owner);

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
