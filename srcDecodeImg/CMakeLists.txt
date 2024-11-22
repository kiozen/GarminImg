set(TARGET_NAME decodeimg)
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
    ../srcCommon
)



if(MSVC)
  add_definitions(-DGARMIN_MAKEDLL -D_CRT_SECURE_NO_DEPRECATE )
endif(MSVC)


add_executable( ${TARGET_NAME}
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
)

target_link_libraries( ${TARGET_NAME}
    Qt5::Core
    Qt5::Gui
    GDAL::GDAL
    PROJ::proj
    common
)

target_compile_definitions(${TARGET_NAME} PUBLIC
    -DVER_MAJOR=${PROJECT_VERSION_MAJOR}
    -DVER_MINOR=${PROJECT_VERSION_MINOR}
    -DVER_STEP=${PROJECT_VERSION_PATCH}
    -DTARGET_NAME=${TARGET_NAME}
)

