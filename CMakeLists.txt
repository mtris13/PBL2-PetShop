cmake_minimum_required(VERSION 3.16)

project(PetshopGUI VERSION 0.1 LANGUAGES CXX)

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
        clientdashboard.cpp
        clientdashboard.h
        clientdashboard.ui
        billdialog.cpp  # <-- THÊM DÒNG NÀY
        billdialog.h    # <-- THÊM DÒNG NÀY
)

# ... (Mọi thứ từ dòng 1 đến 17 vẫn giữ nguyên) ...

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PetshopGUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        clientdashboard.h clientdashboard.cpp clientdashboard.ui
        images.qrc
        billdialog.h billdialog.cpp billdialog.ui
        staffdashboard.h staffdashboard.cpp staffdashboard.ui
        admindashboard.h admindashboard.cpp admindashboard.ui
    )
# ... (Phần 'else' cho Qt5 và Android từ dòng 24 đến 40) ...
else()
    if(ANDROID)
        add_library(PetshopGUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(PetshopGUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# === BẮT ĐẦU THÊM VÀO TỪ ĐÂY ===

# 1. Thêm đường dẫn thư mục "include"
target_include_directories(PetshopGUI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 2. Tìm tất cả các file .cpp trong thư mục "src" đã copy
#    (Lệnh này sẽ tự động tìm tất cả file .cpp trong các thư mục con)
file(GLOB_RECURSE BACKEND_SOURCES
    "src/*.cpp"
)

# 3. Thêm các file .cpp của backend vào dự án
target_sources(PetshopGUI PRIVATE
    ${BACKEND_SOURCES}
)

# === KẾT THÚC THÊM TẠI ĐÂY ===


target_link_libraries(PetshopGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# ... (Phần còn lại của file từ dòng 50 trở đi giữ nguyên) ...

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.PetshopGUI)
endif()
set_target_properties(PetshopGUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PetshopGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PetshopGUI)
endif()
