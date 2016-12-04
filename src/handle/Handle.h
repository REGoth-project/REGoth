#pragma once

namespace Handle
{
    /**
     * Handle-Type wrapper, so that every class can define what handle-type
     * it corresponds to
     */
    template <typename T>
    struct HandleTypeDescriptor
    {
        typedef T HandleType;
    };

    /**
     * Simple handle wrapping a pointer
     * @tparam T Type to wrap
     */
    template<typename T>
    struct PtrHandle
    {
        PtrHandle()
        {
            value = nullptr;
        }

        PtrHandle(T* _value)
        {
            value = _value;
        }

        bool isValid()
        {
            return value != nullptr;
        }

        void invalidate()
        {
            value = nullptr;
        }

        static PtrHandle makeInvalidHandle()
        {
            return PtrHandle();
        }

        T& get()
        {
            return *value;
        }

        bool operator==(const PtrHandle<T>& b)
        {
            return value == b.value;
        }

    private:
        T* value;
    };
}