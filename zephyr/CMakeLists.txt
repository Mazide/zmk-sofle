zephyr_library()

zephyr_library_sources_ifdef(CONFIG_ZMK_OLED_HW_TEST src/oled_hw_test.c)
