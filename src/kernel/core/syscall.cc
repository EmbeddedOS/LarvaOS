#include <syscall_table.hh>
#include <syscall.hh>
#include <process.hh>
#include <panic.hh>
#include <api.hh>

extern "C"
{
#include <interrupt.h>

    void *syscall_handler(int num, interrupt_frame *frame)
    { // System callback function.

        void *res = NULL;
        lava::proc::get_instance().load_kernel_page();

        // Save state of the task, we can resume from that state at any time that we like.
        // We can pause tasks, do some thing else and then resume them later on.
        lava::proc::get_instance().save_current_task_state(frame);

        lava::sys_args args;
        args.load_args(frame);
        res = lava::syscalls::get_instance().call(num, args);

        // Switch back to task page and return response.
        lava::proc::get_instance().load_user_page();
        return res;
    }
}

namespace lava
{
    void syscalls::initialize()
    {
        // Add some syscall handler function here.
        this->add(syscall_entry::SYS_zero, &sys_zero);
    }

    syscalls &syscalls::get_instance()
    {
        static syscalls instance;
        return instance;
    }

    void syscalls::add(int num, syscall_handler h)
    {
        if (num < 0 || num > MAXIMUM_NUMBER_OF_SYSCALLS)
        {
            kernel_panic::panic("The syscall number is out of bounds.");
        }

        if (_calls[num] != nullptr)
        {
            kernel_panic::panic("Handler of the syscall number is existing.");
        }

        _calls[num] = h;
    }

    void *syscalls::call(int num, const sys_args &args)
    {

        if (num < 0 || num > MAXIMUM_NUMBER_OF_SYSCALLS)
        {
            return nullptr;
        }

        if (_calls[num] != nullptr)
        {
            return _calls[num](args);
        }

        return nullptr;
    }

    void sys_args::__set_param(uint32_t ret,
                               uint32_t ret1,
                               uint32_t ret2,
                               uint32_t ret3,
                               uint32_t ret4)
    {
        _ret_reg[0] = ret;
        _ret_reg[1] = ret1;
        _ret_reg[2] = ret2;
        _ret_reg[3] = ret3;
        _ret_reg[4] = ret4;
    }

    uint32_t sys_args::get_arg(uint32_t n)
    {
        if (n < MAXIMUM_NUMBER_OF_SYSCALL_ARGS)
        {
            return _ret_reg[n];
        }
        else
        {
            return 0;
        }
    }

    void sys_args::load_args(interrupt_frame *frame)
    {

        uint32_t *sp_ptr = (uint32_t *)frame->esp;

        proc::get_instance().load_user_page();

        __set_param(sp_ptr[0], sp_ptr[1], sp_ptr[2], sp_ptr[3], sp_ptr[4]);

        proc::get_instance().load_kernel_page();
    }

}