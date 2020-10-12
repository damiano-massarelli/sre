#pragma once
#include <cstdint>
#include <functional>

/**
 * Reference Count utility for cachable objects.
 * A cachable object must keep an instance variable of this
 * type. In its destructor it should query shouldCleanUp() method,
 * if this method returns true then resources should be cleaned up.
 */
class RefCount {
private:
    std::int32_t* mRefs = nullptr;
    bool mIsWeak = false;

    void copy(const RefCount& rc);

    /**
     * Decreases the reference count. */
    void decrease();

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
     * is the last one and should be responsible for cleaning up resources.
     * @return true if resources should be cleaned up, false otherwise
     */
    bool shouldCleanUp() const;

    /**
     * Sets this reference count as a weak reference count.
     * Used by objects stored in caches.
     */
    void setWeak();

    RefCount& operator=(const RefCount& rc);

    ~RefCount();
};
