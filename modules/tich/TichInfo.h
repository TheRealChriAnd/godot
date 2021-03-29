#ifndef TICH_INFO_H
#define TICH_INFO_H

class TichInfo
{
	friend class TichSystem;
	friend class TichProfiler;

public:
	static bool IsSaving();
	static bool IsLoading();
	static bool IsGA();

private:
	static bool s_IsSaving;
	static bool s_IsLoading;
	static bool s_IsGA;
};

#endif
