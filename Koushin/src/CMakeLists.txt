set(Koushin_SRCS
  action.cpp
  actionmanager.cpp
  actionowner.cpp
  actionparser.cpp
  building.cpp
  citizen.cpp
  citizenaction.cpp
  Koushin.cpp
  main.cpp
  player.cpp
  playeraction.cpp
  population.cpp
  town.cpp
  townaction.cpp
  GUI/constructionmenu.cpp
  GUI/resourceinfowidget.cpp
  GUI/townwidget.cpp
)

kde4_add_executable(Koushin ${Koushin_SRCS} ${Koushin_GUI})
target_link_libraries(Koushin ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY})
install(TARGETS Koushin ${INSTALL_TARGETS_DEFAULT_ARGS} )
