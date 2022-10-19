# x86 instruction set

## 1. 8086 Registers

### 1.1. General-purpose registers

General-purpose registers are used to store temporary data within the microprocessor. There are 8 general-purpose registers in the 8086 microprocessor:

* `AX`: This is the **accumulator**. It is of 16 bits and is divided into two 8-bit registers AH and AL to also perform 8-bit instructions. It is generally used for arithmetical and logical instructions but in 8086 microprocessor it is not mandatory to have an accumulator as the destination operand. Example: ```ADD AX, AX (AX = AX + AX)```

* `BX`: This is the **base register**. It is of 16 bits and is divided into two 8-bit registers BH and BL to also perform 8-bit instructions. It is used to store the value of the offset. Example: ```MOV BL, [500] (BL = 500H)```

* `CX`: This is the **counter register**. It is of 16 bits and is divided into two 8-bit registers CH and CL to also perform 8-bit instructions. It is used in looping and rotation. Example:

```asm
MOV CX, 0005
LOOP
```

* `DX`: This is the **data register**. It is of 16 bits and is divided into two 8-bit registers DH and DL to also perform 8-bit instructions. It is used in the multiplication and input/output port addressing. Example: ```MUL BX (DX, AX = AX * BX)```

* `SP`: This is the **stack pointer**. It is of 16 bits. It points to the topmost item of the stack. If the stack is empty the stack pointer will be `(FFFE)H`. Its offset address is relative to the stack segment.

* `BP` – This is the **base pointer**. It is of 16 bits. It is primarily used in accessing parameters passed by the stack. Its offset address is relative to the stack segment.

* `SI` – This is the **source index register**. It is of 16 bits. It is used in the pointer addressing of data and as a source in some string-related operations. Its offset is relative to the data segment.

* `DI` – This is the **destination index register**. It is of 16 bits. It is used in the pointer addressing of data and as a destination in some string-related operations. Its offset is relative to the extra segment.

* `IP` - **Instruction pointer**, contains the value of the current address you are executing a memory, so the process uses the IP register to know which instruction to load from RAM.

### 1.2. 8086 Segment Registers

* `CS` - **Code segment register**. Is used for addressing memory location in the code segment of the memory, where the executable program is stored.

* `DS` - **Data segment register**. Points to the data segment of the memory where the data is stored.

* `ES` - **Extra Segment register**. Also refers to a segment in the memory which is another data segment in the memory.

* `SS` - **Stack segment register**. Is used for addressing stack segment of the memory. The stack segment is that segment of memory which is used to store stack data.

### 1.3. 32-bit data registers

Four 32-bit data registers are used for arithmetic, logical, and other operations. These 32-bit registers can be used in three ways:

* As complete 32-bit data registers: EAX, EBX, ECX, EDX.
* Lower halves of the 32-bit registers can be used as four 16-bit data registers: AX, BX, CX and DX.
* Lower and higher halves of the above-mentioned four 16-bit registers can be used as eight 8-bit data registers: AH, AL, BH, BL, CH, CL, DH, and DL.

## 2. Define data

### 2.1. Intrinsic Data types

The assembler recognizes a basic set of intrinsic data types, which describe types in terms of their size (byte, word, doubleword, and so on).
|Type|Usage|
|----|-----|
|BYTE|8-bit unsigned integer. B stands for byte.|
|SBYTE|8-bit signed integer. S stands for signed.|
|WORD|16-bit unsigned integer.|
|SWORD|16-bit signed integer.|
|DWORD|32-bit unsigned integer. D stands for double.|
|SDWORD|32-bit signed integer. SD stands for signed double.|
|FWORD|48-bit integer (Far pointer in protected mode).|
|QWORD|64-bit integer. Q stands for quad.|
|TBYTE|80-bit (10-byte) integer. T stands for Ten-byte.|
|REAL4|32-bit (4-byte) IEEE short real.|
|REAL8|64-bit (8-byte) IEEE long real.|
|REAL10|80-bit (10-byte) IEEE extended real.|

### 2.2. Data definition Statement

A data definition statement sets aside storage in memory for a variable, with an optional name. Data definition statements create variables based in intrinsic data types. A data definition has the following syntax:

```text
[name] directive initializer [,initializer]...
```

This is an example of a data definition statement:

```asm
count DWORD 12345
```

* **Directive** The directive in a data definition statement can be `BYTE`, `WORD`, `DWORD`, `SBYTE`, `SWORD`, or any of the types listed in the top table. In addition, it can be any of the **legacy** data definition directives shown:

|Directive|Usage|
|---------|-----|
|DB|8-bit integer.|
|DW|16-bit integer.|
|DD|32-bit integer or real.|
|DQ|64-bit integer or real.|
|DT|define 80-bit (10-byte) integer.|

* **Initializer**, At least one initializer is required in a data definition, even if it is zero. Additional initializers, if any, are separated by commas.

#### 2.2.1 Multiple Initializers

If Multiple Initializers are used in the same data definition, its label refers only to the offset of the first initializer.
In the following example, assume list is located at offset `0000`. If so, the value `10` is at offset `0000`, `20` is at offset `0001`, `30` is at offset `0002`, and `40` is at offset `0003`:

```asm
list BYTE 10,20,30,40
```

Memory layout of a byte sequence:

|Offset|Value|
|------|-----|
|0000|10|
|0001|20|
|0002|30|
|0003|40|

Not all data definitions require labels. To continue the array of bytes begun with list, for example, we can define additional bytes on the next lines:

```asm
list BYTE 10,20,30,40
     BYTE 50,60,70,80
     BYTE 81,82,83,84
```

#### 2.2.2 Defining Strings

To define a string of characters, **onclose** them in single or double quotation **marks**. The most common type of string ends with a **NULL** byte (containing 0). called a **null-terminated string**, strings of this type are used in many programming languages:

```asm
greeting1 BYTE "Good afternoon",0
greeting2 BYTE 'Good night',0
```

Each character uses a byte of storage.

Strings are an exception to the rule that byte values must be separated by **commas**. Without that exception, greeting1 would have to be defined as:

```asm
greeting1 BYTE 'G','o','o','d'....etc.
```

#### 2.2.3 DUP Operator

The DUP operator allocates storage for multiple data items, using a integer expression as a counter. It is particularly useful when allocating space for a string or array, and can be used with initialized or uninitialized data:

```asm
BYTE 20 DUP(0)      ; 20 bytes, all equal to zero
BYTE 20 DUP(?)      ; 20 bytes, uninitialized
BYTE 4 DUP("STACK") ; 20 bytes: "STACKSTACKSTACKSTACK"
```

## 3. Directives

* `TIMES` - times directive allows **multiple initializations to the same value**. For example, an array named marks of size 9 can be defined and initialized to zero using the following statement:

```asm
marks  TIMES  9  DW  0
```

## 4. symbols

* `$` - denotes the **current address** of the statement.
* `$$` - denotes the **address of the beginning of the current section**.

## 5. Instructions
