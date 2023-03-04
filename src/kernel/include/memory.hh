#pragma once
#include <types.h>
#include <architecture.hh>

[[gnu::visibility("default")]] void *operator new(size_t size);
[[gnu::visibility("default")]] void operator delete(void *ptr);
[[gnu::visibility("default")]] void *operator new[](size_t size);
[[gnu::visibility("default")]] void operator delete[](void *ptr);
[[gnu::visibility("default")]] void operator delete(void *ptr, size_t size);
[[gnu::visibility("default")]] void operator delete[](void *ptr, size_t size);

typedef struct paging_4GB_chunk* virtual_memory;

namespace lava
{ // Virtual memory.
    class vm : public arch_interface
    {
    public:
        void initialize() override final;
        static vm &get_instance();

        void switch_to_vm(virtual_memory other);
        void switch_to_kvm();

        vm(const vm &) = delete;
        vm(vm &&) = delete;
        void operator=(vm const &) = delete;
        void operator=(vm &&) = delete;

    private:
        virtual_memory _kvm{nullptr};
        vm() = default;
        ~vm() = default;
    };
}