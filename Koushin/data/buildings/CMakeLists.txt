set (koushin_BLDGS
  lumbermill.config
  warehouse.config
)

install(FILES ${koushin_BLDGS} DESTINATION ${DATA_INSTALL_DIR}/koushin/data/buildings)
