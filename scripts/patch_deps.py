Import("env")

from pathlib import Path

project = Path(env["PROJECT_DIR"])
libdeps = project / ".pio" / "libdeps" / "esp32dev"


def patch_ps5():
    target = libdeps / "ps5Controller" / "src" / "ps5.c"
    if not target.exists():
        print("patch_deps: ps5.c not found, skip")
        return
    text = target.read_text(encoding="utf-8")
    if 'esp_mac.h' in text:
        print("patch_deps: ps5.c already patched")
        return
    text = text.replace(
        '#include <esp_system.h>',
        '#include <esp_system.h>\n#include "esp_mac.h"',
        1,
    )
    target.write_text(text, encoding="utf-8")
    print("patch_deps: added esp_mac.h include to ps5.c")


def patch_bmi160():
    # GCC 14 / Arduino-ESP32 3.x: BMI160Class has a non-pure virtual with no
    # definition, so no vtable is emitted -> linker error.
    target = libdeps / "EmotiBit BMI160" / "BMI160.h"
    if not target.exists():
        print("patch_deps: BMI160.h not found, skip")
        return
    text = target.read_text(encoding="utf-8")
    old = "virtual int serial_buffer_transfer(uint8_t *buf, unsigned tx_cnt, unsigned rx_cnt);"
    new = "virtual int serial_buffer_transfer(uint8_t *buf, unsigned tx_cnt, unsigned rx_cnt) = 0;"
    if new in text:
        print("patch_deps: BMI160.h already patched")
        return
    if old not in text:
        print("patch_deps: BMI160.h unexpected contents, skip")
        return
    target.write_text(text.replace(old, new, 1), encoding="utf-8")
    print("patch_deps: made BMI160Class::serial_buffer_transfer pure virtual")


patch_ps5()
patch_bmi160()
