#include "RefCount.h"

void RefCount::copy(const RefCount& rc)
{
	if (mRefs != rc.mRefs && rc.mRefs != nullptr) {
		decrease();

		// working on new refs
		mRefs = rc.mRefs;
		*mRefs = *mRefs + 1;
		onRemove = rc.onRemove;
	}
}

void RefCount::decrease()
{
	if (mRefs == nullptr || mIsWeak) return;
	*mRefs = *mRefs - 1;
	if (*mRefs == 0) {
		if (onRemove)
			onRemove();

		delete mRefs;
		mRefs = nullptr;
	}
}

RefCount::RefCount()
{
	mRefs = new int{ 1 };
}

RefCount::RefCount(const RefCount& rc)
{
	copy(rc);
}

bool RefCount::shouldCleanUp() const
{
	return *mRefs == 1;
}

void RefCount::setWeak()
{
	decrease();
	mIsWeak = true;
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
