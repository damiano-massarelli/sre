#pragma once
#include <cstdint>
#include <functional>
class RefCount
{
	private:
		std::int32_t* refs = nullptr;

		void copy(const RefCount& rc);

	public:
		RefCount();

		RefCount(const RefCount& rc);

		std::function<void()> onRemove = nullptr;

		bool shouldCleanUp() const;

		void decrease();

		RefCount& operator=(const RefCount& rc);

		~RefCount();
};

