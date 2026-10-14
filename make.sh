#!/bin/sh

set -e

OUTPUT=sehnsucht
CC=x86_64-elf-gcc
LD=x86_64-elf-ld
AS=nasm
CCFLAGS="-Wall \
    -Wextra \
    -isystem src/kernel/ \
    -Isrc/libc/include/ \
    -std=gnu99 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mabi=sysv \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel"
LDFLAGS="-nostdlib \
        -static \
        -z max-page-size=0x1000 \
        --gc-sections \
        -T linker.lds"
ASFLAGS="-felf64 -Isrc/common/"
PARTED_PATH="parted"
LIMINE_PATH=$(nix-build '<nixpkgs>' -A limine --no-out-link)
CLANG_STYLE="mozilla"

format() 
{
    find src -name '*.c' -exec clang-format -i --style=file:.clang-format {} +
}

build_limine()
{
    mkdir -p other
    rm -rf other/limine-binary limine-binary.tar.gz
    curl -L https://github.com/Limine-Bootloader/Limine/releases/latest/download/limine-binary.tar.gz | gunzip | tar -xf - -C other/
    make -C other/limine-binary && \
        echo "Done building Limine"
}

clean()
{
    rm -rvf \
       build \
       $OUTPUT \
       disk.img \
       ./other/unscii.psf 
}

build()
{
    mkdir -p build

    find src -name '*.c' | while IFS= read -r f; do
        mkdir -p "build/$(dirname "$f")"
        $CC $CCFLAGS -MMD -MP -c "$f" -o "build/${f%.c}.o"    
    done

    find src -name "*.asm" | while IFS= read -r f; do
        mkdir -p "build/$(dirname "$f")"
        $AS $ASFLAGS "$f" -o "build/${f%.asm}.asm.o"
    done

    if [ ! -f "./other/unscii.psf" ]; then
        lua ./scripts/hex2psf1.lua \
            ./other/unscii.hex \
            ./other/unscii.psf
    fi

    $LD -r -b binary ./other/unscii.psf -o build/unscii.o
    $LD $LDFLAGS -o $OUTPUT $(find build -name "*.o")

    dd if=/dev/zero bs=1M count=0 seek=64 of=disk.img && \
        echo "Done creating disk image"
    parted -s disk.img mklabel gpt && \
        echo "Created GPT table"
    parted -s disk.img mkpart primary 1MiB 2MiB && \
        parted -s disk.img set 1 bios_grub on && \
        echo "Created BIOS boot partition"
    parted -s disk.img mkpart ESP fat32 4096s 100% && \
        parted -s disk.img set 2 esp on && \
        echo "Created ESP partition"
    # Wipe BIOS boot partition
    dd if=/dev/zero of=disk.img bs=512 count=2048 seek=2048 conv=notrunc
    
    #./other/limine-binary/limine bios-install disk.img
    "$LIMINE_PATH/bin/limine" bios-install disk.img
    mformat -F -i disk.img@@2M ::
    mmd -i disk.img@@2M ::/EFI
    mmd -i disk.img@@2M ::/EFI/BOOT
    mmd -i disk.img@@2M ::/boot
    mmd -i disk.img@@2M ::/boot/limine
    
    mcopy -i disk.img@@2M $OUTPUT ::/boot
    mcopy -i disk.img@@2M ./limine.conf ::/boot/limine
    mcopy -i disk.img@@2M "$LIMINE_PATH/share/limine/limine-bios.sys" ::/boot/limine
    mcopy -i disk.img@@2M "$LIMINE_PATH/share/limine/BOOTX64.EFI" ::/EFI/BOOT

    echo "Built"
}
debug()
{
    qemu-system-x86_64 \
	    -drive format=raw,file=disk.img \
	    -S -s \
        -serial mon:stdio \
        -d int \
	    -no-shutdown \
	    -no-reboot
}

run()
{
    qemu-system-x86_64 -drive format=raw,file=disk.img 
}

case "$1" in
    clean)
        clean
        exit 0
        ;;
    build|'')
        build
        exit 0
        ;;
    build_limine)
        build_limine
        exit 0
        ;;
    run)
        run
        exit 0
        ;;
    debug)
        debug
        exit 0
        ;;
    format)
        format
        exit 0
        ;;
    *)
        echo "Wrong arguments"
        exit 1
        ;;
esac
