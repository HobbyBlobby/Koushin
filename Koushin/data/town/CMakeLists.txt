set (koushin_TOWNS
  town.config
)

set (koushin_TOWN_IMAGES
  landscape.jpg
)

install(FILES ${koushin_TOWNS} ${koushin_TOWN_IMAGES} DESTINATION ${DATA_INSTALL_DIR}/koushin/data/towns)
