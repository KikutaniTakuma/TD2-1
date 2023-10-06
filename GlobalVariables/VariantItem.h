#pragma once

#include <string>

///
///  ↓ 使用例
/// VariantItem<float> hoge{ "jsonのキー", 0.125f };
/// 


/// @brief 文字列と任意の変数をセットで保存出来るクラス
/// @tparam T 保存する型
template <typename T>
class VariantItem final {
	VariantItem() = delete;
	VariantItem &operator= (const VariantItem &other) = delete;
public:
	VariantItem(const std::string &key, const T &item = {}) : key_(key), item_(item) {}
	VariantItem(const VariantItem &other) : key_(other.key_), item_(other.item_) {}

	// template<typename U>
	// VariantItem &operator= (const VariantItem<U> &other) { key_ = other.key_; item = other.item_; }
	~VariantItem() = default;

	inline operator const T &() const noexcept;
	template <typename U>
	inline operator VariantItem<U>() const noexcept;
	inline VariantItem &operator=(const T &item);


	//inline T operator+() const { return item_; }
	inline T operator+(const T &other) const { return item_ + other; }
	inline T operator-() const { return -item_; }

	inline T *const operator->() { return &item_; }
	inline const T *const operator->() const { return &item_; }

	const std::string &GetKey() const { return key_; }
	T &GetItem() { return item_; }
	const T &GetItem() const { return item_; }


private:
	const std::string key_;
	T item_;
};

template <typename T>
inline VariantItem<T> &VariantItem<T>::operator=(const T &item) {
	item_ = item;
	return *this;
}

template <typename T>
inline VariantItem<T>::operator const T &() const noexcept {
	return item_;
}
//
//template <typename T, typename U>
//inline T operator+ (const VariantItem<T> &first, const U &second) {
//	return first.GetItem() + second;
//}
//
//template <typename T, typename U>
//inline T operator- (const VariantItem<T> &first, const U &second) {
//	return first.GetItem() - second;
//}

template<typename T>
template<typename U>
inline VariantItem<T>::operator VariantItem<U>() const noexcept {
	return VariantItem<U>{key_, static_cast<U>(item_)};
}

