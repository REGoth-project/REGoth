#pragma once
#include "StaticReferencedAllocator.h"
#include <functional>

namespace Memory
{
    template<typename T, int NUM_OBJ>
    class ObjectAllocator
    {
    public:
        
		ObjectAllocator(const std::function<void(T*)> onDeleted)
			: m_OnDelete(onDeleted){}
    private:
		/**
		 * Actual memory allocation
		 */
        StaticReferencedAllocator<T, NUM_OBJ> m_Alloc;
        
        /**
         * @brief Function called when the memory of an object was free'd
		 */
        std::function<void(T*)> m_OnDelete;
    };
};