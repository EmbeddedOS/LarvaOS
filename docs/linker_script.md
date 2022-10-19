# Linker scripts

**GNU ld** supports configuration of the linking process via the use of linker scripts. Linker scripts are written in a specialized scripting language specific to the GNU ld application.

A Linker script's principal use is specifying the format and layout of the final executable binary. This is of particular relevance to OS development, where executable binaries will often require specific file layouts in order to be recognized by certain [boot-loaders](bootloader.md). **GNU GRUB** is one such boot-loader.

Linker scripts are typically invoked via the use of the `-T script.ld` command line argument when calling the `ld` application.

## 1. Keywords

Listed below are a selection of significant keywords used within linker scripts.

## 1.1. ENTRY

```linker
ENTRY(main)
ENTRY(MultibootEntry)
```

