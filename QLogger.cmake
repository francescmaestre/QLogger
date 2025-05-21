list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/QLogger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/QLoggerWriter.cpp
)

list(APPEND HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/QLogger.h
    ${CMAKE_CURRENT_LIST_DIR}/QLoggerLevel.h
    ${CMAKE_CURRENT_LIST_DIR}/QLoggerWriter.h
    ${CMAKE_CURRENT_LIST_DIR}/QLogger_global.h
)

# Add include and depend paths
include_directories(${CMAKE_CURRENT_LIST_DIR})
