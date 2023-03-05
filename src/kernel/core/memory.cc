#include <memory.hh>
extern "C"
{
#include <memory/kheap.h>
#include <memory/paging.h>
}

[[gnu::visibility("default")]] void *operator new(size_t size)
{
    return kmalloc(size);
}

[[gnu::visibility("default")]] void operator delete(void *ptr)
{
    kfree(ptr);
}

[[gnu::visibility("default")]] void *operator new[](size_t size)
{
    return ::operator new(size);
}

[[gnu::visibility("default")]] void operator delete[](void *ptr)
{
    return ::operator delete(ptr);
}

[[gnu::visibility("default")]] void operator delete(void *ptr, size_t size)
{
    ::operator delete(ptr);
}

[[gnu::visibility("default")]] void operator delete[](void *ptr, size_t size)
{
    ::operator delete(ptr);
}

namespace lava
{

    void kernel_heap::initialize_kernel_heap()
    {
        kheap_init();
    }

    void vm::initialize()
    { // Make 4GB virtual memory address space for the kernel and switch to it.
        _kvm = make_new_4GB_virtual_memory_address_space(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
        paging_install_kernel_page(_kvm);
        switch_to_kvm();
        enable_paging();
    }

    vm &vm::get_instance()
    {
        static vm instance;
        return instance;
    }

    void vm::switch_to_vm(virtual_memory other)
    {
        switch_to_page(other);
    }

    void vm::switch_to_kvm()
    {
        switch_to_kernel_page();
    }
}