#pragma once

// __cpp_lib_make_unique is not set in MSVC and old Clang
// __cplusplus is incorrect in MSVC
#if !defined(_MSC_VER) && __cplusplus < 201402L
namespace std {

template<typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, unique_ptr<T>>::type
make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename std::enable_if<std::is_array<T>::value, unique_ptr<T>>::type
make_unique(std::size_t size)
{
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[size]());
}

} // namespace std
#endif // __cpp_lib_make_unique
