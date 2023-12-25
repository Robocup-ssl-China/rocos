set(TARGET_NAME Controller)

set(libs)

find_package(Qt5 COMPONENTS Core Quick SerialPort REQUIRED)
list(APPEND libs Qt5::Core Qt5::Quick Qt5::SerialPort)

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
include_directories(${PROJECT_INCLUDE_DIRS})
## cpp files
file(GLOB CPPFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
## header files
file(GLOB HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
set(SRC
    ${HEADERS}
    ${SOURCES}
    ${CPPFILES}
)

add_executable(${TARGET_NAME} ${SRC} "Controller.qrc")
target_compile_definitions(${TARGET_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${TARGET_NAME} PRIVATE ${libs})
