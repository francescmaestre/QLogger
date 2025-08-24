file(GLOB SRC_FILES
  src/*.cpp
  include/*.h
)

add_library(QLogger STATIC
    ${SRC_FILES}
)

target_link_libraries(QLogger
    PUBLIC
    Qt::Core
)

target_include_directories(QLogger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
