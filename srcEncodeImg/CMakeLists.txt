set(APPLICATION_NAME imgEncode)
set(CMAKE_AUTOMOC ON)

set(SRCS
    main.cpp
    argv.cpp
    Garmin.cpp
    CEncoder.cpp
    CMap.cpp
    CSubMap.cpp
)

set(HDRS
    argv.h
    Platform.h
    Garmin.h
    CEncoder.h
    CMap.h
    CSubMap.h
)

set(UIS
)

set(RCS
)

qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${PROJ4_INCLUDE_DIRS}
    ${GDAL_INCLUDE_DIRS}
)

if(MSVC)
  add_definitions(-DGARMIN_MAKEDLL -D_CRT_SECURE_NO_DEPRECATE )
endif(MSVC)


add_executable( ${APPLICATION_NAME}
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
)

target_link_libraries( ${APPLICATION_NAME}
    Qt5::Core
    ${GDAL_LIBRARIES}
    ${PROJ4_LIBRARIES}
)

