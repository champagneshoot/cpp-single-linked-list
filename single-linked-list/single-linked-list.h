#pragma once

#include <algorithm>
//#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList
{
    struct Node
    {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val), next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator
    {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) :node_(node) {}
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
        {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
        {
            return this->node_ == rhs.node_;

        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        // Оператор прединкремента. 
        BasicIterator& operator++() noexcept
        {
            if (node_)
            {
                node_ = node_->next_node;
            }
            return *this;
        }

        // Оператор постинкремента.
        BasicIterator operator++(int) noexcept
        {
            auto old_value(*this);
            ++(*this);
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept
        {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept
        {
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    SingleLinkedList(std::initializer_list<Type> values) :size_(0)
    {
        Node* current = &head_;
        for (const auto& val : values)
        {
            current->next_node = new Node(val, nullptr);
            current = current->next_node;
            ++size_;
        }
    }
    SingleLinkedList(const SingleLinkedList& other) :size_(0)
    {
        assert(size_ == 0 && head_.next_node == nullptr);

        SingleLinkedList tmp;
        Node** head = &tmp.head_.next_node;
        for (auto it = other.begin(); it != other.end(); it++)
        {
            *head = new Node(*it, nullptr);
            head = &((*head)->next_node);
            ++tmp.size_;
        }
        this->swap(tmp);
    }
    SingleLinkedList& operator=(const SingleLinkedList& rhs)
    {

        if (this != &rhs)
        {
            SingleLinkedList cur(rhs);
            this->swap(cur);
        }
        return *this;
    }
    void swap(SingleLinkedList& other) noexcept
    {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }
    SingleLinkedList() : size_(0) {};
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    void PushFront(const Type& value)
    {
        head_.next_node = new Node(value, head_.next_node);
        size_++;
    }
    void Clear() noexcept
    {
        Node* cur = head_.next_node;
        while (cur != nullptr)
        {
            Node* next = cur->next_node;
            delete cur;
            cur = next;
        }
        head_.next_node = nullptr;
        size_ = 0;
    }

    [[nodiscard]] size_t GetSize() const noexcept { return size_; }

    [[nodiscard]] bool IsEmpty() const noexcept { return size_ == 0; }

    // Итератор, допускающий изменение элементов списка
    using Iterator = BasicIterator<Type>;
    // Константный итератор, предоставляющий доступ для чтения к элементам списка
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept
    {
        if (size_ == 0)
            return end();
        return Iterator{ head_.next_node };

    }

    [[nodiscard]] Iterator end() noexcept
    {
        return Iterator{ nullptr };
    }

    [[nodiscard]] ConstIterator begin() const noexcept
    {
        if (size_ == 0)
            return end();
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator end() const noexcept
    {
        return ConstIterator{ nullptr };
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        if (size_ == 0)
            return cend();
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept
    {
        return ConstIterator{ nullptr };
    }


    [[nodiscard]] Iterator before_begin() noexcept
    {
        return Iterator{ &head_ };
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept
    {
        return ConstIterator{ const_cast<Node*>(&head_) };
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept
    {
        return ConstIterator{ const_cast<Node*>(&head_) };
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value)
    {
        if (pos == before_begin()) 
        {
            Node* new_node = new Node(value, head_.next_node);
            head_.next_node = new_node;
            ++size_;
            return Iterator{ new_node };
        }
        else {
            Node* node_to_insert = new Node(value, pos.node_->next_node);
            pos.node_->next_node = node_to_insert;
            ++size_;
            return Iterator{ node_to_insert };
        }
    }

    void PopFront() noexcept
    {
        if (size_ != 0)
        {
            Node* cur = head_.next_node;
            Node* next = cur->next_node;
            delete cur;
            head_.next_node = next;
            --size_;
        }
    }

    Iterator EraseAfter(ConstIterator pos) noexcept
    {
        if (pos == cbefore_begin()) 
        {
            if (head_.next_node) 
            {
                Node* node_to_delete = head_.next_node;
                head_.next_node = node_to_delete->next_node;
                delete node_to_delete;
                --size_;
                return Iterator{ head_.next_node };
            }
        }
        else 
        {
            Node* node_to_delete = pos.node_->next_node;
            if (node_to_delete) 
            {
                pos.node_->next_node = node_to_delete->next_node;
                delete node_to_delete;
                --size_;
                return Iterator{ pos.node_->next_node };
            }
        }
        return end();
    }

    ~SingleLinkedList() { Clear(); };

private:
    Node head_;
    size_t size_ = 0;
};
template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()) or std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs < rhs);
}
