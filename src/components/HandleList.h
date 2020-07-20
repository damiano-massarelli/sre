#ifndef HANDLELIST_H
#define HANDLELIST_H
#include <vector>
#include <queue>
#include <cstdint>
#include <stack>
#include <memory>
#include <map>

/**
  * A Handle manages the relationship between a HandleList and a data vector.
  * \sa HandleList */
struct Handle {
    /** The index for this element in the data vector */
    std::uint32_t index;

    /** The generation of this element */
    std::uint32_t generation;
};

/**
  * An ExternalHandle is a reference to an element in the data vector.
  * Elements in a data vector can be referenced from the outside of a handle
  * list through an ExternalHandle
  * \sa HandleList */
struct ExternalHandle {
    /** the index of this element in the HandleList */
    std::uint32_t mHandleIndex;

    /** The generation of this element */
    std::uint32_t mGeneration;

    /**
      * Creates a new ExternalHandle with a certain index and generation. */
    ExternalHandle(std::uint32_t index, std::uint32_t generation) : mHandleIndex{index}, mGeneration{generation} {}

    virtual ~ExternalHandle() = default;
};

using ExternalHandlePtr = std::shared_ptr<ExternalHandle>;


template <class T>
class HandleList
{
    private:
        std::vector<T>& mData;

        std::vector<Handle> mHandles;
        std::queue<std::uint32_t> mFreeSlots;
        std::map<std::uint32_t, std::uint32_t> dataIndex2handleIndex;

        std::uint32_t mGeneration = 1;

    public:
        HandleList(std::vector<T>& data) : mData{data} {}

        void add(const T& data, std::uint32_t& outHandleIndex, std::uint32_t& outHandleGeneration) {
            std::uint32_t dataIndex = static_cast<std::uint32_t>(mData.size());
            Handle h{dataIndex, mGeneration};
            mGeneration++;

            mData.push_back(data);

            std::uint32_t handleIndex;
            if (mFreeSlots.empty()) {
                handleIndex = static_cast<std::uint32_t>(mHandles.size());
                mHandles.push_back(h);
            }
            else {
                handleIndex = mFreeSlots.front();
                mFreeSlots.pop();
                mHandles[handleIndex] = h;
            }

            dataIndex2handleIndex[dataIndex] = handleIndex;

            outHandleIndex = handleIndex;
            outHandleGeneration = (mGeneration - 1);
        }

        void remove(std::uint32_t handleIndex, std::uint32_t handleGeneration) {
            if (handleIndex >= mHandles.size() || mHandles[handleIndex].generation != handleGeneration)
                throw "Accessing invalid or deleted handle";

            std::uint32_t dataIndexToRemove = mHandles[handleIndex].index;

            // swaps values
            std::uint32_t lastDataIndex = static_cast<std::uint32_t>(mData.size()) - 1;
            std::iter_swap(mData.begin() + lastDataIndex, mData.begin() + dataIndexToRemove);
            mData.erase(mData.end() - 1, mData.end());

            // swaps handle data
            mHandles[dataIndex2handleIndex[lastDataIndex]].index = mHandles[handleIndex].index;
            dataIndex2handleIndex[mHandles[handleIndex].index] = dataIndex2handleIndex[lastDataIndex];
            mHandles[handleIndex].generation = 0;

            if (handleIndex == mHandles.size() - 1)
                mHandles.erase(mHandles.end() - 1, mHandles.end());
            else
                mFreeSlots.push(handleIndex);
        }

        void removeAll() {
            mHandles.clear();
            mFreeSlots = {};
            dataIndex2handleIndex.clear();

            mData.clear();
        }

        T& get(std::uint32_t handleIndex, std::uint32_t generation) {
            if (!isValid(handleIndex, generation))
                throw std::runtime_error("Access to an invalid or deleted handle");
            return mData[mHandles[handleIndex].index];
        }

        bool isValid(std::uint32_t handleIndex, std::uint32_t handleGeneration) {
            return (handleIndex < mHandles.size() && mHandles[handleIndex].generation == handleGeneration);
        }

		virtual ~HandleList() = default;
};

#endif // HANDLELIST_H
