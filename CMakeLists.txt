cmake_minimum_required(VERSION 2.8.11)
PROJECT(fidoconf C)
set(fidoconf_VERSION 1.9.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(GNUInstallDirs)

option(BUILD_SHARED_LIBS "Build shared libs" ON)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-g -DDEBUG -D_DEBUG)
  set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "My multi config types" FORCE)
else(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DNDEBUG)
  set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "My multi config types" FORCE)
endif(CMAKE_BUILD_TYPE MATCHES Debug)


if (MSVC)
  if (BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-D_DLL -D_MAKE_DLL)
  else()
    set(CompilerFlags
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_C_FLAGS
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_RELEASE
    )
    foreach(CompilerFlag ${CompilerFlags})
      string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
  endif()
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../out/bin")
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS -D_CONSOLE -D_CRT_SECURE_NO_WARNINGS)
  include_directories("${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_SOURCE_DIR}/fidoconf;${CMAKE_CURRENT_SOURCE_DIR}/../huskylib;${CMAKE_CURRENT_SOURCE_DIR}/../smapi")
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    list( APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib/Debug" )
  else(CMAKE_BUILD_TYPE MATCHES Debug)
    list( APPEND CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib/Release" )
  endif(CMAKE_BUILD_TYPE MATCHES Debug)
else ()
  include_directories("fidoconf")
  set(CFGDIR "/etc/ftn" CACHE STRING "CFGDIR")
  ADD_DEFINITIONS(-Wall -DUNIX -DCFGDIR="${CFGDIR}")
endif (MSVC)

find_library(husky_LIB NAMES husky PATHS "${CMAKE_FIND_ROOT_PATH}")
find_library(smapi_LIB NAMES smapi PATHS "${CMAKE_FIND_ROOT_PATH}")

message("-------------------------")
message("System            : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}")
message("Compiler          : ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message("Prefix            : ${CMAKE_INSTALL_PREFIX} (run \"cmake . -DCMAKE_INSTALL_PREFIX:PATH=/other\" for other prefix)")
message("libhusky          : ${husky_LIB}")
message("libsmapi          : ${smapi_LIB}")
message("Build shared      : ${BUILD_SHARED_LIBS}")
message("Build type        : ${CMAKE_CONFIGURATION_TYPES}")
if (NOT MSVC)
  message("Config Dir        : ${CFGDIR}")
endif (NOT MSVC)
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

ADD_LIBRARY(fidoconfig ${fidoconf_SOURCES})
ADD_EXECUTABLE(fc2binkd src/fc2binkd.c)
ADD_EXECUTABLE(tparser src/tparser.c)
ADD_EXECUTABLE(linked src/linked.c)
ADD_EXECUTABLE(fc2ged src/fc2ged.c)
set_target_properties(fidoconfig PROPERTIES VERSION ${fidoconf_VERSION})
set_target_properties(fidoconfig PROPERTIES PUBLIC_HEADER "${fidoconf_HEADERS}")
target_link_libraries(fidoconfig ${husky_LIB} ${smapi_LIB})
target_link_libraries(linked fidoconfig ${husky_LIB} ${smapi_LIB})
target_link_libraries(fc2binkd fidoconfig ${husky_LIB} ${smapi_LIB})
target_link_libraries(fc2ged fidoconfig ${husky_LIB} ${smapi_LIB})
target_link_libraries(tparser fidoconfig ${husky_LIB} ${smapi_LIB})

  include(GNUInstallDirs)
  install(FILES "man/fconf2.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
  install(FILES "man/linked.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
  install(FILES "man/tparser.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
  INSTALL(TARGETS fidoconfig fc2binkd tparser linked fc2ged
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION include/fidoconf
  )
if (NOT MSVC)
  find_program(MAKEINFO
    NAMES makeinfo
    DOC "makeinfo tool"
  )
  if (MAKEINFO)
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/fidoconfig.html
      COMMAND makeinfo --force --html --no-split -o ${PROJECT_BINARY_DIR}/fidoconfig.html ${CMAKE_CURRENT_SOURCE_DIR}/doc/fidoconfig.texi
      COMMENT "Creating documentation from ${CMAKE_CURRENT_SOURCE_DIR}/doc/fidoconfig.texi"
    )
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/fidoconfig.info
      COMMAND makeinfo --force --no-split -o ${PROJECT_BINARY_DIR}/fidoconfig.info ${CMAKE_CURRENT_SOURCE_DIR}/doc/fidoconfig.texi
      COMMENT "Creating documentation from ${CMAKE_CURRENT_SOURCE_DIR}/doc/fidoconfig.texi"
    )
    add_custom_target(docs DEPENDS ${PROJECT_BINARY_DIR}/fidoconfig.html ${PROJECT_BINARY_DIR}/fidoconfig.info)
    install(FILES ${PROJECT_BINARY_DIR}/fidoconfig.html DESTINATION ${CMAKE_INSTALL_DOCDIR})
    install(FILES ${PROJECT_BINARY_DIR}/fidoconfig.info DESTINATION ${CMAKE_INSTALL_INFODIR})
    add_dependencies(fidoconfig docs)
  endif()
  set(CPACK_GENERATOR ${CPACK_GENERATOR} TGZ)
else(NOT MSVC)
  set(CPACK_GENERATOR ${CPACK_GENERATOR} 7Z)
endif (NOT MSVC)

set (CPACK_PACKAGE_VERSION_MAJOR "1")
set (CPACK_PACKAGE_VERSION_MINOR "9")
file(READ cvsdate.h CPACK_PACKAGE_VERSION_PATCH LIMIT 10 OFFSET 17)
string(SUBSTRING ${CPACK_PACKAGE_VERSION_PATCH} 0 10 CPACK_PACKAGE_VERSION_PATCH)
string(REGEX REPLACE "-" "" CPACK_PACKAGE_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS       ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY ">=")
INCLUDE(CPack)
