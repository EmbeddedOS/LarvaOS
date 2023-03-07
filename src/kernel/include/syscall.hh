#pragma once
#include <architecture.hh>

struct interrupt_frame;

namespace lava
{
    constexpr int MAXIMUM_NUMBER_OF_SYSCALL_ARGS{5};
    constexpr int MAXIMUM_NUMBER_OF_SYSCALLS{100};

    class sys_args
    {
    public:
        sys_args() = default;
        ~sys_args() = default;

        uint32_t get_arg(uint32_t n); /* Get a syscall argument. */
        void load_args(interrupt_frame *frame);

    private:
        void __set_param(uint32_t ret,
                         uint32_t ret1,
                         uint32_t ret2,
                         uint32_t ret3,
                         uint32_t ret4); /* Set the syscall arguments. */

        uint32_t _ret_reg[MAXIMUM_NUMBER_OF_SYSCALL_ARGS];
    };

    typedef void *(*syscall_handler)(const sys_args&);

    class syscalls : public arch_interface
    {

    public:
        void initialize() override final;
        static syscalls &get_instance();

        syscalls(const syscalls &) = delete;
        syscalls(syscalls &&) = delete;
        void operator=(syscalls const &) = delete;
        void operator=(syscalls &&) = delete;

        void add(int num, syscall_handler h);
        void *call(int num,const sys_args& args);

    private:
        syscalls() = default;
        ~syscalls() = default;

        syscall_handler _calls[MAXIMUM_NUMBER_OF_SYSCALLS];
    };
}