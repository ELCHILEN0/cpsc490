#include <type_traits>
#include <memory>

#include "catch.hpp"

using namespace std;

/**
 * A reimplementation of the classic unique_ptr from the
 * C++ standard library.
 *
 * see here: http://en.cppreference.com/w/cpp/memory/unique_ptr
 *
 * Your implementation must match the official specification precisely,
 * except where noted. To avoid clutter, we will not be covering compatibility
 * with auto_ptr or usage with deleters.
 *
 * Notation will be closely related to the specification linked above, if
 * in doubt look at the relevant section there.
 *
 * Follow the C++17 versions of the spec points when in doubt, and watch out
 * for the template specialisation.
 */
template<typename T>
class my_unique_ptr{
private:
    T *ptr;

public:
    /**
     * Constructor.
     *
     * For simplicity, skip the following overloads 3, 4 and 7.
     * For overload 6, follow this instead:
     * template<class U> my_unique_ptr(my_unique_ptr<U>&& u) noexcept;
     */

    ~my_unique_ptr()
    {
        reset();
    }

    constexpr my_unique_ptr() noexcept
    : ptr(nullptr)
    { }
    constexpr my_unique_ptr( nullptr_t ) noexcept
    : ptr(nullptr)
    { }

    explicit my_unique_ptr( T* p ) noexcept 
    : ptr(p)
    { }

    my_unique_ptr( my_unique_ptr&& r ) noexcept
    : ptr(r.release())
    { }

    template<class U>
    my_unique_ptr(my_unique_ptr<U>&& r,
                    typename std::enable_if<
                        !is_array<U>::value &&
                        is_convertible<U, T>::value,
                    bool>::type = true) noexcept
    : ptr(r.release())
    { }

    /**
     * Do not implement get_deleter(), as we do not support them.
     */
		
    my_unique_ptr& operator=( my_unique_ptr&& r ) noexcept
    {
        reset(r.release());
        return *this;
    }

    template<class U>
    typename std::enable_if<
        !is_array<U>::value &&
        is_convertible<U, T>::value,
    my_unique_ptr&>::type 
    operator=( my_unique_ptr<U>&& r ) noexcept 
    {
        reset(r.release());
        return *this;
    }

    my_unique_ptr& operator=( nullptr_t ) noexcept
    {
        reset();
        return *this;
    }

    T* release() noexcept
    { 
        T* old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }

    void reset( T* p = nullptr ) noexcept 
    {
        T* old_ptr = ptr;
        ptr = p;
        if (old_ptr)
            delete old_ptr;
    }

    void swap(my_unique_ptr& other) noexcept
    {
        // p.swap(other.p);
        std::swap(ptr, other.ptr);
    }

    T* get() const noexcept
    {
        if (ptr)
            return ptr;

        return nullptr;
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    typename add_lvalue_reference<T>::type operator*() const 
    {
        return *ptr;
    }

    T* operator->() const noexcept
    {
        return ptr;
    }
};

/**
 * Specialisation for arrays goes here.
 */

template<typename T>
class my_unique_ptr<T[]> : my_unique_ptr<T>
{
private:
    T *ptr;

public:
    ~my_unique_ptr()
    {
        reset();
    }

    constexpr my_unique_ptr() noexcept
    : ptr(nullptr)
    { }
    constexpr my_unique_ptr( nullptr_t ) noexcept
    : ptr(nullptr)
    { }

    explicit my_unique_ptr( T* p ) noexcept 
    : ptr(p)
    { }

    template <class U>
    explicit my_unique_ptr(U p,
                    typename std::enable_if<
                        is_convertible<U, T>::value,
                    bool>::type = true) noexcept
    : ptr(p)
    { }

    my_unique_ptr( my_unique_ptr&& r ) noexcept
    : ptr(r.release())
    { }

    template<class U>
    my_unique_ptr(my_unique_ptr<U>&& r,
                    typename std::enable_if<
                        is_array<U>::value &&
                        is_convertible<U, T>::value,
                    bool>::type = true) noexcept
    : ptr(r.release())
    { }

    my_unique_ptr& operator=( my_unique_ptr&& r ) noexcept
    {
        reset(r.release());
        return *this;
    }

    template<class U>
    typename enable_if<
        is_array<U>::value &&
        is_convertible<U, T>::value,
    my_unique_ptr&>::type
    operator=( my_unique_ptr<U>&& r ) noexcept
    {
        reset(r.release());
        return *this;
    }

    my_unique_ptr& operator=( nullptr_t ) noexcept
    {
        reset();
        return *this;
    }

    template<class U> 
    my_unique_ptr( const U& u ) = delete;

    T* get() const noexcept
    {
        return ptr;
    }

    explicit operator bool() const noexcept
    {
        return ptr != nullptr;
    }

    T* release() noexcept
    {
        T* old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }

    template<class U>
    typename enable_if<
        is_array<U>::value &&   // TODO: correct?
        is_convertible<U, T>::value,
    void>::type
    reset(U p) noexcept
    {
        T* old_ptr = ptr;
        ptr = p;
        if (old_ptr)
            delete old_ptr;
    }

    void reset(nullptr_t = nullptr) noexcept
    {
        T* old_ptr = ptr;
        ptr = nullptr;
        if (old_ptr)
            delete old_ptr;
    }

    void swap(my_unique_ptr& other) noexcept
    {
        // p.swap(other.p);
        std::swap(ptr, other.ptr);
    }

    T& operator[](size_t i) const {
        return ptr[i];
    }
};


/**
 * Implementing make_unique is optional; you do not have to, but it
 * is interesting to do so if you want to go above and beyond.
 */

/*

template<class T, class... Args> unique_ptr<T> make_unique(Args&&... args);     // C++14
template<class T>                unique_ptr<T> make_unique(size_t n);           // C++14
template<class T, class... Args> unspecified   make_unique(Args&&...) = delete; // C++14, T == U[N]

*/

/**
 * You are not required to implement operator<< or std::hash.
 */

struct Foo 
{
    bool val = true;
    bool bar() { return val; }
};

bool f(const Foo& foo)
{
    return true;
}

struct Bar
{ };

bool f(const Bar& bar)
{
    return true;
}

struct FooBar : Foo, Bar { };

TEST_CASE("default constructor") {
    unique_ptr<int> ptr_u = unique_ptr<int>::unique_ptr();
    my_unique_ptr<int> ptr_m = my_unique_ptr<int>::my_unique_ptr();

    CHECK(ptr_u.get() == nullptr);
    CHECK(ptr_m.get() == nullptr);
}

TEST_CASE("nullptr constructor") {
    unique_ptr<int> ptr_u(nullptr);
    my_unique_ptr<int> ptr_m(nullptr);

    CHECK(ptr_u.get() == nullptr);
    CHECK(ptr_m.get() == nullptr);
}

TEST_CASE("new constructor") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    CHECK(ptr_u.get() != nullptr);
    CHECK(ptr_m.get() != nullptr);

    CHECK(ptr_u->val == ptr_m->val);
}

TEST_CASE("move constructor<T>") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    ptr_u->val = false;
    ptr_m->val = false;

    unique_ptr<Foo> ptr_u1(move(ptr_u));
    my_unique_ptr<Foo> ptr_m1(move(ptr_m));

    CHECK(ptr_u.get() == nullptr);
    CHECK(ptr_m.get() == nullptr);
    CHECK(ptr_u1.get() != nullptr);
    CHECK(ptr_m1.get() != nullptr);

    CHECK(ptr_u1->val == ptr_m1->val);
}

TEST_CASE("move constructor<U>") {
    unique_ptr<FooBar> ptr_u(new FooBar);
    my_unique_ptr<FooBar> ptr_m(new FooBar);

    unique_ptr<Foo> ptr_u1(move(ptr_u));
    my_unique_ptr<Foo> ptr_m1(move(ptr_m));

    CHECK(ptr_u.get() == ptr_m.get());
}

TEST_CASE("single-object operators") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    CHECK(ptr_u->val == ptr_m->val);

    ptr_u->val = false;
    ptr_m->val = false;

    CHECK(ptr_u->val == ptr_m->val);

    // TODO: *

    if (!ptr_u || !ptr_m)
        CHECK(false);

    ptr_u.release();
    ptr_m.release();

    if (ptr_u || ptr_m)
        CHECK(false);
}

TEST_CASE("swap") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    ptr_u->val = false;
    ptr_m->val = false;

    unique_ptr<Foo> ptr_u1(new Foo);
    my_unique_ptr<Foo> ptr_m1(new Foo);

    ptr_u.swap(ptr_u1);
    ptr_m.swap(ptr_m1);

    CHECK(ptr_u->val == ptr_m->val);
    CHECK(ptr_u1->val == ptr_m1->val);
}

TEST_CASE("assignment") {
    unique_ptr<FooBar> ptr_u(new FooBar);
    my_unique_ptr<FooBar> ptr_m(new FooBar);

    ptr_u->val = false;
    ptr_m->val = false;

    unique_ptr<FooBar> ptr_u1 = std::move(ptr_u);
    my_unique_ptr<FooBar> ptr_m1 = std::move(ptr_m);

    CHECK(ptr_m.get() == ptr_u.get());
    CHECK(ptr_m1.get()->val == ptr_u1.get()->val);

    unique_ptr<Foo> ptr_u2 = std::move(ptr_u1);
    my_unique_ptr<Foo> ptr_m2 = std::move(ptr_m1);

    CHECK(ptr_m1.get() == ptr_u1.get());
    CHECK(ptr_m2.get()->val == ptr_u2.get()->val);    

    ptr_u2 = nullptr;
    ptr_m2 = nullptr;

    CHECK(ptr_m2.get() == ptr_u2.get());
}

TEST_CASE("release") {
    Foo *f1 = new Foo();
    Foo *f2 = new Foo();

    unique_ptr<Foo> ptr_u(f1);
    my_unique_ptr<Foo> ptr_m(f2);

    CHECK(ptr_u.get() == f1);
    CHECK(ptr_m.get() == f2);

    Foo *r1 = ptr_u.release();
    Foo *r2 = ptr_m.release();

    CHECK(r1 == f1);
    CHECK(r2 == f2);

    CHECK(ptr_u.get() == ptr_m.get());
}

TEST_CASE("reset") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    CHECK(ptr_u.get() != nullptr);
    CHECK(ptr_m.get() != nullptr);

    ptr_u.reset();
    ptr_m.reset();

    CHECK(ptr_m.get() == ptr_u.get());
}

TEST_CASE("array") {
    // unique_ptr<Foo[]> ptr_u = unique_ptr<Foo[]>();
    // my_unique_ptr<Foo[]> ptr_m = my_unique_ptr<Foo[]>();

    unique_ptr<Foo[]> ptr_u(new Foo[2]);
    my_unique_ptr<Foo[]> ptr_m(new Foo[2]);

    CHECK(ptr_u[0].bar() == ptr_m[0].bar());
    CHECK(ptr_u[1].bar() == ptr_m[1].bar());

    unique_ptr<Foo[]> ptr_u1(nullptr); // Verify...
    my_unique_ptr<Foo[]> ptr_m1(nullptr); // Verify... should be Foo[]


    // unique_ptr<Foo[]> ptr_u2(new FooBar[2]);
    // my_unique_ptr<Foo[]> ptr_m2(new FooBar[2]);

    // ptr_m2[0];

    // TODO: How to check? both will fault
}