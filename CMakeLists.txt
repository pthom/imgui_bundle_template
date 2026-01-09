cmake_minimum_required(VERSION 3.28)
project(imgui_bundle_example_integration)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add imgui_bundle
# =================
# 1/  Option 1: if you added imgui_bundle in a subfolder, you can add it to your project with:
if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui_bundle)
    add_subdirectory(external/imgui_bundle EXCLUDE_FROM_ALL)
endif()

## 2/  Option 2: simply fetch imgui_bundle during the build
if (NOT TARGET imgui_bundle)
    message(STATUS "Fetching imgui_bundle...")
    include(FetchContent)
    Set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(imgui_bundle GIT_REPOSITORY https://github.com/pthom/imgui_bundle.git GIT_TAG main EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(imgui_bundle)

    # Uncomment the next line if you which to also automatically fetch and compile OpenCV for immvision support
    # set(IMMVISION_FETCH_OPENCV ON)
endif()




# Build your app
# ==============
# imgui_bundle_add_app is a helper function, similar to cmake's "add_executable"
# Usage:
# imgui_bundle_add_app(app_name file1.cpp file2.cpp ...)
#
# Features:
# * It will automatically link the target to the required libraries (hello_imgui, OpenGl, glad, etc)
# * It will embed the assets (for desktop, mobile, and emscripten apps)
# * It will perform additional customization (app icon and name on mobile platforms, etc)
#
# Now you can build your app with
#     mkdir build && cd build && cmake .. && cmake --build .

# By default, we install in a portable way (i.e. assets and executable are in the same folder)
option(EXAMPLE_INTEGRATION_PORTABLE_INSTALL "Install in a portable way" ON)


if (EXAMPLE_INTEGRATION_PORTABLE_INSTALL)
    # portable installation is the easiest way to install the app
    imgui_bundle_add_app(imgui_bundle_example_integration hello_world.cpp)
else()
    # Disable HelloImGui default install behavior
    set(HELLOIMGUI_ADD_APP_WITH_INSTALL OFF CACHE BOOL "" FORCE)

    imgui_bundle_add_app(imgui_bundle_example_integration hello_world.cpp)

    # If not portable, we have to
    # - install the app manually
    # - install the assets manually
    # - pass the assets location to the app, and set this assets location in main() via the compile definition ASSETS_LOCATION:
    include(GNUInstallDirs)
    install(TARGETS imgui_bundle_example_integration DESTINATION ${CMAKE_INSTALL_BINDIR})
    set(assets_install_dir ${CMAKE_INSTALL_DATADIR}/imgui_bundle_example_integration)
    install(DIRECTORY assets DESTINATION ${assets_install_dir})

    # Calculate relative path from bin to assets and pass to HelloImGui,
    # which will resolve it relative to the executable location at runtime.
    file(RELATIVE_PATH assets_relative_to_bin
        "/${CMAKE_INSTALL_BINDIR}"
        "/${assets_install_dir}/assets")
    target_compile_definitions(imgui_bundle_example_integration PRIVATE ASSETS_LOCATION="${assets_relative_to_bin}")
endif()
