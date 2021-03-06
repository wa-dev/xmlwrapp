cmake_minimum_required(VERSION 2.8)

PROJECT (xmlwrapp)
enable_language(CXX)
set(XMLWRAPP_VERSION_MAJOR 0)
set(XMLWRAPP_VERSION_MINOR 8)
set(XMLWRAPP_VERSION_PATCH 1)
set(XMLWRAPP_VERSION "${XMLWRAPP_VERSION_MAJOR}.${XMLWRAPP_VERSION_MINOR}.${XMLWRAPP_VERSION_PATCH}")

enable_testing ()

#set(CMAKE_VERBOSE_MAKEFILE ON)

option(XMLWRAPP_DOC "Build and install the documentation (requires Doxygen)" ON)
option(XMLWRAPP_TEST "Build test" ON)
option(XMLWRAPP_BUILD_XSLTWRAPP "Build libxsltwrapp library" ON)
option(XMLWRAPP_NO_CPP11 "Prevent the use of C++11 features even if they are deemed to be available")

set(INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(INSTALL_DOC_DIR ${CMAKE_INSTALL_PREFIX}/share/xmlwrapp/doc)

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/Xmlwrapp)
endif(WIN32 AND NOT CYGWIN)
set(INSTALL_CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/${DEF_INSTALL_CMAKE_DIR})

set(CMAKE_CXX_COMPILER_VISIBILITY_PRESET hidden)

if(XMLWRAPP_NO_CPP11)
  add_definitions( -DXMLWRAPP_NO_CPP11 )
endif(XMLWRAPP_NO_CPP11)

find_package(PkgConfig)

pkg_search_module(LIBXML2 REQUIRED libxml-2.0 >= 2.4.28)

if (LIBXML2_FOUND)
    include_directories( ${LIBXML2_INCLUDE_DIRS} )
    link_directories( ${LIBXML2_LIBRARY_DIRS} )
endif (LIBXML2_FOUND)

#find_package (Boost COMPONENTS pool unit_test_framework iostreams)
# -- removing pool from the components as pool currently has no real checks
find_package (Boost COMPONENTS unit_test_framework iostreams)
if (BOOST_FOUND)
    include_directories( ${Boost_INCLUDE_DIRS} )
endif(BOOST_FOUND)

if( XMLWRAPP_BUILD_XSLTWRAPP )
    pkg_search_module(LIBXSLT REQUIRED libxslt >= 1.1.6)
    include_directories( ${LIBXSLT_INCLUDE_DIRS} )
endif( XMLWRAPP_BUILD_XSLTWRAPP )


add_subdirectory(include)
add_subdirectory(src)

#TODO install examples in <prefix>/share/xmlwrapp/examples/01-.. 02-.. etc
add_subdirectory(examples)

if( XMLWRAPP_TEST )
    #TODO configure with ctest
    add_subdirectory(tests)
endif( XMLWRAPP_TEST)

if( XMLWRAPP_DOC )
    #TODO build and install docs in <prefix>/share/xmlwrapp/doc/html/
    add_subdirectory(docs)
endif( XMLWRAPP_DOC )


#----------------------------------------------
# pkg-config files use autoconf variable names
# placed in function to keep project namespace free of autoconf build sys var names
function(config_pkgconf 
          prefix libdir includedir binarydir VERSION 
          xml2dirs xml2libs 
          xsltdirs xsltlibs )


    set(exec_prefix ${prefix})
    set(XML2_LIBS "")
    foreach(p ${xml2dirs})
        list(APPEND XML2_LIBS "-L${p}")
    endforeach()
    foreach(lib ${xml2libs})
        list(APPEND XML2_LIBS "-l${lib}")
    endforeach()

    set(LEGACY_LINK_FLAGS "-lxmlwrapp ${XML2_LIBS}")

    if( XMLWRAPP_BUILD_XSLTWRAPP )
        set(XSLT_LIBS "")
        foreach(p ${xsltdirs})
            list(APPEND XSLT_LIBS "-L${p}")
        endforeach()
        foreach(lib ${xsltlibs})
            list(APPEND XSLT_LIBS "-l${lib}")
        endforeach()
        list(APPEND LEGACY_LINK_FLAGS "-lxsltwrapp ${XSLT_LIBS}")
    endif( XMLWRAPP_BUILD_XSLTWRAPP )


    configure_file( "xmlwrapp.pc.in" "xmlwrapp.pc" @ONLY)
    install (FILES "${binarydir}/xmlwrapp.pc" DESTINATION lib/pkgconfig)

    configure_file( "xmlwrapp-config.in" "xmlwrapp-config" @ONLY)
    install (PROGRAMS "${binarydir}/xmlwrapp-config" 
             DESTINATION bin)

    if( XMLWRAPP_BUILD_XSLTWRAPP )
        configure_file( "xsltwrapp.pc.in" "xsltwrapp.pc" @ONLY)
        install (FILES "${binarydir}/xsltwrapp.pc" DESTINATION lib/pkgconfig)
    endif( XMLWRAPP_BUILD_XSLTWRAPP )

endfunction()

config_pkgconf("${CMAKE_INSTALL_PREFIX}"
               "${INSTALL_LIBRARY_DIR}"
               "${INSTALL_INCLUDE_DIR}"
               "${PROJECT_BINARY_DIR}"
               "${XMLWRAPP_VERSION}"
               "${LIBXMLS2_LIBRARY_DIRS}"
               "${LIBXML2_LIBRARIES}"
               "${LIBXSLT_LIBRARY_DIRS}"
               "${LIBXSLT_LIBRARIES}"
               )
#----------------------------------------------



# add all targets to build-tree export set 
export(TARGETS xmlwrapp_shared xmlwrapp_static FILE "${PROJECT_BINARY_DIR}/XmlwrappTargets.cmake")
if( XMLWRAPP_BUILD_XSLTWRAPP )
    export(TARGETS xsltwrapp_shared xsltwrapp_static APPEND FILE "${PROJECT_BINARY_DIR}/XmlwrappTargets.cmake")
endif( XMLWRAPP_BUILD_XSLTWRAPP )

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE Xmlwrapp)

include (CMakePackageConfigHelpers)


file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
file(RELATIVE_PATH REL_LIBRARY_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_LIBRARY_DIR}")

set(CONF_INCLUDE_DIR "\${XMLWRAPP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIBRARY_DIR "\${XMLWRAPP_CMAKE_DIR}/${REL_LIBRARY_DIR}")

configure_package_config_file(XmlwrappConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/XmlwrappConfig.cmake
                              INSTALL_DESTINATION ${INSTALL_CMAKE_DIR})
configure_file( XmlwrappConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/XmlwrappConfigVersion.cmake" @ONLY)

install(FILES 
          ${PROJECT_BINARY_DIR}/XmlwrappConfig.cmake
          ${PROJECT_BINARY_DIR}/XmlwrappConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKE_DIR}
        COMPONENT dev)

install(EXPORT XmlwrappTargets DESTINATION "${DEF_INSTALL_CMAKE_DIR}" COMPONENT dev)



set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xmlwrapp is a modern style C++ library for working with XML data, built atop the venerable libxml2 C library.")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${XMLWRAPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${XMLWRAPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${XMLWRAPP_VERSION_PATCH})

set(CPACK_SOURCE_IGNORE_FILES
    "~$"
    "^.gitignore$"
    "^*.git"
)
include(CPack)
