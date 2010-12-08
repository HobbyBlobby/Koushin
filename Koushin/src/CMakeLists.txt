set(Koushin_SRCS
  action.cpp
  actionmanager.cpp
  actionparser.cpp
  citizen.cpp
  citizenaction.cpp
  Koushin.cpp
  main.cpp
  population.cpp
)

kde4_add_executable(Koushin ${Koushin_SRCS})
target_link_libraries(Koushin ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY})
install(TARGETS Koushin ${INSTALL_TARGETS_DEFAULT_ARGS} )
