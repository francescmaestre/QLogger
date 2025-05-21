set(TARGET QLogger)

set(CMAKE_AUTOMOC ON)

include(QLogger.cmake)

qt_add_library(QLogger STATIC
    ${QLOGGER_SOURCES}
)
