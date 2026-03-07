zephyr_library()

zephyr_library_sources_ifdef(CONFIG_ZMK_OLED_HW_TEST src/oled_hw_test.c)

if(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM
   AND NOT CONFIG_SHIELD_NICE_OLED
   AND NOT CONFIG_SHIELD_NICE_EPAPER
   AND NOT CONFIG_SHIELD_NICE_CUSTOM)
  zephyr_library_sources(src/custom_status_screen.c)
endif()
