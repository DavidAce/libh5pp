function(build_dependency dep_name install_dir extra_flags)
    set(build_dir    ${CMAKE_BINARY_DIR}/h5pp-deps-build/${dep_name})
    if (H5PP_DEPS_IN_SUBDIR)
        set(install_dir ${install_dir}/${dep_name})
        mark_as_advanced(install_dir)
    endif ()

    execute_process( COMMAND  ${CMAKE_COMMAND} -E make_directory ${build_dir})
    execute_process(
            COMMAND  ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX:PATH=${install_dir}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
            ${extra_flags}
            -G "${CMAKE_GENERATOR}"
            -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
            ${PROJECT_SOURCE_DIR}/cmake/external_${dep_name}
            WORKING_DIRECTORY ${build_dir}
            RESULT_VARIABLE config_result
            ERROR_VARIABLE  config_error

    )
    if(${config_result})
        message(STATUS "Got non-zero exit code while configuring ${dep_name}")
        message(STATUS  "build_dir         : ${build_dir}")
        message(STATUS  "install_dir       : ${install_dir}")
        message(STATUS  "extra_flags       : ${extra_flags}")
        message(STATUS  "config_result     : ${config_result}")
        message(STATUS  "Output saved to ${build_dir}/stdout and ${build_dir}/stderr")
        file(APPEND ${build_dir}/stdout ${config_result})
        file(APPEND ${build_dir}/stderr ${config_error})
        if(CMAKE_VERBOSE_MAKEFILE)
            message(STATUS "Contents of stdout: \n  ${config_result} \n")
            message(STATUS "Contents of stderr: \n  ${config_error}  \n")
        endif()
    endif()


    include(cmake/GetNumThreads.cmake)
    get_num_threads(num_threads)
    execute_process(COMMAND  ${CMAKE_COMMAND} --build . --parallel ${num_threads}
            WORKING_DIRECTORY "${build_dir}"
            RESULT_VARIABLE build_result
            ERROR_VARIABLE  build_error
    )

    if(build_result)
        message(STATUS "Got non-zero exit code while building ${dep_name}")
        message(STATUS  "build_dir         : ${build_dir}")
        message(STATUS  "install_dir       : ${install_dir}")
        message(STATUS  "extra_flags       : ${extra_flags}")
        message(STATUS  "build_result      : ${build_result}")
        message(STATUS  "Output saved to ${build_dir}/stdout and ${build_dir}/stderr")
        file(APPEND ${build_dir}/stdout ${build_result})
        file(APPEND ${build_dir}/stderr ${build_error})
        if(CMAKE_VERBOSE_MAKEFILE)
            message(STATUS "Contents of stdout: \n  ${build_result} \n")
            message(STATUS "Contents of stderr: \n  ${build_error} \n")
        endif()
    endif()

    # Copy the install manifest if it exists
    file(GLOB_RECURSE INSTALL_MANIFEST "${build_dir}/*/install_manifest.txt")
    if(INSTALL_MANIFEST)
        message(STATUS "Copying install manifest: ${INSTALL_MANIFEST}")
        configure_file(${INSTALL_MANIFEST} ${CMAKE_CURRENT_BINARY_DIR}/install_manifest_${dep_name}.txt)
    endif()

endfunction()