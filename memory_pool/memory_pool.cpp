#include <cstdio>
#include <cstdlib>
#include <cstring>
using namespace std;

#define MEM_PAGE_SIZE   0x1000  // 4k

// total 4kB, block 32B
struct MemoryPool
{
    int block_size;     // 32B
    int free_count;     // 128
    void* pMem;         // position of the first block
    void* ptr;          // position of the next block to be allocated
};

int memory_pool_init(MemoryPool* pMp, size_t block_size)
{
    if (pMp == nullptr)
        return -1;
    
    memset(pMp, 0, sizeof(MemoryPool));

    pMp->block_size = block_size;
    pMp->free_count = MEM_PAGE_SIZE / block_size;

    pMp->pMem = malloc(MEM_PAGE_SIZE);
    if (pMp->pMem == nullptr)
        return -1;

    pMp->ptr = pMp->pMem;

    int i = 0;
    char* ptr = (char*)pMp->ptr;
    for (i = 0; i < pMp->free_count; ++i)
    {
        *(char**)ptr = ptr + block_size;
        ptr += block_size;
    }
    *(char**)ptr = nullptr;

    return 0;
}

void* _malloc(MemoryPool* pMp, size_t size)
{
    if (pMp == nullptr || pMp->free_count == 0 || pMp->block_size < size)
        return nullptr;

    void* ptr = pMp->ptr;
    pMp->ptr = *(char**)ptr;

    pMp->free_count--;

    printf("_malloc\n");

    return ptr;
}

void _free(MemoryPool* pMp, void* ptr)
{
    if (pMp == nullptr)
        return;

    *(char**)ptr = (char*)pMp->ptr;
    pMp->ptr = ptr;

    pMp->free_count++;

    printf("_free\n");
}

#define malloc(pMp, size) _malloc(pMp, size)
#define free(pMp, ptr) _free(pMp, ptr)

int main()
{
    MemoryPool memPool;
    memory_pool_init(&memPool, 32);

    void* p1 = malloc(&memPool, 5);
    void* p2 = malloc(&memPool, 10);
    free(&memPool, p1);
    free(&memPool, p2);

    return 0;
}