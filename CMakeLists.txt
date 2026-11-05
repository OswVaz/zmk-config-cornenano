if(CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
  target_include_directories(app PRIVATE src/display)

  target_sources(app PRIVATE
    src/display/status_screen.c
    src/display/battery_alert.c
  )

  # Biblioteca de animaciones: Kconfig deja activa solo una.
  if(CONFIG_CORNE_ANIMATION_MATRIX_RAIN)
    target_sources(app PRIVATE
      src/display/animations/matrix_rain/matrix_rain.c
      src/display/animations/matrix_rain/matrix_katakana_6.c
    )
  endif()

  if(CONFIG_CORNE_ANIMATION_NAVES)
    target_sources(app PRIVATE
      src/display/animations/naves/naves.c
      src/display/animations/naves/naves_core.c
      src/display/animations/naves/naves_sync.c
    )
  endif()
endif()
