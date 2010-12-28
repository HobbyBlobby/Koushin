set (koushin_BLDGS
  forester.config
  lumberjack.config
  lumbermill.config
  warehouse.config
)
set (koushin_IMGS
  ../blender/landscape.jpg
)

install(FILES ${koushin_BLDGS} DESTINATION ${DATA_INSTALL_DIR}/koushin/data/buildings)
install(FILES ${koushin_IMGS} DESTINATION ${DATA_INSTALL_DIR}/koushin/data/images)