#include "TichInfo.h"

bool TichInfo::s_IsSaving = false;

bool TichInfo::IsSaving()
{
	return s_IsSaving;
}
