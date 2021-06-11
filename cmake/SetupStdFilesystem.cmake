# h5pp requires the filesystem header (and possibly stdc++fs library)
find_package(Filesystem COMPONENTS Final Experimental)
if (TARGET std::filesystem)
    target_link_libraries(deps INTERFACE std::filesystem)
    list(APPEND H5PP_TARGETS std::filesystem)
elseif(H5PP_PACKAGE_MANAGER MATCHES "cmake|fetch|cpm|conan")
    message(STATUS "Your compiler lacks std::filesystem. A drop-in replacement 'ghc::filesystem' will be downloaded")
    message(STATUS "Read more about ghc::filesystem here: https://github.com/gulrak/filesystem")
    include(cmake/InstallGHCFilesystem.cmake)
    install_ghc_filesystem()
    if(TARGET ghcFilesystem::ghc_filesystem)
        target_link_libraries(deps INTERFACE ghcFilesystem::ghc_filesystem)
        list(APPEND H5PP_TARGETS ghcFilesystem::ghc_filesystem)
    else()
        message(WARNING "Your compiler lacks std::filesystem and installing the drop-in replacement 'ghc::filesystem' failed.")
        message(FATAL_ERROR "<filesystem> header and/or library not found")
    endif()
else()
    message(STATUS "Your compiler lacks std::filesystem. Set H5PP_PACKAGE_MANAGER to 'cmake', 'fetch', 'cpm' or 'conan' to get the ghc::filesystem replacement")
    message(STATUS "Read more about ghc::filesystem here: https://github.com/gulrak/filesystem")
    message(FATAL_ERROR "<filesystem> header and/or library not found")
endif()