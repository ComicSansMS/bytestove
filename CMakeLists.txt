cmake_minimum_required(VERSION 3.14)

project(filestove)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/W4>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/permissive->)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>)
add_compile_definitions($<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING>)
add_compile_definitions($<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING>)
add_compile_definitions($<$<CXX_COMPILER_ID:MSVC>:WIN32_LEAN_AND_MEAN>)
add_compile_definitions($<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>)
add_compile_options($<$<CXX_COMPILER_ID:GNU,AppleClang,Clang>:-Wall>)
add_compile_options($<$<CXX_COMPILER_ID:GNU,AppleClang,Clang>:-pedantic>)
add_compile_options($<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>)
add_link_options($<$<CXX_COMPILER_ID:Clang>:-stdlib=libc++>)

set(QT5_BASE_ROOT "" CACHE PATH "Qt5 installation root directory.")
find_package(Qt5 REQUIRED COMPONENTS Widgets
    HINTS ${QT5_BASE_ROOT} $ENV{QT5_ROOT}/5.12.2/msvc2017_64 $ENV{QT5_ROOT}/5.12.0/msvc2017_64
)

set(GHULBUS_BASE_ROOT "" CACHE PATH "GhulbusBase installation root directory.")
if(GHULBUS_BASE_ROOT)
    list(APPEND CMAKE_PREFIX_PATH ${GHULBUS_BASE_ROOT}/cmake)
endif()
find_package(gbBase REQUIRED)


set(FILESTOVE_SOURCE_DIRECTORY ${PROJECT_SOURCE_DIR}/src)

set(FILESTOVE_SOURCE_FILES
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/main.cpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/activity_monitor.cpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/cook.cpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/file_collector.cpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/stove.cpp
)

set(FILESTOVE_HEADER_FILES
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/activity_monitor.hpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/cook.hpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/exceptions.hpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/file_collector.hpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/stove.hpp
)

set(FILESTOVE_QT_SOURCE_FILES
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/ui/cooker.cpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/ui/widget.cpp
)

set(FILESTOVE_QT_HEADER_FILES
)

set(FILESTOVE_QT_MOC_HEADER_FILES
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/ui/cooker.hpp
    ${FILESTOVE_SOURCE_DIRECTORY}/filestove/ui/widget.hpp
)
qt5_wrap_cpp(FILESTOVE_QT_MOC_SOURCE_FILES ${FILESTOVE_QT_MOC_HEADER_FILES})

source_group(moc FILES
    ${FILESTOVE_QT_MOC_SOURCE_FILES}
)

#qt5_add_resources(FILESTOVE_QT_RESOURCE_FILES ${PROJECT_SOURCE_DIR}/resources/filestove.qrc)
#source_group(resources FILES
#    ${FILESTOVE_QT_RESOURCE_FILES}
#)


add_executable(filestove WIN32
    ${FILESTOVE_SOURCE_FILES}
    ${FILESTOVE_HEADER_FILES}
    ${FILESTOVE_QT_SOURCE_FILES}
    ${FILESTOVE_QT_HEADER_FILES}
    ${FILESTOVE_QT_MOC_HEADER_FILES}
    ${FILESTOVE_QT_MOC_SOURCE_FILES}
    ${FILESTOVE_QT_RESOURCE_FILES}
)
target_include_directories(filestove PUBLIC ${FILESTOVE_SOURCE_DIRECTORY})
target_link_libraries(filestove PUBLIC
    Ghulbus::gbBase
    Qt5::Core
    Qt5::Widgets
    Pdh
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT filestove)

if(WIN32)
    function(getPDBForDLL DLL_PATH OUT_VAR)
        get_filename_component(dll_dir ${DLL_PATH} DIRECTORY)
        get_filename_component(dll_we ${DLL_PATH} NAME_WE)
        set(${OUT_VAR} "${dll_dir}/${dll_we}.pdb" PARENT_SCOPE)
    endfunction()


    function(getQt5Dlls QT_TARGET OUT_VAR)
        unset(DLLS)
        get_property(tmp TARGET ${QT_TARGET} PROPERTY IMPORTED_LOCATION_DEBUG)
        list(APPEND DLLS ${tmp})
        getPDBForDLL(${tmp} tmp_pdb)
        if (EXISTS ${tmp_pdb})
            list(APPEND DLLS ${tmp_pdb})
        endif()
        get_property(tmp TARGET ${QT_TARGET} PROPERTY IMPORTED_LOCATION_RELEASE)
        list(APPEND DLLS ${tmp})
        get_property(deps TARGET ${QT_TARGET} PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG)
        foreach(dep ${deps})
            if(TARGET ${dep})
                getQt5Dlls(${dep} tmp)
                list(APPEND DLLS ${tmp})
            endif()
        endforeach()
        set(result ${${OUT_VAR}})
        list(APPEND result ${DLLS})
        list(REMOVE_DUPLICATES result)
        set(${OUT_VAR} ${result} PARENT_SCOPE)
    endfunction()

    getQt5Dlls(Qt5::Widgets qt_DLLS)

    file(COPY ${qt_DLLS} DESTINATION ${PROJECT_BINARY_DIR})

endif()
