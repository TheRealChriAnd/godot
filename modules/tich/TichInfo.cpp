#include "TichInfo.h"

bool TichInfo::s_IsSaving = false;
bool TichInfo::s_IsLoading = false;

bool TichInfo::IsSaving()
{
	return s_IsSaving;
}

bool TichInfo::IsLoading()
{
	return s_IsLoading;
}
