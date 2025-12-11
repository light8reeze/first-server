#pragma once
#include <tuple>
#include <utility>

template <typename Tuple, typename Func, std::size_t... Is>
void for_each_tuple_impl(Tuple&& tuple, Func&& func, std::index_sequence<Is...>) {
    (func(std::get<Is>(tuple)), ...);
}

template <typename Tuple, typename Func>
void for_each_tuple(Tuple&& tuple, Func&& func) {
    constexpr std::size_t N = std::tuple_size<std::decay_t<Tuple>>::value;

    for_each_tuple_impl(
        std::forward<Tuple>(tuple),
        std::forward<Func>(func),
        std::make_index_sequence<N>{}
    );
}