
set(SPDLOG_NO_CMAKE_PACKAGE_REGISTRY TRUE)
find_package(spdlog 1.3.1)


if(TARGET spdlog::spdlog)
#    return()
elseif(TARGET spdlog)
    add_library(spdlog::spdlog ALIAS spdlog)
#    return()
elseif (DOWNLOAD_MISSING)
    message(STATUS "Spdlog will be installed into ${CMAKE_INSTALL_PREFIX}")
    include(${PROJECT_SOURCE_DIR}/cmake-modules/BuildDependency.cmake)
    build_dependency(spdlog "")
    find_package(spdlog 1.3
            HINTS ${CMAKE_BINARY_DIR}/h5pp-deps-install
            PATH_SUFFIXES ${spdlog_suffix}${CMAKE_INSTALL_LIBDIR}/cmake/spdlog spdlog spdlog/${CMAKE_INSTALL_LIBDIR} spdlog/share spdlog/cmake
            NO_DEFAULT_PATH NO_CMAKE_PACKAGE_REGISTRY )

    if(TARGET spdlog::spdlog)
        message(STATUS "spdlog installed successfully")
#        include(cmake-modules/PrintTargetProperties.cmake)
#        print_target_properties(spdlog::spdlog)
    else()
        message(STATUS "config_result: ${config_result}")
        message(STATUS "build_result: ${build_result}")
        message(FATAL_ERROR "Spdlog could not be downloaded.")
    endif()

else()
    message(STATUS "Dependency spdlog not found and DOWNLOAD_MISSING is OFF")

endif()