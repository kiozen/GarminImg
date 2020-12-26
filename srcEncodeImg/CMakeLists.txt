set(APPLICATION_NAME encodeimg)
set(CMAKE_AUTOMOC ON)

set(SRCS
    main.cpp
    CFileGmp.cpp
    CFileImg.cpp
    CFileTre.cpp
    CFileLbl.cpp
    CFileRgn.cpp
    CGarminEncoder.cpp
    ../srcDecodeImg/CGarminRaster.cpp
    ../srcDecodeImg/CGarminPolygon.cpp    
    headers/IHdrSubfile.cpp
    headers/CHdrGmapsupp.cpp
    headers/CHdrGmp.cpp
    headers/CHdrTre.cpp
    headers/CHdrRgn.cpp
    headers/CHdrLbl.cpp
    helpers/CLinePreparer.cpp
    helpers/CBitWriter.cpp
    helpers/CKeyGen.cpp
    items/CPolyline.cpp
    items/CTile.cpp
    items/IItem.cpp
    sections/CSection.cpp
    sections/CLbl1.cpp
    sections/CLbl12.cpp
    sections/CLbl28.cpp
    sections/CLbl29.cpp
    sections/CRgn2.cpp
    sections/CRgn3.cpp
    sections/CRgn4.cpp
    sections/CTre1.cpp
    sections/CTre2.cpp
    sections/CTre3.cpp
    sections/CTre7.cpp
    sections/CTre8.cpp
    sections/ISection.cpp
    types/CFatBlock.cpp    
    types/CMapLevel.cpp    
    types/CSubdiv.cpp    
)

set(HDRS
    version.h
    CException.h
    CFileGmp.h
    CFileImg.h
    CFileTre.h
    CFileLbl.h
    CFileRgn.h
    CGarminEncoder.h
    CSubfileTask.h
    ../srcDecodeImg/CGarminRaster.h
    ../srcDecodeImg/CGarminPolygon.h
    headers/CHdrGmapsupp.h
    headers/CHdrGmp.h
    headers/CHdrTre.h
    headers/CHdrRgn.h
    headers/CHdrLbl.h
    headers/IHdrSubfile.h
    helpers/CLinePreparer.h
    helpers/CBitWriter.h
    helpers/CKeyGen.h
    items/CPolyline.h
    items/CTile.h
    items/IItem.h
    sections/CSection.h
    sections/CLbl1.h
    sections/CLbl12.h
    sections/CLbl28.h
    sections/CLbl29.h
    sections/CRgn2.h
    sections/CRgn3.h
    sections/CRgn4.h
    sections/CTre1.h
    sections/CTre2.h
    sections/CTre3.h
    sections/CTre7.h
    sections/CTre8.h
    sections/ISection.h
    types/CFatBlock.h    
    types/CMapLevel.h    
    types/CSubdiv.h
    types/coords.h
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

