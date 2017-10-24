//
// Created by Oleg on 10/7/17.
//

#ifndef EVOLUTION_PRIORITIZEDQUEUE_H
#define EVOLUTION_PRIORITIZEDQUEUE_H

#include <list>
#include <iterator>
#include <stdexcept>


template <typename T, typename priority_t>
class PrioritizedQueue
{
private:
    typedef std::pair<T, priority_t> item_desc_t;
    typedef std::list<item_desc_t> list_t;

public:
    enum ordering_policy_t
    {
        ORDERING_ASCENDING,
        ORDERING_DESCENDING
    };

    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T>
    {
        friend bool operator==(const Iterator& left, const Iterator& right)
        {
            return left.m_iter == right.m_iter;
        }

        friend bool operator!=(const Iterator& left, const Iterator& right)
        {
            return left.m_iter != right.m_iter;
        }

    public:
        explicit Iterator(typename list_t::iterator iter);
        Iterator(const Iterator& other);

        Iterator& operator++();
        Iterator& operator++(int);
        Iterator& operator--();
        Iterator& operator--(int);

        const T& operator*() const;
        T& operator*();

        const T* operator->() const;
        T* operator->();

    private:
        typename list_t::iterator m_iter;
    };

public:
    explicit PrioritizedQueue(ordering_policy_t ordering_policy = ORDERING_ASCENDING);

    void add(const T& item, priority_t priority);
    void update(const T& item, priority_t priority);
    void remove(const T& item);
    T pop_next();

    bool empty() const;

    Iterator begin();
    Iterator end();

private:
    bool has_better_priority(priority_t priority,
                             const item_desc_t& item_desc) const;

private:
    list_t m_items;
    ordering_policy_t m_ordering_policy;
};

template <typename T, typename priority_t>
PrioritizedQueue<T, priority_t>::Iterator::Iterator(typename list_t::iterator iter)
        : m_iter(iter)
{
}

template <typename T, typename priority_t>
PrioritizedQueue<T, priority_t>::Iterator::Iterator(const Iterator& other)
        : m_iter(other.m_iter)
{
}

template <typename T, typename priority_t>
typename PrioritizedQueue<T, priority_t>::Iterator&
PrioritizedQueue<T, priority_t>::Iterator::operator++()
{
    ++m_iter;
    return *this;
}

template <typename T, typename priority_t>
typename PrioritizedQueue<T, priority_t>::Iterator&
PrioritizedQueue<T, priority_t>::Iterator::operator++(int)
{
    Iterator temp(*this);
    ++m_iter;
    return temp;
}

template <typename T, typename priority_t>
typename PrioritizedQueue<T, priority_t>::Iterator&
PrioritizedQueue<T, priority_t>::Iterator::operator--()
{
    --m_iter;
    return *this;
}

template <typename T, typename priority_t>
typename PrioritizedQueue<T, priority_t>::Iterator&
PrioritizedQueue<T, priority_t>::Iterator::operator--(int)
{
    Iterator temp(*this);
    --m_iter;
    return temp;
}

template <typename T, typename priority_t>
const T& PrioritizedQueue<T, priority_t>::Iterator::operator*() const
{
    return m_iter->first;
}

template <typename T, typename priority_t>
T& PrioritizedQueue<T, priority_t>::Iterator::operator*()
{
    return m_iter->first;
}

template <typename T, typename priority_t>
const T* PrioritizedQueue<T, priority_t>::Iterator::operator->() const
{
    return &m_iter->first;
}

template <typename T, typename priority_t>
T* PrioritizedQueue<T, priority_t>::Iterator::operator->()
{
    return &m_iter->first;
}

template <typename T, typename priority_t>
PrioritizedQueue<T, priority_t>::PrioritizedQueue(ordering_policy_t ordering_policy)
        : m_ordering_policy(ordering_policy)
{
}

template <typename T, typename priority_t>
void PrioritizedQueue<T, priority_t>::add(const T& item, priority_t priority)
{
    auto iter = m_items.cbegin();
    for (; iter != m_items.cend(); ++iter)
    {
        if (has_better_priority(priority, *iter))
        {
            break;
        }
    }
    m_items.insert(iter, std::make_pair(item, priority));
}

template <typename T, typename priority_t>
void PrioritizedQueue<T, priority_t>::update(const T& item, priority_t priority)
{
    auto item_current_iter = m_items.end();
    auto iter = m_items.begin();
    for (; iter != m_items.end(); ++iter)
    {
        if (iter->first == item)
        {
            item_current_iter = iter;
            break;
        }
        if (has_better_priority(priority, *iter))
        {
            break;
        }
    }

    if (item_current_iter == m_items.end())
    {
        m_items.insert(iter, std::make_pair(item, priority));
        ++iter;

        for (; iter != m_items.end(); ++iter)
        {
            if (iter->first == item)
            {
                m_items.erase(iter);
                break;
            }
        }
    }
    else if (has_better_priority(priority, *item_current_iter))
    {
        item_current_iter->second = priority;
    }
}

template <typename T, typename priority_t>
void PrioritizedQueue<T, priority_t>::remove(const T& item)
{
    for (auto iter = m_items.begin(); iter != m_items.end(); ++iter)
    {
        if (iter->first == item)
        {
            m_items.erase(iter);
            break;
        }
    }
}

template <typename T, typename priority_t>
T PrioritizedQueue<T, priority_t>::pop_next()
{
    if (m_items.empty())
    {
        throw std::runtime_error("No items left to pop");
    }
    T item = m_items.front().first;
    m_items.pop_front();
    return item;
}

template <typename T, typename priority_t>
bool PrioritizedQueue<T, priority_t>::empty() const
{
    return m_items.empty();
}

template <typename T, typename priority_t>
typename PrioritizedQueue<T, priority_t>::Iterator
PrioritizedQueue<T, priority_t>::begin()
{
    return Iterator(m_items.begin());
}

template <typename T, typename priority_t>
typename PrioritizedQueue<T, priority_t>::Iterator
PrioritizedQueue<T, priority_t>::end()
{
    return Iterator(m_items.end());
}

template <typename T, typename priority_t>
bool PrioritizedQueue<T, priority_t>::has_better_priority(
        priority_t priority,
        const item_desc_t& item_desc) const
{
    if (m_ordering_policy == ORDERING_ASCENDING
        && priority < item_desc.second)
    {
        return true;
    }
    else if (m_ordering_policy == ORDERING_DESCENDING
             && priority > item_desc.second)
    {
        return true;
    }
    return false;
}

#endif //EVOLUTION_PRIORITIZEDQUEUE_H
