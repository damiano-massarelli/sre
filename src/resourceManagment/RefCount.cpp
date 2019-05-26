#include "RefCount.h"

void RefCount::copy(const RefCount& rc)
{
	if (refs != rc.refs && rc.refs != nullptr) {
		decrease();

		// working on new refs
		refs = rc.refs;
		*refs = *refs + 1;
	}
}

void RefCount::decrease()
{
	if (refs == nullptr) return;
	*refs = *refs - 1;
	if (*refs == 0) {
		delete refs;
		refs = nullptr;
	}
}

RefCount::RefCount()
{
	refs = new int{ 1 };
}

RefCount::RefCount(const RefCount& rc)
{
	copy(rc);
}

bool RefCount::shouldCleanUp() const
{
	return *refs == 1;
}

RefCount & RefCount::operator=(const RefCount& rc)
{
	copy(rc);

	return *this;
}

RefCount::~RefCount()
{
	decrease();
}
