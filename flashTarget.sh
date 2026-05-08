#!/bin/bash
set -e

# Keressük meg az .elf fájlt a build mappában (automatikusan megtalálja a nevet)
ELF_FILE=$(find build -name "*.elf" | head -n 1)

if [ -z "$ELF_FILE" ]; then
    echo "ERROR! Not find .elf file, in the build dir!"
    exit 1
fi

echo "--- Uploading a program with openOCD: $ELF_FILE ---"

# A parancsod, automatizált fájlnévvel:
openocd -f board/st_nucleo_f103rb.cfg -c "program $ELF_FILE verify reset exit"

echo "--- SUCCESSFUL uploading and start ---"

# Egy másik felöltési opció
# echo "--- Program feltöltése: $BIN_FILE ---"
# st-flash write "$BIN_FILE" 0x08000000
#
# echo "--- Reset és Indítás... ---"
# st-flash reset
