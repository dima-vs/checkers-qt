#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdexcept> // Для std::runtime_error
#include <iostream> // Для std::cerr

namespace Structures
{
    template <class T>
    class LinkedList
    {
    private:
        struct Node
        {
            T* valuePtr;
            Node* next;
        };

        int size;
        Node* first;
        Node* last;

        // iteration
        Node* current;
        int currentIndex;
    public:
        LinkedList() : size(0), first(nullptr), last(nullptr),
            current(nullptr), currentIndex(0) {}

        void append(T* valuePtr)
        {
            ++size;
            if (!first)
            {
                first = new Node{ valuePtr, nullptr };
                last = first;
                first->next = nullptr;
                return;
            }

            Node* temp = new Node;
            temp->valuePtr = valuePtr;
            temp->next = nullptr;

            last->next = temp;
            last = temp;
        }

        void prepend(T* valuePtr)
        {
            ++size;
            if (!first)
            {
                first = new Node{ valuePtr, nullptr };
                last = first;
                first->next = nullptr;
                return;
            }

            Node* temp = new Node;
            temp->valuePtr = valuePtr;
            temp->next = first;
            first = temp;
        }

        bool isEmpty() const { return first == nullptr; }
        bool exists(const T& value) const
        {
            if (isEmpty())
                return false;

            Node* current = first;
            while (current)
            {
                if (*(current->valuePtr) == value)
                    return true;
                current = current->next;
            }
            return false;
        }

        void forEach(void (*callback)(T* valuePtr)) const
        {
            Node* current = first;
            while (current)
            {
                callback(current->valuePtr);
                current = current->next;
            }
        }

        void start()
        {
            current = nullptr;
            currentIndex = -1;
        }

        bool next()
        {
            if (currentIndex == -1)
            {
                if (!first)
                    return false;

                current = first;
                ++currentIndex;
                return true;
            }

            if (!current)
                return false;

            current = current->next;
            if (current)
            {
                ++currentIndex;
                return true;
            }
            else
                return false;
        }

        T* getCurrent() const
        {
            return current ? current->valuePtr : nullptr;
        }

        int getCurrentIndex() const
        {
            return currentIndex;
        }

        int getSize() const { return size; }

        T getFirst() const { return *(first->valuePtr); }
        T getLast() const { return *(last->valuePtr); }

        T* getFirstPtr() const { return first->valuePtr; }
        T* getLastPtr() const { return last->valuePtr; }

        void removeFirst()
        {
            if (!size)
                return;

            Node* second = first->next;
            delete first->valuePtr;
            delete first;

            if (size == 1)
            {
                first = last = nullptr;
            }
            else
            {
                first = second;
            }
            --size;
        }

        ~LinkedList()
        {
            Node* current = first;
            while (current != nullptr)
            {
                Node* next = current->next;
                delete current->valuePtr;
                delete current;
                current = next;
            }
        }
    };

    template <class T>
    class Stack
    {
    private:
        LinkedList<T> stack;
    public:
        void push(T* valuePtr)
        {
            stack.prepend(valuePtr);
        }

        T pop()
        {
            if (stack.getSize() == 0)
            {
                std::cerr << "ERROR 'Structures.h': Stack is empty";
                throw std::runtime_error("Stack is empty");
            }

            T copy = stack.getFirst();
            stack.removeFirst();
            return copy;
        }

        bool isEmpty() const { return stack.isEmpty(); }
        bool exists(T value) const { return stack.exists(value); }
        int getSize() const
        {
            return stack.getSize();
        }

        void forEach(void (*callback)(T* valuePtr)) const
        {
            stack.forEach(callback);
        }

        void start() { stack.start(); }
        bool next() { return stack.next(); }
        T getCurrent() { return *(stack.getCurrent()); }
        // перший елемент списку - це останній елемент стеку, тому індексація в порядку спадання
        int getCurrentIndex() { return stack.getSize() - stack.getCurrentIndex() - 1; }
    };
}

#endif