set(TARGET_NAME common)
set(CMAKE_AUTOMOC ON)

set(SRCS
    proj_x.cpp
)

set(HDRS
    proj_x.h
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


add_library( ${TARGET_NAME} STATIC
    ${SRCS}
    ${HDRS}
    ${UI_HDRS}
    ${RC_SRCS}
)

target_link_libraries( ${TARGET_NAME}
    Qt5::Core
    Qt5::Gui
    PROJ::proj
)

