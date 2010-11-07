/* Author: Vladimir Belikov
 *
 * This file is part of Fractal Library.
 *
 * Fractal Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Fractal Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Fractal Libray.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <stddef.h>
#include <stdlib.h>


#define FL_DEBUG_MEMORY

/*! \class MemoryMap
  * \brief Map of memory
  */
class MemoryMap
{
private:
    struct Node
    {
        void *value;
        size_t size;
        Node *next;
    };

    Node *first;
public:
    MemoryMap()
    {
        first = NULL;
    }

    ~MemoryMap()
    {
        clear();
    }

    void clear()
    {
        while (first != NULL)
        {
            Node *node = first;
            first = first->next;
            free(node);
        }
    }

    size_t operator[](void* value)
    {
        for (Node *node = first; node != NULL; node = node->next)
            if (node->value == value)
                return node->size;
        return -1;
    }

    void add(void *value, size_t size)
    {
        if (first == NULL)
            first = newNode(value, size);
        else
        {
            Node *node = first;
            while (node->next != NULL)
                node = node->next;
            node->next = newNode(value, size);
        }
    }
private:
    Node* newNode(void *value, size_t size)
    {
        Node *result = (Node*)malloc(sizeof(Node));
        result->value = value;
        result->size = size;
        result->next = NULL;
        return result;
    }
};


/*! \class MemoryManager
  * \brief Debuging memory manager
  *
  */
class MemoryManager
{
private:
    static size_t m_bytesAllocated;
    static MemoryMap m_memoryMap;
private:
    MemoryManager() {}

    MemoryManager(const MemoryManager& m) {}

    ~MemoryManager() {}

    static void* allocate(size_t bytes)
    {
        m_bytesAllocated += bytes;
        void *result = malloc(bytes);
        m_memoryMap.add(result, bytes);
        return result;
    }

    static void freed(void *space, size_t bytes)
    {
        m_bytesAllocated -= bytes;
        free(space);
    }

    static void freed(void *space)
    {
        size_t size = m_memoryMap[space];
        if ((int)size != -1)
            m_bytesAllocated -= size;
        free(space);
    }

public:

    static bool isDebugEnabled()
    {
        #ifdef FL_DEBUG_MEMORY
        return true;
        #else
        return false;
        #endif
    }

    static size_t bytesAllocated()
    {
        return m_bytesAllocated;
    }


    friend void* operator new(size_t bytes);
    friend void operator delete(void *space, size_t bytes);
    friend void operator delete(void *space);
};

#ifdef FL_DEBUG_MEMORY

inline void* operator new(size_t bytes)
{
    return MemoryManager::allocate(bytes);
}

inline void operator delete(void *space, size_t bytes)
{
    MemoryManager::freed(space, bytes);
}

inline void operator delete(void *space)
{
    MemoryManager::freed(space);
}

#endif

#endif // MEMORYMANAGER_H
