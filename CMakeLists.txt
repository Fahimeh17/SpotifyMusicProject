cmake_minimum_required(VERSION 3.16)

project(SpotifyMusicProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        entities/Account.h
        entities/Artist.h
        entities/Listener.h
        entities/Song.h
        entities/Album.h
        entities/Playlist.h
        exceptions/Exceptions.h
        repositories/AbstractRepository.h
        repositories/AccountRepository.h
        repositories/ArtistRepository.h
        repositories/ListenerRepository.h
        repositories/SongRepository.h
        repositories/AlbumRepository.h
        repositories/PlaylistRepository.h
        services/AuthService.h
                services/ArtistService.h
                services/ListenerService.h
                services/SortFilterUtils.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SpotifyMusicProject
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        loginpage.h loginpage.cpp loginpage.ui
        registerpage.h registerpage.cpp registerpage.ui
    )
else()
    if(ANDROID)
        add_library(SpotifyMusicProject SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(SpotifyMusicProject
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(SpotifyMusicProject PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SpotifyMusicProject PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SpotifyMusicProject)
endif()
set_target_properties(SpotifyMusicProject PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SpotifyMusicProject
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SpotifyMusicProject)
endif()