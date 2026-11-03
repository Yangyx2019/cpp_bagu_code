#include <vector>
#include <cstdlib>
using namespace std;

class Solution {
public:
    vector<int> heapSort(vector<int>& heap, size_t startIdx, size_t endIdx) {
        // 1. 建堆
        for (int i = heap.size() - 1; i >= 0; --i)
        {
            heapify(heap, i, heap.size());
        }
        // 2. 排序
        for (int i = heap.size() - 1; i >= 0; --i)
        {
            std::swap(heap[0], heap[i]);
            heapify(heap, 0, i);
        }
        return heap;
    }

    void mergeSort(vector<int>& nums, size_t startIdx, size_t endIdx) {
        if (startIdx >= endIdx)
        {
            return;
        }
        size_t mid = (startIdx + endIdx) / 2;
        mergeSort(nums, startIdx, mid);
        mergeSort(nums, mid + 1, endIdx);
        // 合并两个有序数组
        size_t k = startIdx;
        size_t i = startIdx;
        size_t j = mid + 1;
        while (i <= mid && j <= endIdx)
        {
            if (nums[i] <= nums[j])
            {
                tmp_[k++] = nums[i];
                ++i;
            } else
            {
                tmp_[k++] = nums[j];
                ++j;
            }
        }
        // 这里容易写错成nums.size()
        while (i <= mid)
        {
            tmp_[k++] = nums[i];
            ++i;
        }
        // 这里容易写错成nums.size()
        while (j <= endIdx)
        {
            tmp_[k++] = nums[j];
            ++j;
        }

        for (size_t u = startIdx; u <= endIdx; ++u)
        {
            nums[u] = tmp_[u];
        }
    }

    void quickSort(vector<int>& nums, int startIdx, int endIdx) {
        if (endIdx <= startIdx)
        {
            return;
        }
        auto mid = partition(nums, startIdx, endIdx);
        quickSort(nums, startIdx, mid - 1);
        quickSort(nums, mid + 1, endIdx);
    }

    void stableQuickSort(vector<int>& nums, int startIdx, int endIdx) {
        if (startIdx >= endIdx)
        {
            return;
        }
        int randIdx = startIdx + rand() % (endIdx - startIdx + 1);
        int pivot = nums[randIdx];
        int k = startIdx;
        for (int i = startIdx; i <= endIdx; ++i)
        {
            if (nums[i] < pivot)
            {
                tmp_[k++] = nums[i];
            }
        }
        int mid1 = k - 1;
        for (int i = startIdx; i <= endIdx; ++i)
        {
            if (nums[i] == pivot)
            {
                tmp_[k++] = nums[i];
            }
        }
        int mid2 = k;
        for (int i = startIdx; i <= endIdx; ++i)
        {
            if (nums[i] > pivot)
            {
                tmp_[k++] = nums[i];
            }
        }
        for (int i = startIdx; i <= endIdx; ++i)
        {
            nums[i] = tmp_[i];
        }
        stableQuickSort(nums, startIdx, mid1);
        stableQuickSort(nums, mid2, endIdx);
    }

    vector<int> sortArray(vector<int>& nums) {
        tmp_ = nums;
        stableQuickSort(nums, 0, int(nums.size()) - 1);
        return nums;
    }
private:
    void heapify(vector<int>& heap, size_t node, size_t heap_len)
    {
        // node的左右孩子必须为根的树必须是大根堆
        auto p = node;
        while (2 * p + 1 < heap_len)
        {
            auto lChildIdx = 2 * p + 1;
            auto rChildIdx = lChildIdx + 1;
            auto biggerChildIdx = lChildIdx;
            if (rChildIdx >= heap_len || heap[lChildIdx] > heap[rChildIdx])
            {
                biggerChildIdx = lChildIdx;
            } else
            {
                biggerChildIdx = rChildIdx;
            }

            if (heap[biggerChildIdx] > heap[p])
            {
                std::swap(heap[biggerChildIdx], heap[p]);
                p = biggerChildIdx;
            } else
            {
                break;
            }
        }
    }

    int partition(vector<int>& nums, int startIdx, int endIdx) {
        int tmp = nums[startIdx];
        while (startIdx < endIdx)
        {
            while (startIdx < endIdx && nums[endIdx] >= tmp)
            {
                --endIdx;
            }
            nums[startIdx] = nums[endIdx];
            while (startIdx < endIdx && nums[startIdx] <= tmp)
            {
                ++startIdx;
            }
            nums[endIdx] = nums[startIdx];
        }
        nums[startIdx] = tmp;
        int mid = startIdx;
        return mid;
    }

    std::vector<int> tmp_;
};