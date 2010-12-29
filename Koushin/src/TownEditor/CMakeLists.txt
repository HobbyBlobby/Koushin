set(Koushin_TownEditor_SRCS
  main.cpp
  editor.cpp
  field.cpp
)

# set(QT_USE_QTSCRIPTTOOLS TRUE)
# set(CMAKE_CXX_FLAGS "-fexceptions")

kde4_add_executable(Koushin_TownEditor ${Koushin_TownEditor_SRCS})
target_link_libraries(Koushin_TownEditor ${KDE4_KDEUI_LIBS} ${KDEGAMES_LIBRARY})
install(TARGETS Koushin_TownEditor ${INSTALL_TARGETS_DEFAULT_ARGS} )
