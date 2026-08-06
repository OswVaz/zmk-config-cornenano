if(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
  target_sources(app PRIVATE
    src/display/status_screen.c
    src/display/matrix_katakana_8.c
  )
endif()
