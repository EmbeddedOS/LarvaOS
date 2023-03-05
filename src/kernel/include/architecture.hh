#pragma once
extern "C"
{
#include <memory/paging.h>
#include <gdt.h>
#include <task/task_state.h>
}

namespace lava
{

    constexpr int TOTAL_GDT_SEGMENTS = 6;

    class arch_interface
    {
    public:
        virtual void initialize() = 0;

    protected:
        arch_interface() = default;
        virtual ~arch_interface() = default;
    };

    class arch : public arch_interface
    {
    public:
        static arch &get_instance();

        void initialize() override final;

        arch(const arch &) = delete;
        arch(arch &&) = delete;
        void operator=(arch const &) = delete;
        void operator=(arch &&) = delete;

    private:
        arch();
        ~arch() = default;

        class global_descriptor_table : public arch_interface
        {
        public:
            void initialize() override final;
            global_descriptor_table(const arch &ar);
            global_descriptor_table() = default;

        private:
            segment_descriptor _gdt_runtime[TOTAL_GDT_SEGMENTS];
            structured_segment_descriptor _structured_gdt_runtime[TOTAL_GDT_SEGMENTS];
        };

        global_descriptor_table _gdt;

        friend class global_descriptor_table;
    };

}
