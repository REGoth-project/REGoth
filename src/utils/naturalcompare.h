// author: markusobi (https://github.com/markusobi/Tiny-Natural-Compare)
#pragma once
#include <cctype>
#include <algorithm>
#include <locale>

namespace NaturalCompare
{
    template <class InputIt>
    struct StringView
    {
        std::size_t size() const {
            return end - begin;
        }

        bool empty() const
        {
            return size() == 0;
        }

        InputIt begin;
        InputIt end;
        // shall be false for empty ranges
        bool onlyDigits;
    };

    template <class InputIt1, class InputIt2>
    static bool stringViewless(StringView<InputIt1> a, StringView<InputIt2> b, bool stripLeadingZeros = true)
    {
        if (stripLeadingZeros && a.onlyDigits && b.onlyDigits)
        {
            while (a.begin < a.end - 1 && *a.begin == '0')
                ++a.begin;
            while (b.begin < b.end - 1 && *b.begin == '0')
                ++b.begin;
        }
        if (a.onlyDigits && b.onlyDigits && a.size() != b.size())
            return a.size() < b.size(); // less digits means: number must be smaller
        else
            return std::lexicographical_compare(a.begin, a.end, b.begin, b.end); // sorts also digit groups of equal size
    }

    template <class Iterator>
    StringView<Iterator> nextGroup(Iterator begin, Iterator end)
    {
        bool firstIsDigit = begin != end && std::isdigit(*begin);
        // finds first not of digit/non-digit
        auto groupEnd = std::find_if(begin, end, [firstIsDigit](const auto& value) -> bool {
            return firstIsDigit != std::isdigit(value, std::locale::classic());
        });
        return {begin, groupEnd, firstIsDigit};
    }

    /**
     * Counterpart to std::lexicographical_compare.
     * Instead of comparing character-wise, this function compares groups of characters.
     * Each sequence is split into groups of characters containing only digits or non-digits.
     * If two digit-groups are compared, they are ordered first by number of digits and then lexicographically.
     * If one of the groups is a non-digit group, they are ordered ASCIIbetical.
     * @returns true if the first sequence is less than the second one else false
     */
    template <class InputIt1, class InputIt2>
    bool lexicographical_compare(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2, bool stripLeadingZeros = true)
    {
        StringView<InputIt1> aView = {first1, first1, false};
        StringView<InputIt2> bView = {first2, first2, false};
        while (true)
        {
            aView = nextGroup(aView.end, last1);
            bView = nextGroup(bView.end, last2);
            if ((aView.empty() && bView.empty()) || stringViewless(bView, aView, stripLeadingZeros))
                return false;
            else if (stringViewless(aView, bView, stripLeadingZeros))
                return true;
        }
    }

    /**
     * Wrapper for lexicographical_compare for containers, that support .begin() .end()
     * @param stripLeadingZeros if false numbers will be ordered by number of digits first including leading zeros
     * @returns less-than comparison function, that takes two containers and returns true if left < right
     */
    template <class Container>
    auto less(bool stripLeadingZeros = true)
    {
        return [stripLeadingZeros](const Container& a, const Container& b){
            return lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), stripLeadingZeros);
        };
    }
}
