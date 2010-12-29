set (koushin_BLDGS
  forester.config
  lumberjack.config
  lumbermill.config
  warehouse.config
)

install(FILES ${koushin_BLDGS} DESTINATION ${DATA_INSTALL_DIR}/koushin/data/buildings)