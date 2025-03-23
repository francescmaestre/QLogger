/**
 * @file main.cpp
 * @class main
 * @author Benoît MOUFLIN
 * @date 2020-11-02
 *
 * @brief The main function
 *
 * @module QLoggerTest
 * @note This is a test and demo project of the QLogger library.
 *
 * Copyright (c) 2020 AphaseDev. All rights reserved.
 * https://github.com/AphaseDev
 */
#include "QLogger.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QThread>

using namespace QLogger;


static const QString g_file1 = QStringLiteral("test1.log");
static const QString g_file2 = QStringLiteral("test2.log");
static const QString g_file3 = QStringLiteral("test3.log");

static const QString g_module1 = QStringLiteral("QLoggerTest");
static const QString g_module2 = QStringLiteral("TestiiTest");
static const QString g_module3 = QStringLiteral("QLoggerTest2");
static const QString g_module4 = QStringLiteral("TestiiTest2");
static const QString g_module5 = QStringLiteral("TestiiTest3");
static const QString g_module6 = QStringLiteral("TestiiTest4");

// --- Default features ---

void logDefault()
{
    auto *l_manager = QLoggerManager::getInstance();

    // Create destination with a given file name
    l_manager->addDestination(g_file1, g_module1, LogLevel::Debug);
    QLog_Debug(g_module1, QStringLiteral("This is a debug log message 0."));
    QLog_Debug(g_module1, QStringLiteral("This is a debug log message 1.."));
    QLog_Debug(g_module1, QStringLiteral("This is a debug log message 2..."));
    QLog_Debug(g_module1, QStringLiteral("This is a debug log message 3...."));

    // Try to create another module of the same name but with a different file name and level - ignoring
    l_manager->addDestination(g_file2, g_module1, LogLevel::Debug);
    // The log message is written into the file1
    QLog_Debug(g_module1, QStringLiteral("This is a debug log message 0."));

    // The module doesn't exist yet - messages are enqueued
    QLog_Debug(g_module2, QStringLiteral("This is a TestiiTest from uncreated module."));
    QThread::msleep(500);  // To test uncreated module case
    // Create the corresponding module
    l_manager->addDestination(g_file2, g_module2, LogLevel::Debug);
    QLog_Debug(g_module2, QStringLiteral("This is a TestiiTest on created module."));
}

// --- Custom features ---

void logCustom()
{
    auto *l_manager = QLoggerManager::getInstance();

    // Create module at the default destination folder and full mode (console and file)
    l_manager->addDestination(g_file3, g_module3, LogLevel::Debug, QString(), LogMode::Full);
    QLog_Debug(g_module3, QStringLiteral("This is a debug log message 0."));
    QLog_Debug(g_module3, QStringLiteral("This is a debug log message 1.."));
    QLog_Debug(g_module3, QStringLiteral("This is a debug log message 2..."));
    QLog_Debug(g_module3, QStringLiteral("This is a debug log message 3...."));

    // The module doesn't exist yet - messages are enqueued
    QLog_Debug(g_module4, QStringLiteral("This is a TestiiTest."));
    // Create the corresponding module with auto-generated filename, default destination folder with a custom log message display
    l_manager->addDestination(QString(), g_module4, LogLevel::Debug, QString(), LogMode::Full
                              , LogFileDisplay::Number, LogMessageDisplay::DateTime|LogMessageDisplay::Message);
    QLog_Debug(g_module4, QStringLiteral("This is a TestiiTest two.."));
}

// --- Custom features 2 ---

void logCustom2()
{
    auto *l_manager = QLoggerManager::getInstance();

    // Create destination with a given file name, default settings but
    // no notificaton about the created module and mode is only file even if default global mode is full
    l_manager->addDestination(g_file1, g_module5, LogLevel::Debug, QString()
                              , LogMode::OnlyFile, LogFileDisplay::Default
                              , LogMessageDisplay::Default, false);
    QLog_Debug(g_module5, QStringLiteral("This is a debug log message 0-0."));

    // Create destination with a given file name, default3 settings but no notificaton about the created module
    l_manager->addDestination(g_file1, g_module6, LogLevel::Debug, QString()
                              , LogMode::Full, LogFileDisplay::Default
                              , LogMessageDisplay::Default3, false);
    QLog_Debug(g_module6, QStringLiteral("This is a debug log message 0-1."));
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << "--- QLoggerTest ---";
    qInfo() << "# Welcome";

    auto *l_manager = QLoggerManager::getInstance();
    // Setup mode and level for testing
    l_manager->setDefaultMode(LogMode::Full);
    // l_manager->setDefaultLevel(LogLevel::Debug);

    logDefault();

    logCustom();

    logCustom2();


    QLog_Info(g_module1, QStringLiteral("\n---- Close Logger ----\n"));
    QLog_Info(g_module2, QStringLiteral("\n---- Close Logger ----\n"));
    QLog_Info(g_module3, QStringLiteral("\n---- Close Logger ----\n"));
    QLog_Info(g_module4, QStringLiteral("\n---- Close Logger ----\n"));
    QLog_Info(g_module5, QStringLiteral("\n---- Close Logger ----\n"));
    QLog_Info(g_module6, QStringLiteral("\n---- Close Logger ----\n"));

    QLoggerManager::getInstance()->closeLogger();

    // --- End ---
    QTimer::singleShot(2500, &a, []() {
        qInfo() << "# Done.";
        QLoggerManager::getInstance()->deleteLogger();
        exit(0);
    });

    return a.exec();
}

