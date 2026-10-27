#pragma once

#include "utility.h"
#include "memory.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>

BEGIN_PAFCORE

template<class T, class Pred = std::less<T>, class A = StlAllocator<T> >
class FlatSet : private std::vector<T, A>, private Pred
{
    typedef std::vector<T, A> Base;
public:
    typedef T key_type;
    typedef T value_type;
    typedef Pred key_compare;
    typedef A allocator_type;
    typedef typename Base::size_type size_type;
    typedef typename Base::difference_type difference_type;
    typedef typename Base::reference reference;
    typedef typename Base::const_reference const_reference;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;
    typedef typename Base::reverse_iterator reverse_iterator;
    typedef typename Base::const_reverse_iterator const_reverse_iterator;
    typedef std::pair<iterator, bool> _Pairib;
    typedef std::pair<iterator, iterator> _Pairii;
    typedef std::pair<const_iterator, const_iterator> _Paircc;

    using Base::begin;
    using Base::end;
    using Base::cbegin;
    using Base::cend;
    using Base::rbegin;
    using Base::rend;
    using Base::crbegin;
    using Base::crend;
    using Base::empty;
    using Base::size;
    using Base::max_size;
    using Base::capacity;
    using Base::reserve;
    using Base::shrink_to_fit;
    using Base::clear;
    using Base::get_allocator;

    FlatSet() : 
        Base(), Pred() 
    {}

    explicit FlatSet(const key_compare& comp, const allocator_type& alloc = allocator_type()):
        Base(alloc), Pred(comp) 
    {}

    explicit FlatSet(const allocator_type& alloc) :
        Base(alloc), Pred() 
    {}

    template<class InputIt>
    FlatSet(InputIt first, InputIt last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
        : Base(first, last, alloc), Pred(comp)
    {
        sort_unique();
    }

    FlatSet(std::initializer_list<T> il, const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type()) :
        Base(il, alloc), Pred(comp)
    {
        sort_unique();
    }

    FlatSet(const FlatSet& other) : 
        Base(other), Pred(static_cast<const Pred&>(other))
    {}

    FlatSet(FlatSet&& other) : 
        Base(std::move(other)), Pred(std::move(static_cast<Pred&>(other))) 
    {}

    FlatSet& operator=(const FlatSet& other)
    {
        if (this != &other)
        {
            Base::operator=(other);
            static_cast<Pred&>(*this) = static_cast<const Pred&>(other);
        }
        return *this;
    }

    FlatSet& operator=(FlatSet&& other)
    {
        if (this != &other)
        {
            Base::operator=(std::move(other));
            static_cast<Pred&>(*this) = std::move(static_cast<Pred&>(other));
        }
        return *this;
    }

    ~FlatSet() = default;

    key_compare key_comp() const
    {
        return comp();
    }

    iterator find(const T& t)
    {
        iterator it = lower_bound(t);
        return (it != end() && is_equivalent(*it, t)) ? it : end();
    }

    const_iterator find(const T& t) const
    {
        const_iterator it = lower_bound(t);
        return (it != end() && is_equivalent(*it, t)) ? it : end();
    }

    size_type count(const T& t) const
    {
        return find(t) != end() ? 1 : 0;
    }

    bool contains(const T& t) const
    {
        return find(t) != end();
    }

    iterator lower_bound(const T& t)
    {
        return std::lower_bound(begin(), end(), t, comp());
    }

    const_iterator lower_bound(const T& t) const
    {
        return std::lower_bound(begin(), end(), t, comp());
    }

    iterator upper_bound(const T& t)
    {
        return std::upper_bound(begin(), end(), t, comp());
    }

    const_iterator upper_bound(const T& t) const
    {
        return std::upper_bound(begin(), end(), t, comp());
    }

    _Pairii equal_range(const T& t)
    {
        return std::equal_range(begin(), end(), t, comp());
    }

    _Paircc equal_range(const T& t) const
    {
        return std::equal_range(begin(), end(), t, comp());
    }

    _Pairib insert(const value_type& x)
    {
        iterator it = lower_bound(x);
        if (it != end() && is_equivalent(*it, x))
        {
            return _Pairib(it, false);
        }
        it = Base::insert(it, x);
        return _Pairib(it, true);
    }

    _Pairib insert(value_type&& x)
    {
        iterator it = lower_bound(x);
        if (it != end() && is_equivalent(*it, x))
        {
            return _Pairib(it, false);
        }
        it = Base::insert(it, std::move(x));
        return _Pairib(it, true);
    }

    iterator insert(const_iterator hint, const value_type& x)
    {
        if (is_valid_hint(hint, x))
        {
            return Base::insert(hint, x);
        }
        return insert(x).first;
    }

    iterator insert(const_iterator hint, value_type&& x)
    {
        if (is_valid_hint(hint, x))
        {
            return Base::insert(hint, std::move(x));
        }
        return insert(std::move(x)).first;
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (; first != last; ++first)
        {
            insert(*first);
        }
    }

    void insert(std::initializer_list<T> il)
    {
        for (const T& x : il)
        {
            insert(x);
        }
    }

    template<class... Args>
    _Pairib emplace(Args&&... args)
    {
        value_type tmp(std::forward<Args>(args)...);
        return insert(std::move(tmp));
    }

    iterator erase(const_iterator it)
    {
        return Base::erase(it);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return Base::erase(first, last);
    }

    size_type erase(const T& t)
    {
        iterator it = find(t);
        if (it == end())
        {
            return 0;
        }
        Base::erase(it);
        return 1;
    }

    const_reference operator[](size_type n) const
    {
        PAF_ASSERT(n < size());
        return Base::operator[](n);
    }

    const_reference at(size_type n) const
    {
        return Base::at(n);
    }

    void swap(FlatSet& other)
        noexcept(noexcept(Base::swap(other)) &&
            noexcept(std::declval<Pred&>() = std::declval<Pred&&>()))
    {
        Base::swap(other);
        using std::swap;
        swap(comp(), other.comp());
    }

    friend bool operator==(const FlatSet& a, const FlatSet& b)
    {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

    friend bool operator!=(const FlatSet& a, const FlatSet& b)
    {
        return !(a == b);
    }

private:
    Pred& comp()
    {
        return static_cast<Pred&>(*this);
    }

    const Pred& comp() const
    {
        return static_cast<const Pred&>(*this);
    }

    bool is_equivalent(const T& lhs, const T& rhs) const
    {
        return !comp()(lhs, rhs) && !comp()(rhs, lhs);
    }

    bool is_valid_hint(const_iterator hint, const T& x) const
    {
        const_iterator b = begin();
        const_iterator e = end();
        if (hint < b || hint > e) return false;
        if (hint != b && !comp()(*(hint - 1), x)) return false;
        if (hint != e && !comp()(x, *hint)) return false;
        return true;
    }

    void sort_unique()
    {
        std::sort(begin(), end(), comp());
        iterator newEnd = std::unique(begin(), end(),
            [this](const T& a, const T& b) { return is_equivalent(a, b); });
        Base::erase(newEnd, end());
    }
};

template<class T, class Pred, class A>
void swap(FlatSet<T, Pred, A>& lhs, FlatSet<T, Pred, A>& rhs)
noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

END_PAFCORE