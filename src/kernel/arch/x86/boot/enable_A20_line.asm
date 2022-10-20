; Enable A20 line by using Fast A20 Gate.
; Why do we need enable A20 line? - https://en.wikipedia.org/wiki/A20_line
; How to enable it and how many ways to do that? - https://wiki.osdev.org/A20_Line#Enabling
; Enabling the Gate-A20 line is one of the first steps that a protected mode x86 OS does in the bootup process,
; often before control has been passed to the kernel from bootstrap.
;
; On most newer computers starting with the IBM PS/2, 
; the chipset has a FAST A20 option that can quickly enable the A20 line. To enable A20 this way, 
; there is no need for delay loops or polling, just 3 simple instructions.
[BITS 32]
enable_A20_line:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret