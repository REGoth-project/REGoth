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
}