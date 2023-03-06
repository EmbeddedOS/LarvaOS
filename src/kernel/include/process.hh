#pragma once
#include <architecture.hh>
#include <string.hh>

extern "C"
{
#include <task/task_state.h>
}

struct process;
struct interrupt_frame;
namespace lava
{
    typedef struct process *arch_proc;

    class proc : public arch_interface
    {
    public:
        void initialize() override final;
        static proc &get_instance();

        proc(const proc &) = delete;
        proc(proc &&) = delete;
        void operator=(proc const &) = delete;
        void operator=(proc &&) = delete;

        void load_user_page();
        void load_kernel_page();
        void load_proc(const string &filename);
        void save_current_task_state(struct interrupt_frame *frame);
        void save_the_syscall_arguments(struct interrupt_frame *frame);

        static task_state_segment *get_task_state_segment();


    private:
        proc() = default;
        ~proc() = default;
        static void __initialize_task_state_segment();

        static task_state_segment _tss;
        int _current_proc_id{0};
        arch_proc _current_proc{nullptr};

        friend class global_descriptor_table;
    };
}