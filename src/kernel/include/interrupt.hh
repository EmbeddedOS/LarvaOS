#pragma once
#include <architecture.hh>

namespace lava
{
    class interrupt : public arch_interface
    {
    public:
        void initialize() override final;
        static interrupt &get_instance();

        interrupt(const interrupt &) = delete;
        interrupt(interrupt &&) = delete;
        void operator=(interrupt const &) = delete;
        void operator=(interrupt &&) = delete;

        void disable() const;
        void enable() const;

    private:
        interrupt() = default;
        ~interrupt() = default;
    };
}
