#pragma once
#include <type_traits>

namespace Lamb {
	template<class T>
	concept IsPtr = std::is_pointer_v<T>;

	template<class T>
	concept IsNotPtr = !std::is_pointer_v<T>;

	template<class T>
	concept IsContainer = requires(T a) {
		{ a.size() } -> std::convertible_to<std::size_t>;
		{ a.begin() } -> std::input_or_output_iterator;
		{ a.end() } -> std::input_or_output_iterator;
	};
}