#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(oled_hw_test, LOG_LEVEL_INF);

#define EXT_POWER_NODE DT_PATH(ext_power)

#if DT_NODE_EXISTS(EXT_POWER_NODE) && DT_NODE_HAS_PROP(EXT_POWER_NODE, control_gpios)
#define OLED_HW_TEST_HAS_EXT_POWER_GPIO 1
static const struct gpio_dt_spec ext_power_gpio =
    GPIO_DT_SPEC_GET_BY_IDX(EXT_POWER_NODE, control_gpios, 0);
#else
#define OLED_HW_TEST_HAS_EXT_POWER_GPIO 0
#endif

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

#if OLED_HW_TEST_HAS_EXT_POWER_GPIO
    if (device_is_ready(ext_power_gpio.port)) {
        int ep_ret = gpio_pin_configure_dt(&ext_power_gpio, GPIO_OUTPUT_ACTIVE);
        if (ep_ret < 0) {
            LOG_ERR("Failed to force ext-power GPIO active: %d", ep_ret);
        } else {
            /* Let the rail stabilize before talking to the display. */
            k_msleep(50);
        }
    } else {
        LOG_WRN("Ext-power GPIO device not ready");
    }
#endif

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
