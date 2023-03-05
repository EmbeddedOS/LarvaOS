#include <process.hh>
#include <iostream.hh>
#include <memory.hh>
#include <panic.hh>

extern "C"
{
#include <interrupt.h>
#include <string.h>
#include <task/process.h>
#include <task/task.h>
}

namespace lava
{
    task_state_segment proc::_tss;

    void proc::initialize()
    {
        __initialize_task_state_segment();
    }

    proc &proc::get_instance()
    {
        static proc instance;
        return instance;
    }

    void proc::load_proc(const string &filename)
    {
        _current_proc_id = load_process(filename.data(), &_current_proc);
        if (_current_proc_id >= 0)
        {
            lava::cout << "Load success process: " << filename << "with pid: " << _current_proc_id << lava::endl;
        }
    }

    task_state_segment *proc::get_task_state_segment()
    {
        return &_tss;
    }

    void proc::__initialize_task_state_segment()
    {
        memset(&_tss, 0x00, sizeof(_tss));
        _tss.esp0 = 0x600000;
        _tss.ss0 = KERNEL_DATA_SELECTOR;
        tss_load(0x28);
    }

    void proc::load_user_page()
    {
        load_user_task_page();
    }

    void proc::load_kernel_page()
    {
        load_kernel_data_segment_registers();
        vm::get_instance().switch_to_kvm();
    }

    void proc::save_current_task_state(struct interrupt_frame *frame)
    {
        struct task *task = get_current_task();
        if (task == NULL)
        {
            kernel_panic::panic("No current task to save.");
        }

        task_save_state(task, frame);
    }

}