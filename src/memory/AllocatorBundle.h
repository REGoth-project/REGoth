#pragma once
#include <algorithm>
#include <tuple>
#include "StaticReferencedAllocator.h"
#include "utils/tuple.h"

namespace Memory
{
    /**
     * Class wrapping different allocators into one. If an object is requested, all allocators
     * will create one. That means, that the handle given is valid on all of them.
     */
    template <int NUM_ALLOC, typename HT, typename... S>
    class AllocatorBundle
    {
    public:
        typedef HT Handle;

        /**
         * Structure to hold direct pointers to all of the datasets
         */
        struct DataBundle
        {
            std::tuple<S*...> m_Data;
            size_t m_NumElements;
        };

        AllocatorBundle()
            : m_NumObtainedObjects(0)
        {
        }

        ~AllocatorBundle() = default;

        /**
         * Returns a handle to a free chunk of memory and marks it as used
         */
        Handle createObject()
        {
            Handle h;
            h.index = Handle::INVALID_HANDLE;
            Utils::for_each_in_tuple(m_Allocators, [&](auto& alloc) {
                h = alloc.createObject();
            });

            m_NumObtainedObjects++;

            return h;
        }

        /**
         * @return Whether handle h is still active and valid
         */
        bool isHandleValid(const Handle& h)
        {
            bool invalid = false;
            Utils::for_each_in_tuple(m_Allocators, [&](auto& alloc) {
                if (!alloc.isHandleValid(h))
                    invalid = true;
            });

            return !invalid;
        }

        /**
         * @return the actual element to the handle h
         */
        template <typename T>
        T& getElement(const Handle& h)
        {
            return std::get<StaticReferencedAllocator<T, NUM_ALLOC>>(m_Allocators).getElement(h);
        }

        /**
         * Marks the element with the given handle as free and calls the "OnRemoved"-Callback before doing so
         */
        void removeObject(const Handle& h)
        {
            Utils::for_each_in_tuple(m_Allocators, [&](auto& alloc) {
                alloc.removeObject(h);
            });

            m_NumObtainedObjects--;
        }

        /**
         * Sets a callback to what should happen when an object of type T got deleted
         */
        template <typename T>
        void setOnRemoveCallback(const std::function<void(T&)> onRemoved)
        {
            getAllocator<T>().setOnRemoveCallback(onRemoved);
        }

        /**
         * Returns the array of elements created for the given type
         */
        template <typename T>
        T* getElements()
        {
            return getAllocator<T>().getElements();
        }

        /**
         * Returns the number of items obtained until now. This is also the range for the getElements()-Results
         */
        size_t getNumObtainedElements()
        {
            return m_NumObtainedObjects;
        }

        /**
         * Returns all datasets inside a single structure
         */
        DataBundle getDataBundle()
        {
            DataBundle db;
            db.m_Data = std::make_tuple(getElements<S>()...);
            db.m_NumElements = m_NumObtainedObjects;

            return db;
        }

    protected:
        /**
         * @return Allocator of the given type
         */
        template <typename T>
        auto& getAllocator()
        {
            return std::get<StaticReferencedAllocator<T, NUM_ALLOC>>(m_Allocators);
        }

        std::tuple<StaticReferencedAllocator<S, NUM_ALLOC>...> m_Allocators;

        /**
         * Duplicated count here for easier access. Would have to take one of the allocators value
         * for this otherwise, where access gets somewhat messy.
         */
        size_t m_NumObtainedObjects;
    };
}