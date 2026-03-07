#include <lvgl.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/wpm_status.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

static lv_obj_t *anim_label;
static lv_timer_t *anim_timer;

static void status_anim_tick(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    if (anim_label == NULL) {
        return;
    }

    static const char *frames[] = {"-", "\\", "|", "/"};
    static uint8_t frame_index;

    lv_label_set_text(anim_label, frames[frame_index]);
    frame_index = (frame_index + 1U) % 4U;
}

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen = lv_obj_create(NULL);
    const lv_font_t *font_small = lv_theme_get_font_small(screen);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_text_font(zmk_widget_output_status_obj(&output_status_widget), font_small,
                               LV_PART_MAIN);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
    zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
    lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget), LV_ALIGN_TOP_LEFT, 0,
                 12);
    lv_obj_set_style_text_font(zmk_widget_peripheral_status_obj(&peripheral_status_widget),
                               font_small, LV_PART_MAIN);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
    zmk_widget_battery_status_init(&battery_status_widget, screen);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_text_font(zmk_widget_battery_status_obj(&battery_status_widget), font_small,
                               LV_PART_MAIN);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget), font_small,
                               LV_PART_MAIN);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_text_font(zmk_widget_wpm_status_obj(&wpm_status_widget), font_small,
                               LV_PART_MAIN);
#endif

    anim_label = lv_label_create(screen);
    lv_label_set_text(anim_label, "-");
    lv_obj_set_style_text_font(anim_label, font_small, LV_PART_MAIN);
    lv_obj_align(anim_label, LV_ALIGN_TOP_MID, 0, 0);

    if (anim_timer == NULL) {
        anim_timer = lv_timer_create(status_anim_tick, 220, NULL);
    }

    return screen;
}
