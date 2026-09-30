if (CMAKE_VERSION VERSION_LESS "3.28")
    include(${CMAKE_SOURCE_DIR}/cmake/ImportSFML.cmake)
else ()
    include(FetchContent)
    FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.1.0
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM
    )
    set(SFML_USE_SYSTEM_DEPS OFF)
    FetchContent_MakeAvailable(SFML)

    # AI generated fix:
    # "My CMake build fails with this issue, how do I fix this?
    #
    #    [100%] Linking CXX executable Project1.exe
    #    ld.exe: _deps/sfml-build/lib/libfreetyped.a(unity_0_c.c.obj):
    #    in function `ft_hb_ft_reference_table':
    #    ft-hb-ft.c:67: undefined reference to `hb_blob_create'
    #    ft-hb-ft.c:86: undefined reference to `hb_face_create'
    #    ft-hb-ft.c:87: undefined reference to `hb_blob_destroy'
    #    ft-hb-ft.c:89: undefined reference to `hb_face_create_for_tables'
    #    ... (dozens more hb_* symbols)
    #    collect2.exe: error: ld returned 1 exit status"
    # Reflection: Successfully fixes the build errors in a simple way.
    # Ensures the font libraries are linked properly.
    if (TARGET freetype AND TARGET harfbuzz)
        target_link_libraries(freetype INTERFACE harfbuzz)
    endif ()
endif ()

file(GLOB ENGINE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(Engine STATIC ${ENGINE})

target_include_directories(Engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(Engine PRIVATE cxx_std_20)
target_link_libraries(Engine PUBLIC SFML::Graphics)
