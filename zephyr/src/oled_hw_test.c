#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(oled_hw_test, LOG_LEVEL_INF);

#if !DT_HAS_CHOSEN(zephyr_display)
#error "CONFIG_ZMK_OLED_HW_TEST requires zephyr,display chosen node"
#endif

static int oled_hw_test_init(void) {
    const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    struct display_buffer_descriptor desc = {
        .width = 128,
        .height = 32,
        .pitch = 128,
        .buf_size = (128 * 32) / 8,
    };
    static uint8_t frame[(128 * 32) / 8];

    if (!device_is_ready(display)) {
        LOG_ERR("Display device not ready");
        return 0;
    }

    /* Build a stable visible pattern in MONO_VTILED layout (8 vertical pixels per byte). */
    for (size_t page = 0; page < 4; page++) {
        for (size_t x = 0; x < 128; x++) {
            frame[(page * 128) + x] = (((x / 8U) + page) & 1U) ? 0xFF : 0x00;
        }
    }

    display_blanking_off(display);

    if (display_write(display, 0, 0, &desc, frame) != 0) {
        LOG_ERR("display_write failed");
        return 0;
    }

    LOG_INF("OLED HW test pattern written");
    return 0;
}

SYS_INIT(oled_hw_test_init, APPLICATION, 99);
