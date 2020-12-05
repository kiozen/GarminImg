set(APPLICATION_NAME decodeimg)
set(CMAKE_AUTOMOC ON)

set(SRCS
    main.cpp
    Garmin.cpp
    CGarminDecoder.cpp
    CGarminPolygon.cpp
    CGarminRaster.cpp
    IGarminStrTbl.cpp
    CGarminStrTbl6.cpp
    CGarminStrTbl8.cpp
    CGarminStrTblUtf8.cpp
)

set(HDRS
    version.h
    Garmin.h
    Platform.h
    CGarminDecoder.h
    CGarminPolygon.h
    CGarminRaster.h
    CFuzzyMap.h
    CException.h
    CSubfileTask.h
    IGarminStrTbl.h
    CGarminStrTbl6.h
    CGarminStrTbl8.h
    CGarminStrTblUtf8.h
)

set(UIS
)

set(RCS
)

qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})

include_directories(
    SYSTEM # this prevents warnings from non-QMS headers
    ${CMAKE_BINARY_DIR}
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
    Qt5::Gui
    ${GDAL_LIBRARIES}
    ${PROJ4_LIBRARIES}
)

target_compile_definitions(${APPLICATION_NAME} PUBLIC
    -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVER_MINOR=${PROJECT_VERSION_MINOR}
    -DVER_STEP=${PROJECT_VERSION_PATCH}
    -DAPPLICATION_NAME=${APPLICATION_NAME}
)

