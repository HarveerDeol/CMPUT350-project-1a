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

    # SFML's bundled FreeType calls into HarfBuzz but leaves that dependency
    # undeclared. Declare it so CMake repeats the libraries on the link line.
    # AI-generated (Claude). Prompt: "The build fails at the link step with
    # dozens of 'undefined reference to hb_*' errors from libfreetyped.a as
    # soon as GameEngine calls sf::Font::openFromMemory. How do I fix this?"

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
