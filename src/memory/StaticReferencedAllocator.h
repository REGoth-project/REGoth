#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include "FreeList.h"
#include "MemUtils.h"
#include <cassert>

namespace Memory
{
    /**
     * Max size a GenericHandle can be in bits
     */
    enum
    {
        GENERIC_HANDLE_MAX_SIZE_BITS = sizeof(uint32_t) * 8 * 2
    };

    /**
     * Note: Dif is only used to ensure type-safety! 
     */
    template <int N1, int N2, int dif = 0>
    struct GenericHandle
    {
        GenericHandle()
        {
            invalidate();
        }

        uint32_t index : N1;
        uint32_t generation : N2;

        enum
        {
            INVALID_HANDLE = (1U << N1) - 1,
            RESERVED = dif
        };

        inline void invalidate()
        {
            index = INVALID_HANDLE;
            generation = 0;
        }

        inline bool isValid()
        {
            return index != INVALID_HANDLE;
        }

        static GenericHandle<N1, N2, dif> makeInvalidHandle()
        {
            GenericHandle<N1, N2, dif> h;
            h.invalidate();

            return h;
        }

        bool operator<(const GenericHandle<N1, N2, dif>& r) const
        {
            return index < r.index;
        }

        bool operator==(const GenericHandle<N1, N2, dif>& r) const
        {
            return index == r.index && generation == r.generation;
        }

        bool operator!=(const GenericHandle<N1, N2, dif>& r) const
        {
            return index != r.index || generation != r.generation;
        }
    };

    /**
     * @param T Type of data stored in the allocator
     * @param NUM Number of elements statically allocated
     */
    template <typename T, unsigned int NUM>
    class StaticReferencedAllocator
    {
    public:
        /**
         * Outside-Mirror for the type this can create
         */
        typedef T Type;

        StaticReferencedAllocator()
            : m_Elements(new T[NUM])
            , m_ElementsToInternalHandles(new size_t[NUM])
            , m_InternalHandles(new FLHandle[NUM])
            , m_FreeList(m_InternalHandles, m_InternalHandles + NUM, sizeof(m_InternalHandles[0]), NUM, sizeof(m_InternalHandles[0]), 0)
            , m_LastInternalHandle(nullptr)
        {
            // Initialize handles
            for (size_t i = 0; i < NUM; i++)
            {
                m_InternalHandles[i].m_Handle.invalidate();
            }
        }

        virtual ~StaticReferencedAllocator()
        {
            kill();
        }

        /**
         * Returns a handle to a free chunk of memory and marks it as used
         */
        typename T::HandleType createObject()
        {
            assert(m_FreeList.getNumObtainedElements() != NUM);

            // Use the element at the end of the array as target
            size_t idx = m_FreeList.getNumObtainedElements();

            // Get a new handle from the free-list
            auto* handle = m_FreeList.obtainElement();
            handle->m_Handle.index = static_cast<uint32_t>(idx);  // TODO: Care for bit-size of 'index'

            // We're modifying this... Bump the generation;
            handle->m_Handle.generation++;

            // Store this as the new handle to the end of the list
            m_LastInternalHandle = handle;

            // Create output handle
            typename T::HandleType hOut;
            hOut.index = static_cast<uint32_t>(handle - m_InternalHandles);
            hOut.generation = handle->m_Handle.generation;

            // Connect element and internal handle
            m_ElementsToInternalHandles[idx] = hOut.index;

            // Call constructor on object
            //new (&reinterpret_cast<T*>(m_Elements)[idx]) T;

            return hOut;
        }

        /**
         * @return the actual element to the handle h (Checks generation)
         */
        T& getElement(const typename T::HandleType& h)
        {
            assert(m_InternalHandles[h.index].m_Handle.generation == h.generation);

            return reinterpret_cast<T*>(m_Elements)[m_InternalHandles[h.index].m_Handle.index];
        }

        /**
         * @return Whether handle h is still active and valid
         */
        bool isHandleValid(const typename T::HandleType& h)
        {
            return h.index < NUM && m_InternalHandles[h.index].m_Handle.generation == h.generation;
        }

        /**
         * @return the actual element to the handle h (Does not check generation)
         */
        T& getElementForce(const typename T::HandleType& h)
        {
            return reinterpret_cast<T*>(m_Elements)[m_InternalHandles[h.index].m_Handle.index];
        }

        /**
         * Marks the element with the given handle as free and calls the "OnRemoved"-Callback before doing so
         */
        void removeObject(const typename T::HandleType& h)
        {
            // Check if the handle is still valid. If not, we are accessing a different object!
            assert(m_InternalHandles[h.index].m_Handle.generation == h.generation);
            assert(m_LastInternalHandle != nullptr);  // Must have at least one handle in there

            // We're modifying this... Bump the generation;
            m_InternalHandles[h.index].m_Handle.generation++;

            // Get actual index of handle-target
            uint32_t actIdx = m_InternalHandles[h.index].m_Handle.index;

            if (m_OnRemoved)
                m_OnRemoved(reinterpret_cast<T*>(m_Elements)[actIdx]);

            // Call destructor on slot to free up memory
            //reinterpret_cast<T*>(m_Elements)[actIdx].~T(); // FIXME: This leaks right here! I need to fix the moving of the last element first!

            // Overwrite this element with the last one.
            //memcpy(&reinterpret_cast<T*>(m_Elements)[actIdx], &reinterpret_cast<T*>(m_Elements)[m_LastInternalHandle->m_Handle.index], sizeof(T));
            reinterpret_cast<T*>(m_Elements)[actIdx] = reinterpret_cast<T*>(m_Elements)[m_LastInternalHandle->m_Handle.index];

            // Fix the handle of the last element
            m_LastInternalHandle->m_Handle.index = actIdx;
            m_ElementsToInternalHandles[actIdx] = m_LastInternalHandle - m_InternalHandles;

            // Return the handle to the free-list
            m_FreeList.returnElement(&m_InternalHandles[h.index]);

            // Get new back-object
            if (m_FreeList.getNumObtainedElements() > 0)
                m_LastInternalHandle = &m_InternalHandles[m_ElementsToInternalHandles[m_FreeList.getNumObtainedElements() - 1]];
            else
                m_LastInternalHandle = nullptr;
        }

        /**
         * Sets a callback to what should happen when an object got deleted
         */
        void setOnRemoveCallback(const std::function<void(T&)> onRemoved)
        {
            m_OnRemoved = onRemoved;
        }

        /**
         * Returns all elements as continuous chunk of memory
         */
        T* getElements()
        {
            return reinterpret_cast<T*>(m_Elements);
        }

        /**
         * Returns the number of allocated elements, aka. how far to go using getElements()
         */
        size_t getNumObtainedElements()
        {
            return m_FreeList.getNumObtainedElements();
        }

        /**
         * Basically destructs the allocator and makes it unusable (frees memory)
         */
        void kill()
        {
            delete[] m_Elements;
            m_Elements = nullptr;
            delete[] m_InternalHandles;
            m_InternalHandles = nullptr;
            delete[] m_ElementsToInternalHandles;
            m_ElementsToInternalHandles = nullptr;
        }

    private:
        /** Actual element data */
        T* m_Elements;

        /** Contains the index of an internal handle for each element */
        size_t* m_ElementsToInternalHandles;

        /** Helper-struct to get around FreeList needing at least the size of a pointer to operate */
        struct FLHandle
        {
            void* m_Next;  // Don't let the free-list overwrite our generation
            typename T::HandleType m_Handle;
        };

        /** Make handles with enough bits to hold NUM indices. Use the rest for generations. */
        FLHandle* m_InternalHandles;

        /** Handle to the last element created */
        FLHandle* m_LastInternalHandle;

        /** List of free handles */
        FreeList<FLHandle> m_FreeList;

        /** Function to call when an object was removed */
        std::function<void(T&)> m_OnRemoved;
    };
}