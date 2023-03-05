#include <process.hh>
#include <iostream.hh>

extern "C"
{
#include <interrupt.h>
#include <string.h>
#include <task/process.h>
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
        if (_current_proc_id > 0)
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

}