find_package(Chemkit COMPONENTS io md graphics gui REQUIRED)
include_directories(${CHEMKIT_INCLUDE_DIRS})

find_package(Boost COMPONENTS system filesystem iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Qt4 4.6 COMPONENTS QtCore QtGui QtOpenGL REQUIRED)
set(QT_USE_QTGUI TRUE)
include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  builder.cpp
  buildertool.cpp
  builderwindow.cpp
  buildtool.cpp
  displaysettingsdock.cpp
  energyminimizationdock.cpp
  energyminimizer.cpp
  manipulatetool.cpp
  moleculelistdock.cpp
  moleculepropertiesdialog.cpp
  navigatetool.cpp
  toolsettingsdock.cpp
)

set(MOC_HEADERS
  builderwindow.h
  buildtool.h
  displaysettingsdock.h
  energyminimizationdock.h
  energyminimizer.h
  moleculelistdock.h
  moleculepropertiesdialog.h
  toolsettingsdock.h
)

set(UI_FORMS
  builderwindow.ui
  displaysettingsdock.ui
  energyminimizationdock.ui
  moleculelistdock.ui
  moleculepropertiesdialog.ui
  toolsettingsdock.ui
)

qt4_wrap_ui(UI_SOURCES ${UI_FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

add_executable(chemkit-builder ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES})
target_link_libraries(chemkit-builder ${CHEMKIT_LIBRARIES} ${Boost_LIBRARIES} ${QT_LIBRARIES})
install(TARGETS chemkit-builder DESTINATION bin)
