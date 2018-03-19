#include "catch.hpp"
#include <memory>

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
        delete p;
        p = nullptr;
    }

    // 1)
    constexpr my_unique_ptr() noexcept : p(new T()) 
    { }
    constexpr my_unique_ptr( nullptr_t ) noexcept : p(nullptr) 
    { }

    // 2)
    explicit my_unique_ptr( T* p ) noexcept : p(p) 
    { }

    // 5)
    my_unique_ptr( my_unique_ptr&& u ) noexcept : p(std::move(u.p))
    {
        u.p = nullptr;        
    }

    // 6)
    template<class U>
    my_unique_ptr(my_unique_ptr<U>&& u) noexcept : p(std::move(u.p))
    { 
        u.p = nullptr;
    }

    template<class U> 
    my_unique_ptr( const U& u ) = delete;

    /**
     * Do not implement get_deleter(), as we do not support them.
     */
		
    my_unique_ptr& operator=( my_unique_ptr&& r ) noexcept
    {
        reset(r.release());
        return *this;
    }

    template< class U>
    my_unique_ptr& operator=( my_unique_ptr<U>&& r ) noexcept 
    {
        reset(r.release());
        return *this;
    }

    my_unique_ptr& operator=( nullptr_t ) noexcept
    {
        reset(nullptr); // TODO:
        return *this;
    }

    T* release() noexcept
    { 
        T* old_ptr = p;
        p = nullptr;
        return old_ptr;
    }

    void reset( T* ptr = new T() ) noexcept 
    {
        T* old_ptr = p;
        p = ptr;
        delete old_ptr;
    }

    void swap(my_unique_ptr& other) noexcept
    {
        std::swap(p, other.p);
    }

        T* get() const noexcept
    {
        if (p)
            return p;

        return nullptr;
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    typename add_lvalue_reference<T>::type operator*() const {
        // if (p)
        //     return *p;
        // return std::declval<T>();
        return *p;
    }

    T* operator->() const noexcept {
        return p;
    }

private:
    T* p;
};

/**
 * Specialisation for arrays goes here.
 */
/*
template<typename T>
class my_unique_ptr<T[]>
{
    constexpr my_unique_ptr() noexcept : p(new T[0])
    { }

    constexpr my_unique_ptr( nullptr_t ) noexcept : p(nullptr)
    { }

    explicit my_unique_ptr( T[] (*p) ) noexcept : p(p)
    { }

    // template<class U> explicit my_unique_ptr( U p ) noexcept;

    // my_unique_ptr( unique_ptr&& u ) noexcept;

    // template< class U, class E >
    // my_unique_ptr( unique_ptr<U, E>&& u ) noexcept;

    
    // The template version of this assignment operator in the specialization for arrays, std::unique_ptr<T[]> behaves the same as in the primary template, except that will only participate in overload resolution if all of the following is true:
    // * U is an array type
    // * pointer is the same type as element_type*
    // * unique_ptr<U,E>::pointer is the same type as unique_ptr<U,E>::element_type*
    // * unique_ptr<U,E>::element_type(*)[] is convertible to element_type(*)[]
    
    my_unique_ptr& operator=( my_unique_ptr&& r ) noexcept
    {

    }

    template< class U, class E >
    my_unique_ptr& operator=( my_unique_ptr<U,E>&& r ) noexcept
    {

    }

    my_unique_ptr& operator=( nullptr_t ) noexcept
    {

    }

    void reset( T* ptr = new T[0] ) noexcept {
        T* old_ptr = p;
        p = ptr;
        delete old_ptr;
    }

    
    // 3) In the specialization for dynamic arrays, std::unique_ptr<T[]>, this template member is provided to prevent using reset() with a pointer to derived (which would result in undefined behavior with arrays).
    // 3) Behaves the same as the reset member of the primary template, except that it will only participate in overload resolution if either
    //     U is the same type as pointer, or
    //     pointer is the same type as element_type* and U is a pointer type V* such that V(*)[] is convertible to element_type(*)[].
    

    template< class U > 
    void reset( U ) = delete;

    template< class U > 
    void reset( U ) noexcept;

    void reset( std::nullptr_t p ) noexcept
    {
        reset(new T[0]);
    }

    void reset( std::nullptr_t p = nullptr ) noexcept
    {
        reset(new T[0]);
    }

    T& operator[](size_t i) const {
        return this->get()[i];
    }
};

*/

/**
 * Implementing make_unique is optional; you do not have to, but it
 * is interesting to do so if you want to go above and beyond.
 */

/**
 * You are not required to implement operator<< or std::hash.
 */

struct Foo {
    bool ret = true;
    bool bar() { return ret; }
};

bool f(const Foo& foo)
{
    return true;
}

TEST_CASE("default constructor") {
    unique_ptr<int> i = unique_ptr<int>::unique_ptr();
    my_unique_ptr<int> j = my_unique_ptr<int>::my_unique_ptr();

    *i;
    *j;
}

TEST_CASE("nullptr constructor") {
    unique_ptr<int> i = unique_ptr<int>::unique_ptr(nullptr);
    my_unique_ptr<int> j = my_unique_ptr<int>::my_unique_ptr(nullptr);

    *i;
    *j;
}

TEST_CASE("new constructor") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    CHECK(ptr_u->bar());
    CHECK(ptr_m->bar());

    CHECK(f(*ptr_u));
    CHECK(f(*ptr_m));
}

TEST_CASE("move constructor") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    ptr_u->ret = false;
    ptr_m->ret = false;

    unique_ptr<Foo> ptr_u1 = unique_ptr<Foo>::unique_ptr(move(ptr_u));
    my_unique_ptr<Foo> ptr_m1 = my_unique_ptr<Foo>::my_unique_ptr(move(ptr_m));

    CHECK(ptr_u1.get() != nullptr);
    CHECK(ptr_m1.get() != nullptr);
    CHECK(ptr_m1->ret == ptr_u1->ret);
    CHECK(ptr_m1->bar() == ptr_u1->bar());
}

TEST_CASE("assignment") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    ptr_u->ret = false;
    ptr_m->ret = false;

    unique_ptr<Foo> ptr_u1 = std::move(ptr_u);
    my_unique_ptr<Foo> ptr_m1 = std::move(ptr_m);

    CHECK(ptr_m1->bar() == ptr_u1->bar());

    CHECK(ptr_m.get() == ptr_u.get());
    CHECK(ptr_m1.get()->ret == ptr_u1.get()->ret);

    ptr_u1 = nullptr;
    ptr_m1 = nullptr;

    CHECK(ptr_m1.get() == ptr_u1.get());
}

TEST_CASE("swap") {
    unique_ptr<Foo> ptr_u(new Foo);
    my_unique_ptr<Foo> ptr_m(new Foo);

    ptr_u->ret = false;
    ptr_m->ret = false;

    unique_ptr<Foo> ptr_u1(new Foo);
    my_unique_ptr<Foo> ptr_m1(new Foo);

    ptr_u.swap(ptr_u1);
    ptr_m.swap(ptr_m1);

    CHECK(ptr_u->ret == ptr_m->ret);
    CHECK(ptr_u1->ret == ptr_m1->ret);
}