CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

IF(POLICY CMP0043)
        # Don't ignore COMPILE_DEFINITIONS_{CONFIG} directives, as these are used
        # by Qt.
        CMAKE_POLICY(SET CMP0043 OLD)
ENDIF()

IF (POLICY CMP0071)
        CMAKE_POLICY(SET CMP0071 OLD)
ENDIF()

if(${CMAKE_VERSION} VERSION_EQUAL "3.11.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.11.0")
IF (POLICY CMP0072)
        CMAKE_POLICY(SET CMP0072 OLD)
ENDIF()
endif()

PROJECT("tesseract2djvused")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

FIND_PACKAGE(Qt5Core ${Qt_MIN_VERSION} REQUIRED)
FIND_PACKAGE(Qt5Xml ${Qt_MIN_VERSION} REQUIRED)

SET( sources
     main.cpp
     hocr2djvused.cpp
)

ADD_EXECUTABLE(tesseract2djvused ${sources} )

QT5_USE_MODULES(tesseract2djvused Xml)
