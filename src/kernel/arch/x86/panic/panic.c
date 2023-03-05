#include <panic.h>
#include <video.h>

/*
 * Panic is called on unresolvable
 * fatal errors.
 * It syncs, prints "panic: msg" and
 * then loops.
 */
void arc_panic(const char *msg)
{
    print("PANIC: ");
    print(msg);
    print("\n");
    for (;;)
        ;
}