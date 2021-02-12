#ifndef TICH_INFO_H
#define TICH_INFO_H

class TichInfo
{
	friend class TichSystem;

public:
	static bool IsSaving();

private:
	static bool s_IsSaving;
};

#endif
