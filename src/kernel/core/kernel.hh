#pragma once

 
#if defined(__linux__)
#warning "You are NOT using a cross-compiler."
#endif
 
#if !defined(__i386__)
#warning "This kernel needs to be compiled with a ix86-elf compiler."
#endif
 
extern "C"
{
    void kernel_main();
}