#include <cstddef>
#include <iostream>

template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
public:
    MemoryPool()
    {
        currentBlock = nullptr;
        currentSlot = nullptr;
        lastSlot = nullptr;
        freeSlots = nullptr;
    }

    MemoryPool(const MemoryPool& memoryPool)
        : MemoryPool()
    {
    }

    MemoryPool(MemoryPool&& memoryPool)
    {
        currentBlock = memoryPool.currentBlock;
        memoryPool.currentBlock = nullptr;
        currentSlot = memoryPool.currentSlot;
        lastSlot = memoryPool.lastSlot;
        freeSlots = memoryPool.freeSlots;
    }

    template <class U>
    MemoryPool(const MemoryPool<U>& memoryPool)
        : MemoryPool()
    {
    }

    ~MemoryPool()
    {
        Slot* curr = currentBlock;
        while (curr != nullptr)
        {
            Slot* prev = curr->next;
            operator delete(reinterpret_cast<void*>(curr));
            curr = prev;
        }
    }

    MemoryPool& operator=(const MemoryPool& memoryPool) = delete;
    MemoryPool& operator=(MemoryPool&& memoryPool)
    {
        if (this != &memoryPool)
        {
            std::swap(currentBlock, memoryPool.currentBlock);
            currentSlot = memoryPool.currentSlot;
            lastSlot = memoryPool.lastSlot;
            freeSlots = memoryPool.freeSlots;
        }
        return *this;
    }

    T* address(T& x) const
    {
        return &x;
    }

    const T* address(const T& x) const
    {
        return &x;
    }

    // Can only allocate one object at a time. n and hint are ignored
    T* allocate(size_t n = 1, const T* hint = 0)
    {
        if (freeSlots != nullptr)
        {
            T* result = reinterpret_cast<T*>(freeSlots);
            freeSlots = freeSlots->next;
            return result;
        }
        else 
        {
            if (currentSlot >= lastSlot)
                allocateBlock();
            return reinterpret_cast<T*>(currentSlot++);
        }
    }

    void deallocate(T* p, size_t n = 1)
    {
        if (p != nullptr)
        {
            reinterpret_cast<Slot*>(p)->next = freeSlots;
            freeSlots = reinterpret_cast<Slot*>(p);
        }
    }

    size_t max_size() const  
    {
        size_t maxBlocks = -1 / BlockSize;
        return (BlockSize - sizeof(char*)) / sizeof(Slot) * maxBlocks;
    }

    template <class U, class... Args>
    void construct(U* p, Args&&... args)
    {
        new (p) U(std::forward<Args>(args)...);
    }
    
    template <class U>
    void destroy(U* p)
    {
        p->~U();
    }

    template <class... Args>
    T* newElement(Args&&... args)
    {
        T* result = allocate();
        construct<T>(result, std::forward<Args>(args)...);
        return result;
    }

    void deleteElement(T* p)
    {
        if (p != nullptr)
        {
            p->~T();
            deallocate(p);
        }
    }

private:

    size_t padPointer(char* p, size_t align) const  
    {
        uintptr_t result = reinterpret_cast<uintptr_t>(p);
        return ((align - result) % align);
    }

    void allocateBlock()
    {
        // Allocate space for the new block and store a T* to the previous one
        char* newBlock = reinterpret_cast<char*>
            (operator new(BlockSize));
        reinterpret_cast<Slot*>(newBlock)->next = currentBlock;
        currentBlock = reinterpret_cast<Slot*>(newBlock);
        // Pad block body to staisfy the alignment requirements for elements
        char* body = newBlock + sizeof(Slot*);
        size_t bodyPadding = padPointer(body, alignof(Slot));
        currentSlot = reinterpret_cast<Slot*>(body + bodyPadding);
        lastSlot = reinterpret_cast<Slot*>
            (newBlock + BlockSize - sizeof(Slot) + 1);
    }

private:

    union Slot
    {
        T element;
        Slot* next;
    };

    static_assert(BlockSize >= 2 * sizeof(Slot), "BlockSize too small.");

    Slot* currentBlock;
    Slot* currentSlot;
    Slot* lastSlot;
    Slot* freeSlots;

};

int main()
{

    return 0;
}