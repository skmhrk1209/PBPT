#pragma once

#include <iostream>
#include <tuple>
#include <variant>

#include "algorithm.hpp"
#include "utility.hpp"

namespace coex {

template <typename... Ts>
decltype(auto) operator>>(std::istream &istream, std::tuple<Ts...> &tuple) {
    for_each(tuple, [&istream](auto &element) { istream >> element; });
    return istream;
}

template <typename... Ts>
decltype(auto) operator<<(std::ostream &ostream, const std::tuple<Ts...> &tuple) {
    ostream << "( ";
    for_each(tuple, [&ostream](const auto &element) { ostream << element << " "; });
    ostream << ")";
    return ostream;
}

template <typename T, typename U>
decltype(auto) operator>>(std::istream &istream, std::pair<T, U> &pair) {
    istream >> pair.first >> pair.second;
    return istream;
}

template <typename T, typename U>
decltype(auto) operator<<(std::ostream &ostream, const std::pair<T, U> &pair) {
    ostream << "( " << pair.first << " " << pair.second << " )";
    return ostream;
}

template <typename T, typename... Ts>
decltype(auto) operator>>(std::istream &istream, std::variant<T, Ts...> &variant) {
    std::visit([&istream](auto &value) { istream >> value; }, variant);
    return istream;
}

template <typename T, typename... Ts>
decltype(auto) operator<<(std::ostream &ostream, const std::variant<T, Ts...> &variant) {
    std::visit([&ostream](const auto &value) { ostream << value; }, variant);
    return ostream;
}

template <typename T>
decltype(auto) operator>>(std::istream &istream, T &iterable)
requires(coex::Iterable<T> && !coex::is_string_v<T>)
{
    for (auto &element : iterable) {
        istream >> element;
    }
    return istream;
}

template <typename T>
decltype(auto) operator<<(std::ostream &ostream, const T &iterable)
requires(coex::Iterable<T> && !coex::is_string_v<T>)
{
    ostream << "[ ";
    for (const auto &element : iterable) {
        ostream << element << " ";
    }
    ostream << "]";
    return ostream;
}

}  // namespace coex
