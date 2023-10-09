# Install script for directory: D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/P7")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libp7-baical" TYPE FILE FILES
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/libp7-baical-config.cmake"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/libp7-baical-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Debug/P7x64d.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Release/P7x64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/MinSizeRel/P7x64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/RelWithDebInfo/P7x64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libp7-baical/libp7-baical-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libp7-baical/libp7-baical-targets.cmake"
         "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/CMakeFiles/Export/7ca57e2c173dfaf7d22b8055f545bdec/libp7-baical-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libp7-baical/libp7-baical-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libp7-baical/libp7-baical-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libp7-baical" TYPE FILE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/CMakeFiles/Export/7ca57e2c173dfaf7d22b8055f545bdec/libp7-baical-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libp7-baical" TYPE FILE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/CMakeFiles/Export/7ca57e2c173dfaf7d22b8055f545bdec/libp7-baical-targets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libp7-baical" TYPE FILE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/CMakeFiles/Export/7ca57e2c173dfaf7d22b8055f545bdec/libp7-baical-targets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libp7-baical" TYPE FILE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/CMakeFiles/Export/7ca57e2c173dfaf7d22b8055f545bdec/libp7-baical-targets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/libp7-baical" TYPE FILE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/CMakeFiles/Export/7ca57e2c173dfaf7d22b8055f545bdec/libp7-baical-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/P7" TYPE FILE FILES
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/GTypes.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/P7_Client.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/P7_Cproxy.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/P7_Extensions.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/P7_Telemetry.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/P7_Trace.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/Headers/P7_Version.h"
    "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/P7_Export.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Debug/P7-Test-Speed.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Release/P7-Test-Speed.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/MinSizeRel/P7-Test-Speed.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/RelWithDebInfo/P7-Test-Speed.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Debug/P7-Test-Trace.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Release/P7-Test-Trace.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/MinSizeRel/P7-Test-Trace.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/RelWithDebInfo/P7-Test-Trace.exe")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/Examples/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/VisualStudioProjects/GameEngine/GameEngine/vendor/p7/vcproj/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
