# x86 calling conventions

Calling conventions describe the interface of called code:

* The order in which atomic (scalar) parameters, or individual parts of a complex parameter, are allocated.
* How parameters are passed (pushed on the stack, placed in registers, or a mix of both).
* Which registers the called function must preserve for the caller (also known as: callee-saved registers or non-volatile registers).
* How the task of preparing the stack for, and restoring after, a function call is divided between the caller and the callee.

There are subtle differences in how various compilers implement these conventions, so it is often difficult to interface code which is compiled by different compilers. On the other hand, conventions which are used as an API standard (such as **stdcall**) are very uniformly implemented.

## 1. Caller clean-up

In these conventions, the caller cleans the arguments from the stack.

### 1.1. cdecl

The **cdecl** (which stands for C declaration) is a calling convention for the C programming language and is used by many C compilers for the x86 architecture. In **cdecl**, subroutine arguments are passed on the stack. Integer values and memory addresses are returned in the **EAX** register, floating point values in the ST0 x87 register. Registers **EAX**, **ECX**, and **EDX** are caller-saved, and the rest are callee-saved.

In the context of the C programming language, function arguments are pushed on the stack in the **right-to-left** order, i.e. the last argument is pushed first.

Consider the following C source code snippet:

```c
int callee(int, int, int);

int caller(void)
{
    return callee(1, 2, 3) + 5;
}
```

On x86, it might produce the following assembly code (Intel syntax):

```asm
caller:
    ; make new call frame
    ; (some compilers may produce an 'enter' instruction instead)
    push    ebp       ; save old call frame
    mov     ebp, esp  ; initialize new call frame
    ; push call arguments, in reverse
    ; (some compilers may subtract the required space from the stack pointer,
    ; then write each argument directly, see below.
    ; The 'enter' instruction can also do something similar)
    ; sub esp, 12      : 'enter' instruction could do this for us
    ; mov [ebp-4], 3   : or mov [esp+8], 3
    ; mov [ebp-8], 2   : or mov [esp+4], 2
    ; mov [ebp-12], 1  : or mov [esp], 1
    push    3
    push    2
    push    1
    call    callee    ; call subroutine 'callee'
    add     esp, 12   ; remove call arguments from frame
    add     eax, 5    ; modify subroutine result
                      ; (eax is the return value of our callee,
                      ; so we don't have to move it into a local variable)
    ; restore old call frame
    ; (some compilers may produce a 'leave' instruction instead)
    mov     esp, ebp  ; most calling conventions dictate ebp be callee-saved,
                      ; i.e. it's preserved after calling the callee.
                      ; it therefore still points to the start of our stack frame.
                      ; we do need to make sure
                      ; callee doesn't modify (or restore) ebp, though,
                      ; so we need to make sure
                      ; it uses a calling convention which does this
    pop     ebp       ; restore old call frame
    ret               ; return
```

The caller cleans the stack after the function call returns.
