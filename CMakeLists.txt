cmake_minimum_required(VERSION 3.0)

set(lib_base_name BookFiler-Module-Recognize)
project(${lib_base_name} VERSION 1.0.0 LANGUAGES CXX)

# Configurable Options
OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_STATIC_LIBS "Build static libraries" ON)

find_package(Boost 1.56 REQUIRED COMPONENTS
             system filesystem)

# third party
set(INCLUDE_DIRECTORIES
  third_party/rapidjson/include/
)

# Set up source files
set(SOURCES
  src/Module.cpp
  src/core/recognizeModel.cpp
)

set(HEADERS
  src/Module.hpp
  src/Interface.hpp
  src/core/config.hpp
  src/core/recognizeModel.hpp
)

set(SHARED_COMPILE_DEFINITIONS
  -DSHARED_LIBRARY
)

set(STATIC_COMPILE_DEFINITIONS
  -DSTATIC_LIBRARY
)

set(LIBRARIES ${LIBRARIES}
    # Boost
    Boost::system
    Boost::filesystem
)

if(WIN32)
    set(LIBRARIES ${LIBRARIES}
        # Windows Libraries
        z
        stdc++
        gcc
        uuid

        windowscodecs
        opengl32
        gdi32
        comdlg32
        oleaut32
        imm32
        winmm
        winspool
        kernel32
        user32
        shell32
        ole32
        advapi32
        ws2_32
        mingw32
        rpcrt4
    )
elseif(UNIX)
    set(LIBRARIES ${LIBRARIES}
        # Unix Libraries
        z
        stdc++
        gcc
        uuid

        pthread
        glib-2.0
        gthread-2.0
        dl
        m
    )
endif()


set(SHARED_LINK_LIBRARIES ${LIBRARIES})
set(STATIC_LINK_LIBRARIES ${LIBRARIES})

set(CMAKE_CXX_STANDARD 17)

# Set up library names
if(BUILD_SHARED_LIBS)
  set(lib_shared_name ${lib_base_name}LibShared)
  set(lib_shared_name_decorated ${lib_shared_name}${JKQtPlotter_LIBNAME_ADDITION})
  set(${lib_base_name}_SHARED_LIBRARY ${lib_shared_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_shared_name}" )
endif(BUILD_SHARED_LIBS)

if(BUILD_STATIC_LIBS)
  set(lib_name ${lib_base_name}Lib)
  set(lib_name_decorated ${lib_name}${JKQtPlotter_LIBNAME_ADDITION})
  set(${lib_base_name}_STATIC_LIBRARY ${lib_name_decorated}  CACHE STRING "name of static library for ${lib_base_name}" FORCE)
  message( STATUS "-- Building ${lib_name}" )
endif(BUILD_STATIC_LIBS)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include(CMakePackageConfigHelpers)

if(BUILD_SHARED_LIBS)
  add_library(${lib_shared_name} SHARED  ${SOURCES} ${HEADERS})
  target_compile_features(${lib_shared_name} PUBLIC cxx_std_17)
  set_property(TARGET ${lib_shared_name} PROPERTY VERSION "${PROJECT_VERSION}")
  set_property(TARGET ${lib_shared_name} PROPERTY OUTPUT_NAME  "${lib_shared_name_decorated}")
  target_include_directories(${lib_shared_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  set_target_properties(${lib_shared_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")
  if(UNIX)
      #set_property(TARGET ${lib_shared_name} PROPERTY POSITION_INDEPENDENT_CODE ON)
  endif()
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_shared_name}Version.cmake
    VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion )

  # Compile Definitions
  target_compile_definitions(${lib_shared_name} PUBLIC ${SHARED_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_shared_name} PUBLIC ${SHARED_LINK_LIBRARIES})
endif()

if(BUILD_STATIC_LIBS)
  add_library(${lib_name} STATIC  ${SOURCES} ${HEADERS})
  target_compile_features(${lib_name} PUBLIC cxx_std_17)
  set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
  set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME  "${lib_name_decorated}")
  target_include_directories(${lib_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
    VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion )

  # Compile Definitions
  target_compile_definitions(${lib_name} PUBLIC ${STATIC_COMPILE_DEFINITIONS})
  # Libraries to link
  target_link_libraries(${lib_name} PUBLIC ${STATIC_LINK_LIBRARIES})
endif()

# Post build
if(BUILD_SHARED_LIBS AND PARENT_RELEASE_DIR)
    add_custom_command(TARGET ${lib_shared_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PARENT_RELEASE_DIR}/modules/")
    add_custom_command(TARGET ${lib_shared_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${lib_shared_name}> "${PARENT_RELEASE_DIR}/modules/")
endif()

# Installation
if(BUILD_SHARED_LIBS)
  message(STATUS "INSTALL: ${lib_shared_name} TO root=${CMAKE_BINARY_DIR} and current=${CMAKE_CURRENT_BINARY_DIR}")
  install(TARGETS ${lib_shared_name}
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/modules"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/modules"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/modules"
  )
endif()

if(BUILD_STATIC_LIBS)
  install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/lib"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/lib"
    LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/lib"
  )
endif()

install(FILES ${HEADERS}
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/headers"
COMPONENT Headers)

install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${CMAKE_INSTALL_PREFIX}/${BIN_NAME}/doc/" RENAME "${lib_name}_LICENSE.txt" )
