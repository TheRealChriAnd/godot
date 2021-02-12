#ifndef TICH_INFO_H
#define TICH_INFO_H

class TichInfo
{
	friend class TichSystem;

public:
	static bool IsSaving();
	static bool IsLoading();

private:
	static bool s_IsSaving;
	static bool s_IsLoading;
};

#endif
