#include <syscall.hh>
#include <process.hh>

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

        res = lava::syscall::get_instance().handle(num, frame);

        // Switch back to task page and return response.
        lava::proc::get_instance().load_user_page();
        return res;
    }
}

namespace lava
{
    void syscall::initialize()
    {
    }

    syscall &syscall::get_instance()
    {
        static syscall instance;
        return instance;
    }

    void *syscall::handle(int command, interrupt_frame *frame)
    {
        void *res = NULL;
        return res;
    }
}