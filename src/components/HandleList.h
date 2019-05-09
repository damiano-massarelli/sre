#ifndef HANDLELIST_H
#define HANDLELIST_H
#include <vector>
#include <queue>
#include <cstdint>
#include <stack>
#include <memory>

struct Handle {
    std::uint32_t index;
    std::uint32_t generation;
};

struct ExternalHandle {
    std::uint32_t mHandleIndex;
    std::uint32_t mGeneration;

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
        std::stack<std::uint32_t> mLastPositions;

        std::uint32_t mGeneration = 1;

    public:
        HandleList(std::vector<T>& data) : mData{data} {}

        void add(const T&& data, std::uint32_t& outHandleIndex, std::uint32_t& outHandleGeneration) {
            Handle h{static_cast<std::uint32_t>(mData.size()), mGeneration};
            mGeneration++;

            mData.push_back(data);

            std::uint32_t handleIndex;
            if (mFreeSlots.empty()) {
                handleIndex = mHandles.size();
                mHandles.push_back(h);
            }
            else {
                handleIndex = mFreeSlots.front();
                mFreeSlots.pop();
                mHandles[handleIndex] = h;
            }

            mLastPositions.push(handleIndex);

            outHandleIndex = handleIndex;
            outHandleGeneration = (mGeneration - 1);
        }

        void remove(std::uint32_t handleIndex, std::uint32_t handleGeneration) {
            if (!isValid(handleIndex, handleGeneration))
                throw std::runtime_error("Access to an invalid or deleted handle");

            std::uint32_t posOfLast = mLastPositions.top();
            mLastPositions.pop();

            // deletes data
            std::uint32_t dataIndexOfLast = mHandles[posOfLast].index;
            std::uint32_t dataIndexToRemove = mHandles[handleIndex].index;

            // swaps values
            std::iter_swap(mData.begin() + dataIndexOfLast, mData.begin() + dataIndexToRemove);
            mData.erase(mData.end() - 1, mData.end());

            // swaps handle data
            mHandles[posOfLast].index = mHandles[handleIndex].index;
            mHandles[handleIndex].generation = 0;

            if (handleIndex == mHandles.size() - 1)
                mHandles.erase(mHandles.end() - 1, mHandles.end());
            else
                mFreeSlots.push(handleIndex);
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
