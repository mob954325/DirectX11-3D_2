#pragma once
#include <memory>

/// @brief CRTP 패턴을 적용한 싱글톤 템플릿 클래스
/// @tparam T singleton으로 만들 클래스
template <typename T>
class Singleton
{
public:
    static T& Instance();
    
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton (Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
    
protected:
    Singleton() = default;
    ~Singleton() = default;
    struct token {};
};

template <typename T>
inline T& Singleton<T>::Instance()
{
    static const std::unique_ptr<T> instance{new T{token{}}};
    return *instance;
}
