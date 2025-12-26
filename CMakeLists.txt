# Project Name
project(Pcsx2)
if(APPLE)
    # 1. Architecture Settings
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "" FORCE)
    
    # 2. Define ARM architecture & Suppress Warnings
    add_compile_definitions(PCSX2_ARCH_ARM64=1)
    add_compile_definitions(PCSX2_ARCH_X86=0)
    add_compile_definitions(FMT_USE_INT128=0)
    
    # These lines ignore warnings to focus on real errors
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
    
    # 3. Core & Feature Settings
    set(USE_WXWIDGETS OFF CACHE BOOL "" FORCE)
    set(PCSX2_CORE ON CACHE BOOL "" FORCE)
    set(USE_OPENGL OFF CACHE BOOL "" FORCE)
    set(USE_VULKAN ON CACHE BOOL "" FORCE)

    # 4. Include Dummy headers
    include_directories(${CMAKE_SOURCE_DIR}/dummy_wx)

    # 5. Targets & Definitions
    if(NOT TARGET wxWidgets::all)
        add_library(wxWidgets::all INTERFACE IMPORTED)
    endif()

    add_definitions(-D__ARM_NEON)
    add_definitions(-DPCSX2_TARGET_ARCH_ARM)
    
    message(STATUS "Build target set to iOS ARM64 - Warnings suppressed")
endif()
cmake_minimum_required(VERSION 3.10...3.17)

# Force iOS and Apple definitions
set(TOP_CMAKE_WAS_SOURCED TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(IOS TRUE)
set(APPLE TRUE)

# set module path
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# include some generic functions
include(Pcsx2Utils)

# Bypassing the OS check which doesn't support iOS by default
# detectOperatingSystem() 

# Set OS manually to bypass restrictions
set(OPERATING_SYSTEM "linux")
set(LINUX TRUE)

# Skip strict compiler version check for now
# check_compiler_version("7.0" "7.0")

#-------------------------------------------------------------------------------
# Include specific module
include(GNUInstallDirs)
# BuildParameters Must be done before SearchForStuff
include(BuildParameters)
include(SearchForStuff)

# Must be done after SearchForStuff
get_git_version_info()
write_svnrev_h()

if(NOT NO_TRANSLATION AND NOT PCSX2_CORE)
	# make the translation
	if(EXISTS "${CMAKE_SOURCE_DIR}/locales")
		add_subdirectory(locales)
	endif()
endif()

# make common
add_subdirectory(common)

# make pcsx2
add_subdirectory(pcsx2)

if (QT_BUILD)
	add_subdirectory(pcsx2-qt)
endif()

# tests
if(ACTUALLY_ENABLE_TESTS)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	add_subdirectory(3rdparty/gtest EXCLUDE_FROM_ALL)
	#add_subdirectory(tests/ctest)
endif()

#-------------------------------------------------------------------------------

# Install some files to ease package creation
if(PACKAGE_MODE)
	INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/bin/resources" DESTINATION "${CMAKE_INSTALL_DATADIR}/PCSX2")

	# set categories depending on system/distribution in pcsx2.desktop
	if(openSUSE)
		set(PCSX2_MENU_CATEGORIES "System;Emulator;")
	else()
		set(PCSX2_MENU_CATEGORIES "Game;Emulator;")
	endif()
	configure_file("${CMAKE_SOURCE_DIR}/linux_various/PCSX2.desktop.in" "${CMAKE_BINARY_DIR}/linux_various/PCSX2.desktop" @ONLY)

	INSTALL(FILES "${CMAKE_BINARY_DIR}/linux_various/PCSX2.desktop" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/linux_various/PCSX2.xpm"     DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pixmaps")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/bin/docs/PCSX2_FAQ.pdf"      DESTINATION "${CMAKE_INSTALL_DOCDIR}")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/bin/docs/Configuration_Guide.pdf"   DESTINATION "${CMAKE_INSTALL_DOCDIR}")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/bin/docs/PCSX2.1"            DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
	if(NOT DISABLE_PCSX2_WRAPPER)
		INSTALL(FILES "${CMAKE_SOURCE_DIR}/linux_various/PCSX2-linux.sh"          DESTINATION "${CMAKE_INSTALL_BINDIR}")
	endif()

else()
	if(NOT DISABLE_PCSX2_WRAPPER)
		# special case to avoid having linux files in windows
		INSTALL(FILES "${CMAKE_SOURCE_DIR}/linux_various/PCSX2-linux.sh"          DESTINATION "${CMAKE_SOURCE_DIR}/bin")
	endif()
endif()
