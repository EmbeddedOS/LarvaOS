# ATA Hard drive

## 1. What is ATA Hard Drive?

Hard drives are used to store data and information so that you can access and retrieve these data when you need them. Once the hard drive can no longer store data, you can purchase another hard drive to replace it. In a computer system, there are two types of ATA hard drive - **PATA hard drive** and **SATA hard drive**.

What is ATA hard drive? ATA is the abbreviation of Advanced Technology Attachment, it can also be called **IDE** or **PATA**. It is a **disk drive** (also called "**disk driver**") that integrates the **drive controller** directly on the drive itself. The computer can use an ATA hard disk without a specific controller to support the drive.

The motherboard must still support ATA connections, but a separate card is not required. The different types of ATA standards include `ATA-1`, `ATA-2`, `ATA-3`, `Ultra ATA`, `ATA/66`, and `ATA/100`. ATA hard drive’s maximum data transfer rate is `133MB/s`.

### 1.1. IDE

The name **IDE** is often used interchangeably with ATA, but "**IDE**" actually refers to only the electrical specifications of the signals on the `40/80` pin disk cable. **ATA** is the proper name for the entire specification.

### 1.2. SATA and PATA

Recently a new standard for ATA data transmission has emerged. It was named **SATA**, and the previous well-known forms of ATA were retroactively renamed **PATA**, "to reduce confusion" -- while certainly causing it.

## 2. ATA PIO Mode

According to the ATA specs, PIO mode must alway be supported by all ATA-compliant drives as the default data transfer mechanism.

PIO mode uses a tremendous amount of CPU resources, because every byte of data transferred between the disk and the CPU must be sent though the CPU's `IO port bus` (not the memory). On some CPUs, PIO mode can still achieve actual transfer speeds of 16MB per sec, but no other processes on the machine will get any CPU time.

However, when a computer is just beginning to boot there are no other processes. So PIO mode is an excellent and simple interface to utilize during boot-up, until the system goes into multitasking mode.

### 2.1. Hardware

### 2.2. Master/Slave Drives

### 2.3. Primary/Secondary Bus

### 2.4. 400ns delays

### 2.5. Cache Flush

### 2.6. Bad Sectors

### 2.7. Detection and initialization

### 2.8. Addressing Modes

## 3. ATA read/write sectors

Since interrupts (like `INT 13h`) can not be called easily in protected mode or long mode, direct disk access though ports might be the only solution. The below code is example of subroutines to read and write disk sectors directly from the first hard disk (`80h`) in long mode using **CHS** and **LBA**.

### 3.1. ATA read sectors

#### 3.1.1. Read in CHS mode

TODO: complete me, please!

#### 3.1.2. Read in LBA mode

```asm
;=============================================================================
; ATA read sectors (LBA mode) 
;
; @param EAX Logical Block Address of sector
; @param CL  Number of sectors to read
; @param RDI The address of buffer to put data obtained from disk
;
; @return None
;=============================================================================
ata_lba_read:
               pushfq
               and rax, 0x0FFFFFFF
               push rax
               push rbx
               push rcx
               push rdx
               push rdi
 
               mov rbx, rax         ; Save LBA in RBX
 
               mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
               shr eax, 24          ; Get bit 24 - 27 in al
               or al, 11100000b     ; Set bit 6 in al for LBA mode
               out dx, al
 
               mov edx, 0x01F2      ; Port to send number of sectors
               mov al, cl           ; Get number of sectors from CL
               out dx, al
 
               mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
               mov eax, ebx         ; Get LBA from EBX
               out dx, al
 
               mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
               mov eax, ebx         ; Get LBA from EBX
               shr eax, 8           ; Get bit 8 - 15 in AL
               out dx, al
 
 
               mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
               mov eax, ebx         ; Get LBA from EBX
               shr eax, 16          ; Get bit 16 - 23 in AL
               out dx, al
 
               mov edx, 0x1F7       ; Command port
               mov al, 0x20         ; Read with retry.
               out dx, al
 
.still_going:  in al, dx
               test al, 8           ; the sector buffer requires servicing.
               jz .still_going      ; until the sector buffer is ready.
 
               mov rax, 256         ; to read 256 words = 1 sector
               xor bx, bx
               mov bl, cl           ; read CL sectors
               mul bx
               mov rcx, rax         ; RCX is counter for INSW
               mov rdx, 0x1F0       ; Data port, in and out
               rep insw             ; in to [RDI]
 
               pop rdi
               pop rdx
               pop rcx
               pop rbx
               pop rax
               popfq
               ret
```

TODO: complete me, please!
