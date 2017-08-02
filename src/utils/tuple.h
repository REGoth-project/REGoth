#pragma once
#include <tuple>

namespace Utils
{
    /**
     * @brief tuple-foreach. Based on http://stackoverflow.com/a/16387374
     */
    namespace tf
    {
        /**
         * @brief generate a compile-time list of values from 0 to N
         */
        template <int... Is>
        struct seq
        {
        };

        template <int N, int... Is>
        struct gen_seq : gen_seq<N - 1, N - 1, Is...>
        {
        };

        template <int... Is>
        struct gen_seq<0, Is...> : seq<Is...>
        {
        };

        template <typename T, typename F, int... Is>
        void _internal_for_each(T&& t, F f, seq<Is...>)
        {
            // Generate an array looking like this:
            // auto l = {(f1,0), (f2,0), ... , (fn,0)}

            // The ",0" in (fi, 0) are important to get the compiler to
            // be quiet about not knowing the size of the fi-call for
            // the initializer list: The actual value of the fi-call will
            // be discarded and only the rightmost one will be used,
            // making the initializer list ending up as a list of 0s
            // while actually calling all functions.
            auto l = {(f(std::get<Is>(t)), 0)...};
        }
    }

    /**
     * @brief Calls lambda-function f for each element in the tuple t
     */
    template <typename... Ts, typename F>
    void for_each_in_tuple(std::tuple<Ts...>& t, F f)
    {
        // Generate a list for all types of fields we have in the tuple
        tf::_internal_for_each(t, f, tf::gen_seq<sizeof...(Ts)>());
    }

    /**
     * Example:
     * auto t = std::make_tuple(1, 2, "test", 4.0f);
     * for_each_in_tuple(t, [](auto& v){v += 1;});
     */
}