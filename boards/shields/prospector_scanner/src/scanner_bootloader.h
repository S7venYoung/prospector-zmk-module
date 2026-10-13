#pragma once
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/retention/retention.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/sys/reboot.h>
#include <errno.h>

/* XIAO nRF52840's Adafruit UF2 loader consumes this byte in GPREGRET.
 * Write the raw retention target explicitly rather than relying on an
 * arbitrary sys_reboot(type) argument or the generic mode mapper. */
static inline int scanner_enter_bootloader(void) {
    int ret;
#if DT_HAS_CHOSEN(zmk_magic_boot_mode)
    const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zmk_magic_boot_mode));
    if (!device_is_ready(dev)) return -ENODEV;
    const uint8_t uf2_magic = 0x57;
    ret = retention_write(dev, 0, &uf2_magic, sizeof(uf2_magic));
#else
    ret = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
#endif
    if (ret < 0) return ret;
    sys_reboot(SYS_REBOOT_WARM);
    return -EIO;
}
