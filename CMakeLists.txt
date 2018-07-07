cmake_minimum_required(VERSION 2.8.11)
PROJECT(fidoconf C)
set(fidoconf_VERSION 1.9.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-g -DDEBUG -D_DEBUG)
  set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "My multi config types" FORCE)
else(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DNDEBUG)
  set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "My multi config types" FORCE)
endif(CMAKE_BUILD_TYPE MATCHES Debug)


if (MSVC)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../out/bin")
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS -D_DLL -D_CONSOLE -D_MAKE_DLL -D_CRT_SECURE_NO_WARNINGS)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/fidoconf;${CMAKE_CURRENT_SOURCE_DIR}/../huskylib;${CMAKE_CURRENT_SOURCE_DIR}/../smapi")
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    list( APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib/Debug" )
  else(CMAKE_BUILD_TYPE MATCHES Debug)
    list( APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib/Release" )
  endif(CMAKE_BUILD_TYPE MATCHES Debug)
else ()
  include_directories("fidoconf")
  ADD_DEFINITIONS(-Wall -DUNIX -DCFGDIR="/etc/ftn")
endif (MSVC)

find_library(husky_LIB NAMES husky PATHS "${CMAKE_FIND_ROOT_PATH}")

message("-------------------------")
message("System            : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message("Compiler          : ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message("Prefix            : ${CMAKE_INSTALL_PREFIX} (run \"cmake . -DCMAKE_INSTALL_PREFIX:PATH=/other\" for other prefix)")
message("libhusky          : ${husky_LIB}")
message("Build type        : ${CMAKE_CONFIGURATION_TYPES}")
message("-------------------------")

SET(fidoconf_SOURCES 
  src/afixcmd.c
  src/afixcmn.c
  src/arealist.c
  src/areatree.c
  src/cfg.c
  src/common.c
#src/fc2aed.c
#src/fc2binkd.c
#src/fc2fgate.c
#src/fc2ged.c
#src/fc2msged.c
#src/fc2sq.c
#src/fc2tor.c
#src/fc2tor_g.c
#src/fecfg146.c
#src/fecfg2fc.c
  src/fidoconf.c
  src/findtok.c
  src/grptree.c
  src/line.c
#  src/linked.c
  src/stat.c
#  src/tparser.c
  src/version.c
)
SET(fidoconf_HEADERS
  fidoconf/afixcmd.h
  fidoconf/arealist.h
  fidoconf/areatree.h
  fidoconf/common.h
  fidoconf/fc2tor_g.h
  fidoconf/fecfg146.h
  fidoconf/fidoconf.h
  fidoconf/findtok.h
  fidoconf/grptree.h
  fidoconf/stat.h
  fidoconf/tokens.h
  fidoconf/version.h
)

ADD_LIBRARY(fidoconfig SHARED ${fidoconf_SOURCES})
ADD_EXECUTABLE(fc2binkd src/fc2binkd.c)
ADD_EXECUTABLE(tparser src/tparser.c)
ADD_EXECUTABLE(linked src/linked.c)
ADD_EXECUTABLE(fc2ged src/fc2ged.c)
set_target_properties(fidoconfig PROPERTIES VERSION ${fidoconf_VERSION})
set_target_properties(fidoconfig PROPERTIES PUBLIC_HEADER "${fidoconf_HEADERS}")
target_link_libraries(fidoconfig ${husky_LIB})
target_link_libraries(linked fidoconfig ${husky_LIB})
target_link_libraries(fc2binkd fidoconfig ${husky_LIB})
target_link_libraries(fc2ged fidoconfig ${husky_LIB})
target_link_libraries(tparser fidoconfig ${husky_LIB})
if (NOT MSVC)
  INSTALL(TARGETS fidoconfig fc2binkd tparser linked fc2ged
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/fidoconf
  )
endif (NOT MSVC)
set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "9")
file(READ cvsdate.h CPACK_PACKAGE_VERSION_PATCH LIMIT 8 OFFSET 17)
string(SUBSTRING ${CPACK_PACKAGE_VERSION_PATCH} 0 8 CPACK_PACKAGE_VERSION_PATCH)
INCLUDE(CPack)
