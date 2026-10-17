cmake_minimum_required(VERSION 3.16...3.28)
project(Project1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(engine)
add_subdirectory(galaga)

add_executable(${PROJECT_NAME} ${GALAGA})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/galaga/)
target_link_libraries(${PROJECT_NAME} Engine)

# Collision demo
set(GALAGA_NO_MAIN ${GALAGA})
list(REMOVE_ITEM GALAGA_NO_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/galaga/main.cpp)
add_executable(CollisionDemo ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp ${GALAGA_NO_MAIN})
target_compile_features(CollisionDemo PRIVATE cxx_std_20)
target_include_directories(CollisionDemo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/)
target_include_directories(CollisionDemo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/galaga/)
target_link_libraries(CollisionDemo Engine)
