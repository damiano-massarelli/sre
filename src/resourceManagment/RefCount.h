#pragma once
#include <cstdint>
#include <functional>

/**
 * Reference Count utility for cachable objects.
 * A cachable object must keep an instance variable of this
 * type. In its destructor it should query shouldCleanUp() method, 
 * if this method returns true then resources should be cleaned up.
 */
class RefCount
{
	private:
		std::int32_t* refs = nullptr;

		void copy(const RefCount& rc);

	public:
		RefCount();

		RefCount(const RefCount& rc);

		/**
		 * A function to call when the reference count is 0.
		 * Usually used to clean up weak references in caches.
		 */
		std::function<void()> onRemove = nullptr;

		/**
		 * Returns true if the object holding this reference 
		 * is the last one and should be responsible for cleaning up resources
		 */
		bool shouldCleanUp() const;

		/**
		 * Decreases the reference count.
		 * Can be used when a weak reference is created.
		 */
		void decrease();

		RefCount& operator=(const RefCount& rc);

		~RefCount();
};

