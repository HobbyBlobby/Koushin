set(Koushin_SRCS
  action.cpp
  actionmanager.cpp
  actionobject.cpp
  actionparser.cpp
  building.cpp
  citizen.cpp
  field.cpp
  game.cpp
  main.cpp
  player.cpp
  population.cpp
  town.cpp
  GUI/constructionmenu.cpp
  GUI/fielditem.cpp
  GUI/resourceinfowidget.cpp
  GUI/townwidget.cpp
)

set(QT_USE_QTSCRIPTTOOLS TRUE)
set(CMAKE_CXX_FLAGS "-fexceptions")

kde4_add_executable(Koushin ${Koushin_SRCS} ${Koushin_GUI})
target_link_libraries(Koushin ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY} ${QT_QTSCRIPT_TOOLS_LIBRARY_RELEASE})
install(TARGETS Koushin ${INSTALL_TARGETS_DEFAULT_ARGS} )
