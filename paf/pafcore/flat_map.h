#pragma once

#include "utility.h"
#include "memory.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>
#include <stdexcept>

BEGIN_PAFCORE

template<class K, class T, class Pred = std::less<K>, class A = StlAllocator<std::pair<K, T> > >
class FlatMap : private std::vector<std::pair<K, T>, A>, private Pred
{
    typedef std::pair<K, T> ValueType;
    typedef std::vector<ValueType, A> Base;
public:
    typedef K key_type;
    typedef T mapped_type;
    typedef ValueType value_type;
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

    class value_compare
    {
        friend class FlatMap;
    protected:
        key_compare comp;
        value_compare(key_compare c) : comp(c) {}
    public:
        typedef bool result_type;
        typedef value_type first_argument_type;
        typedef value_type second_argument_type;
        bool operator()(const value_type& a, const value_type& b) const
        {
            return comp(a.first, b.first);
        }
    };

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

    FlatMap() :
        Base(), Pred()
    {
    }

    explicit FlatMap(const key_compare& comp, const allocator_type& alloc = allocator_type()) :
        Base(alloc), Pred(comp)
    {
    }

    explicit FlatMap(const allocator_type& alloc) :
        Base(alloc), Pred()
    {
    }

    template<class InputIt>
    FlatMap(InputIt first, InputIt last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :
        Base(first, last, alloc), Pred(comp)
    {
        sort_unique();
    }

    FlatMap(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type()) :
        Base(il, alloc), Pred(comp)
    {
        sort_unique();
    }

    FlatMap(const FlatMap& other) :
        Base(other), Pred(static_cast<const Pred&>(other))
    {
    }

    FlatMap(FlatMap&& other) :
        Base(std::move(other)), Pred(std::move(static_cast<Pred&>(other)))
    {
    }

    FlatMap& operator=(const FlatMap& other)
    {
        if (this != &other)
        {
            Base::operator=(other);
            static_cast<Pred&>(*this) = static_cast<const Pred&>(other);
        }
        return *this;
    }

    FlatMap& operator=(FlatMap&& other)
    {
        if (this != &other)
        {
            Base::operator=(std::move(other));
            static_cast<Pred&>(*this) = std::move(static_cast<Pred&>(other));
        }
        return *this;
    }

    ~FlatMap() = default;

    key_compare key_comp() const
    {
        return comp();
    }

    value_compare value_comp() const
    {
        return value_compare(comp());
    }

    iterator find(const key_type& k)
    {
        iterator it = lower_bound(k);
        if (it != end() && is_equivalent(it->first, k))
        {
            return it;
        }
        return end();
    }

    const_iterator find(const key_type& k) const
    {
        const_iterator it = lower_bound(k);
        if (it != end() && is_equivalent(it->first, k))
        {
            return it;
        }
        return end();
    }

    size_type count(const key_type& k) const
    {
        if (find(k) != end())
        {
            return 1;
        }
        return 0;
    }

    bool contains(const key_type& k) const
    {
        return find(k) != end();
    }

    iterator lower_bound(const key_type& k)
    {
        return std::lower_bound(begin(), end(), k, key_less(comp()));
    }

    const_iterator lower_bound(const key_type& k) const
    {
        return std::lower_bound(begin(), end(), k, key_less(comp()));
    }

    iterator upper_bound(const key_type& k)
    {
        return std::upper_bound(begin(), end(), k, key_less(comp()));
    }

    const_iterator upper_bound(const key_type& k) const
    {
        return std::upper_bound(begin(), end(), k, key_less(comp()));
    }

    _Pairii equal_range(const key_type& k)
    {
        return std::equal_range(begin(), end(), k, key_less(comp()));
    }

    _Paircc equal_range(const key_type& k) const
    {
        return std::equal_range(begin(), end(), k, key_less(comp()));
    }

    _Pairib insert(const value_type& x)
    {
        iterator it = lower_bound(x.first);
        if (it != end() && is_equivalent(it->first, x.first))
        {
            return _Pairib(it, false);
        }
        it = Base::insert(it, x);
        return _Pairib(it, true);
    }

    _Pairib insert(value_type&& x)
    {
        iterator it = lower_bound(x.first);
        if (it != end() && is_equivalent(it->first, x.first))
        {
            return _Pairib(it, false);
        }
        it = Base::insert(it, std::move(x));
        return _Pairib(it, true);
    }

    iterator insert(const_iterator hint, const value_type& x)
    {
        if (is_valid_hint(hint, x.first))
        {
            return Base::insert(hint, x);
        }
        return insert(x).first;
    }

    iterator insert(const_iterator hint, value_type&& x)
    {
        if (is_valid_hint(hint, x.first))
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

    void insert(std::initializer_list<value_type> il)
    {
        for (const value_type& x : il)
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

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args)
    {
        value_type tmp(std::forward<Args>(args)...);
        return insert(hint, std::move(tmp));
    }

    iterator erase(const_iterator it)
    {
        return Base::erase(it);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return Base::erase(first, last);
    }

    size_type erase(const key_type& k)
    {
        iterator it = find(k);
        if (it == end())
        {
            return 0;
        }
        Base::erase(it);
        return 1;
    }

    mapped_type& operator[](const key_type& k)
    {
        iterator it = lower_bound(k);
        if (it != end() && is_equivalent(it->first, k))
        {
            return it->second;
        }
        it = Base::insert(it, value_type(k, mapped_type()));
        return it->second;
    }

    mapped_type& operator[](key_type&& k)
    {
        iterator it = lower_bound(k);
        if (it != end() && is_equivalent(it->first, k))
        {
            return it->second;
        }
        it = Base::insert(it, value_type(std::move(k), mapped_type()));
        return it->second;
    }

    mapped_type& at(const key_type& k)
    {
        iterator it = find(k);
        if (it == end())
        {
            throw std::out_of_range("invalid key");
        }
        return it->second;
    }

    const mapped_type& at(const key_type& k) const
    {
        const_iterator it = find(k);
        if (it == end())
        {
            throw std::out_of_range("invalid key");
        }
        return it->second;
    }

    void swap(FlatMap& other)
        noexcept(noexcept(Base::swap(other)) &&
            noexcept(std::declval<Pred&>() = std::declval<Pred&&>()))
    {
        Base::swap(other);
        using std::swap;
        swap(comp(), other.comp());
    }

    friend bool operator==(const FlatMap& a, const FlatMap& b)
    {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

    friend bool operator!=(const FlatMap& a, const FlatMap& b)
    {
        return !(a == b);
    }

    friend bool operator<(const FlatMap& a, const FlatMap& b)
    {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    }

    friend bool operator>(const FlatMap& a, const FlatMap& b)
    {
        return b < a;
    }

    friend bool operator<=(const FlatMap& a, const FlatMap& b)
    {
        return !(b < a);
    }

    friend bool operator>=(const FlatMap& a, const FlatMap& b)
    {
        return !(a < b);
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

    struct key_less
    {
        const Pred& pred;
        explicit key_less(const Pred& p) : pred(p) {}
        bool operator()(const value_type& a, const key_type& b) const
        {
            return pred(a.first, b);
        }
        bool operator()(const key_type& a, const value_type& b) const
        {
            return pred(a, b.first);
        }
    };

    bool is_equivalent(const key_type& a, const key_type& b) const
    {
        return !comp()(a, b) && !comp()(b, a);
    }

    bool is_valid_hint(const_iterator hint, const key_type& k) const
    {
        const_iterator b = begin();
        const_iterator e = end();
        if (hint < b || hint > e)
        {
            return false;
        }
        if (hint != b && !comp()((hint - 1)->first, k))
        {
            return false;
        }
        if (hint != e && !comp()(k, hint->first))
        {
            return false;
        }
        return true;
    }

    void sort_unique()
    {
        std::sort(begin(), end(),
            [this](const value_type& a, const value_type& b)
            {
                return comp()(a.first, b.first);
            });
        iterator newEnd = std::unique(begin(), end(),
            [this](const value_type& a, const value_type& b)
            {
                return is_equivalent(a.first, b.first);
            });
        Base::erase(newEnd, end());
    }
};

template<class K, class T, class Pred, class A>
void swap(FlatMap<K, T, Pred, A>& lhs, FlatMap<K, T, Pred, A>& rhs)
noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

END_PAFCORE