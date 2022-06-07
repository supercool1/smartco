#ifndef SMARTOR_SINGLETON_H_
#define SMARTOR_SINGLETON_H_

namespace smartco{
template<typename T>
class Singleton
{
public:
    static T& GetInstance()
    {
        static T instance;
        return instance;
    }

    Singleton(T&&) = delete;
    Singleton(const T&) = delete;
    void operator= (const T&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

}
#endif