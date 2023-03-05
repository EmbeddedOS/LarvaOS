#pragma once
#include <architecture.hh>

struct interrupt_frame;
namespace lava
{
    class syscall : public arch_interface
    {

    public:
        void initialize() override final;
        static syscall &get_instance();

        syscall(const syscall &) = delete;
        syscall(syscall &&) = delete;
        void operator=(syscall const &) = delete;
        void operator=(syscall &&) = delete;

        void *handle(int command, interrupt_frame *frame);

    private:
        syscall() = default;
        ~syscall() = default;
    };
}