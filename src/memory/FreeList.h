#pragma once
#include <stddef.h>

namespace Memory
{
/**
 * List supporting finding, obtaining and returning free list elements in O(1) time
 */
template <typename T>
class FreeList
{
public:
    FreeList(T* start, T* end, size_t elementSize, size_t numElements, size_t alignment, size_t offset)
    {
        // TODO: Implement alignment and offset

        union {
            T* as_T;
            char* as_char;
            FreeList* as_self;
        };

        // Assign start of the list to the union
        as_T = start;

        // First element is also the first free one
        m_Next = as_self;
        as_char += elementSize;

        // initialize the free list - make every m_next of each element point to the next element in the list
        FreeList* runner = m_Next;
        for (size_t i = 1; i < numElements; ++i)
        {
            runner->m_Next = as_self;
            runner = as_self;
            as_char += elementSize;
        }

        // Mark tail of the list
        runner->m_Next = nullptr;

        // All free
        m_NumObtainedElements = 0;
    }

    /**
         * Returns the next free element from the list
         */
    inline T* obtainElement()
    {
        if (!m_Next)
            return nullptr;  // Out of space

        FreeList* head = m_Next;
        m_Next = head->m_Next;

        m_NumObtainedElements++;

        return reinterpret_cast<T*>(head);
    }

    /**
         * Marks the given list-element as free again
         */
    inline void returnElement(T* ptr)
    {
        FreeList* newHead = reinterpret_cast<FreeList*>(ptr);
        newHead->m_Next = m_Next;
        m_Next = newHead;

        m_NumObtainedElements--;
    }

    /**
         * Returns how many elements where obtained until now
         */
    inline size_t getNumObtainedElements()
    {
        return m_NumObtainedElements;
    }

private:
    FreeList* m_Next;
    size_t m_NumObtainedElements;
};
}