#pragma once

/****************************************************************************************
 ** QLogger is a library to register and print logs into a file.
 ** Copyright (C) 2022 Francesc Martinez
 **
 ** LinkedIn: www.linkedin.com/in/cescmm/
 ** Web: www.francescmm.com
 **
 ** This library is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This library is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

#include <QLoggerLevel.h>
#include <QLogger_global.h>

#include <QMutex>
#include <QMap>
#include <QVariant>

namespace QLogger
{

class QLoggerWriter;

/**
 * @brief The QLoggerManager class manages the different destination files that we would like to have.
 */
class QLOGGERSHARED_EXPORT QLoggerManager : public QObject
{
   Q_OBJECT

public:
   /**
    * @brief Gets an instance to the QLoggerManager.
    * @return A pointer to the instance.
    */
   static QLoggerManager* getInstance();
   /**
    * @brief Gets an instance to the QLoggerManager's thread.
    * @return A pointer to the thread object.
    */
   static QThread* getInstanceThread();
   /**
    * @brief instanceIsAlive
    * @return True if INSTANCE is not null.
    */
   static bool instanceIsAlive();
   /**
    * @brief This method creates a QLoogerWriter that stores the name of the file and the log
    * level assigned to it. Here is added to the map the different modules assigned to each
    * log file. The method returns <em>false</em> if a module is configured to be stored in
    * more than one file.
    *
    * @param fileDest The file name and path to print logs.
    * @param module The module that will be stored in the file.
    * @param level The maximum level allowed.
    * @param fileFolderDestination The complete folder destination.
    * @param mode The logging mode.
    * @param fileSuffixIfFull The filename suffix if the file is full.
    * @param messageOptions Specifies what elements are displayed in one line of log message.
    * @param messageOrder Specifies the element order displayed in one line of log message.
    * @return Returns true if any error have been done.
    */
   bool addDestination(const QString& fileDest, const QString& module, LogLevel level = LogLevel::Default,
                       const QString& fileFolderDestination = QString(), LogMode mode = LogMode::Default,
                       LogFileDisplay fileSuffixIfFull = LogFileDisplay::Default,
                       LogMessageDisplays messageOptions = LogMessageDisplays(),
                       LogMessageDisplayOrder messageOrder = LogMessageDisplayOrder::Default);
   /**
    * @brief This method creates a QLoogerWriter that stores the name of the file and the log
    * level assigned to it. Here is added to the map the different modules assigned to each
    * log file. The method returns <em>false</em> if a module is configured to be stored in
    * more than one file.
    *
    * @param fileDest The file name and path to print logs.
    * @param modules The modules that will be stored in the file.
    * @param level The maximum level allowed.
    * @param fileFolderDestination The complete folder destination.
    * @param mode The logging mode.
    * @param fileSuffixIfFull The filename suffix if the file is full.
    * @param messageOptions Specifies what elements are displayed in one line of log message.
    * @param messageOrder Specifies the element order displayed in one line of log message.
    * @return Returns true if any error have been done.
    */
   bool addDestination(const QString& fileDest, const QStringList& modules, LogLevel level = LogLevel::Default,
                       const QString& fileFolderDestination = QString(), LogMode mode = LogMode::Default,
                       LogFileDisplay fileSuffixIfFull = LogFileDisplay::Default,
                       LogMessageDisplays messageOptions = LogMessageDisplays(),
                       LogMessageDisplayOrder messageOrder = LogMessageDisplayOrder::Default);
   /**
    * @brief Clears old log files from the current storage folder.
    *
    * @param fileFolderDestination The destination folder.
    * @param days Minimum age of log files to delete. Logs older than
    *        this value will be removed. If days is -1, deletes any log file.
    * @param fileExtension To specifies a particular type of file to remove. Empty string: remove all files.
    */
   static void clearFileDestinationFolder(const QString& fileFolderDestination, int days = -1,
                                          const QString& fileExtension=QString());
   /**
    * @brief Default QLogger settings for a command line app without log file.
    * @param level The default log level.
    * @param debugModeOnly Enable in QT_DEBUG mode only.
    */
   static void initializeLoggerConsole(LogLevel level = LogLevel::Info, bool debugModeOnly = false);
   /**
    * @brief Gets the QLoggerWriter's mode corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @return Returns the log mode of the module, Default (-1) is returned if not found.
    */
   LogMode getModuleMode(const QString& module);
   /**
    * @brief Sets the QLoggerWriter's mode corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @param mode The mode to apply to the module.
    */
   void setModuleLogMode(const QString& module, LogMode mode);
   /**
    * @brief Gets the QLoggerWriter's level corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @return Returns the log level of the module, Default (-1) is returned if not found.
    */
   LogLevel getModuleLevel(const QString& module);
   /**
    * @brief Sets the QLoggerWriter's level corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @param level The level to apply to the module.
    */
   void setModuleLogLevel(const QString& module, LogLevel level);
   /**
    * @brief Gets the QLoggerWriter's messageOptions corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @return Returns the log messageOptions of the module, Default (-1) is returned if not found.
    */
   LogMessageDisplays getModuleMessageOptions(const QString& module);
   /**
    * @brief Sets the QLoggerWriter's messageOptions corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @param messageOptions The messageOptions to apply to the module.
    */
   void setModuleMessageOptions(const QString& module, LogMessageDisplays messageOptions);
   /**
    * @brief Gets the QLoggerWriter's file destination corresponding to the module <em>module</em>.
    * @param module The module we look for.
    * @return Returns the file destination of the module or empty if not found.
    */
   QString getModuleFileDestination(const QString& module);

   /**
    * @brief enqueueMessage Enqueues a message in the corresponding QLoggerWritter.
    * @param module The module that writes the message.
    * @param level The level of the message.
    * @param message The message to log.
    * @param function The function in the file where the log comes from.
    * @param file The file that logs.
    * @param line The line in the file where the log comes from.
    */
   void enqueueMessage(const QString& module, LogLevel level, const QString& message, const QString& function,
                       const QString& file, int line);

   /**
    * @brief Whether the QLogger is paused or not.
    */
   bool isPaused() const { return mIsStop; }

   /**
    * @brief pause Pauses all QLoggerWriters.
    */
   void pause();

   /**
    * @brief resume Resumes all QLoggerWriters that where paused.
    */
   void resume();

   /**
    * @brief This method closes the logger and the thread it represents.
    * @note Can be blocking.
    */
   void closeLogger();

   /**
    * @brief This function deletes the instance to the QLoggerManager.
    */
   static void deleteLogger();

   /**
    * @brief getDefaultFileDestinationFolder Gets the default file destination folder.
    * @return The file destination folder
    */
   QString getDefaultFileDestinationFolder() const { return mDefaultFileDestinationFolder; }

   /**
    * @brief getDefaultMode Gets the default log mode.
    * @return The log mode
    */
   LogMode getDefaultMode() const { return mDefaultMode; }

   /**
    * @brief getDefaultLevel Gets the default log level.
    * @return The log level
    */
   LogLevel getDefaultLevel() const { return mDefaultLevel; }

   /**
    * @brief Sets default values for QLoggerWritter parameters. Useful for multiple QLoggerWritter.
    */
   void setDefaultFileDestinationFolder(const QString& fileDestinationFolder);
   void setDefaultFileDestination(const QString& fileDestination) { mDefaultFileDestination = fileDestination; }
   void setDefaultFileSuffixIfFull(LogFileDisplay fileSuffixIfFull) { mDefaultFileSuffixIfFull = fileSuffixIfFull; }

   void setDefaultLevel(LogLevel level) { mDefaultLevel = level; }
   void setDefaultMode(LogMode mode) { mDefaultMode = mode; }
   void setDefaultMaxFileSize(int maxFileSize) { mDefaultMaxFileSize = maxFileSize; }
   void setDefaultMessageOptions(LogMessageDisplays messageOptions) { mDefaultMessageOptions = messageOptions; }
   void setDefaultMessageOptionsOrder(LogMessageDisplayOrder messageOrder)
   {
      mDefaultMessageOptionsOrder = messageOrder;
   }

   /**
    * @brief overwriteLogMode Overwrites the logging mode in all the destinations. Sets the default logging mode.
    *
    * @param mode The new log mode
    */
   void overwriteLogMode(LogMode mode);
   /**
    * @brief overwriteLogLevel Overwrites the log level in all the destinations. Sets the default log level.
    *
    * @param level The new log level
    */
   void overwriteLogLevel(LogLevel level);
   /**
    * @brief overwriteMaxFileSize Overwrites the maximum file size in all the destinations. Sets the default max file
    * size.
    *
    * @param maxSize The new file size
    */
   void overwriteMaxFileSize(int maxSize);

   /**
    * @brief moveLogsWhenClose Moves all the logs to a new folder. This will happen only on close.
    * @param newLogsFolder The new folder that will store the logs.
    */
   void moveLogsWhenClose(const QString& newLogsFolder) { mNewLogsFolder = newLogsFolder; }

   /**
    * @brief Enable writing a log message when a new destination is added or not.
    * @param logNewDestination Enable or not
    */
   void enableLogNewDestination(bool logNewDestination) { mLogNewDestination = logNewDestination; }

private:
   /**
    * @brief Invoked on QLogger's thread when closing logger after posted messages have been processed.
    * @return oldFiles The list of log file paths
    */
   Q_INVOKABLE QVector<QString> waitPostedMessageProcessedFinished();
   /**
    * @brief Checks if the logger is stop
    */
   bool mIsStop = false;

   /**
    * @brief Map that stores the module and the file it is assigned.
    */
   QMap<QString, QLoggerWriter*> mModuleDest;

   /**
    * @brief Defines the queue of messages when no writers have been set yet.
    */
   QMultiMap<QString, QVector<QVariant>> mNonWriterQueue;

   /**
    * @brief Default values for QLoggerWritter parameters. Useful for multiple QLoggerWritter.
    */
   QString mDefaultFileDestinationFolder;
   QString mDefaultFileDestination;
   LogFileDisplay mDefaultFileSuffixIfFull = LogFileDisplay::DateTime;

   LogMode mDefaultMode = LogMode::OnlyFile;
   LogLevel mDefaultLevel = LogLevel::Warning;
   int mDefaultMaxFileSize = 1024 * 1024; //! @note 1Mio
   LogMessageDisplays mDefaultMessageOptions = LogMessageDisplay::Default;
   LogMessageDisplayOrder mDefaultMessageOptionsOrder = LogMessageDisplayOrder::LevelFirst;
   /**
    * @brief Write a log message when a new destination is added.
    */
   bool mLogNewDestination = true;
   /**
    * @brief If set, this is the folder that will store the logs on close.
    */
   QString mNewLogsFolder;

   /**
    * @brief Mutex to make the method thread-safe.
    */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
   QMutex mMutex { QMutex::Recursive };
#else
   QRecursiveMutex mMutex;
#endif

   /**
    * @brief Default builder of the class. It starts the thread.
    */
   QLoggerManager();
   /**
    * @brief Destructor
    */
   ~QLoggerManager();

   /**
    * @brief Initializes and returns a new instance of QLoggerWriter with the given parameters.
    * @param fileDest The file name and path to print logs.
    * @param level The maximum level allowed.
    * @param fileFolderDestination The complete folder destination.
    * @param mode The logging mode.
    * @param fileSuffixIfFull The filename suffix if the file is full.
    * @param messageOptions Specifies what elements are displayed in one line of log message.
    * @param messageOrder Specifies the element order displayed in one line of log message.
    * @return the newly created QLoggerWriter instance.
    */
   QLoggerWriter* createWriter(const QString& fileDest, LogLevel level, const QString& fileFolderDestination,
                               LogMode mode, LogFileDisplay fileSuffixIfFull, LogMessageDisplays messageOptions,
                               LogMessageDisplayOrder messageOrder) const;

   void startWriter(const QString& module, QLoggerWriter* log, LogMode mode);

   /**
    * @brief Checks the queue and writes the messages if the writer is the correct one. The queue is emptied
    * for that module.
    * @param module The module to dequeue the messages from
    */
   void writeAndDequeueMessages(const QString& module);

signals:
   void _startEnqueueMessage(const QString& module, QLogger::LogLevel level, const QString& message,
                             const QString& function, const QString& file, int line, const QString& threadId);

private slots:
   /**
    * @brief enqueueMessage Enqueues a message in the corresponding QLoggerWritter. This method is run on
    * the QLogger's thread.
    *
    * @param module The module that writes the message.
    * @param level The level of the message.
    * @param message The message to log.
    * @param function The function in the file where the log comes from.
    * @param file The file that logs.
    * @param line The line in the file where the log comes from.
    * @param threadId The thread ID.
    */
   void _enqueueMessage(const QString& module, QLogger::LogLevel level, const QString& message, const QString& function,
                        const QString& file, int line, const QString& threadId);
};

/**
 * @brief Here is done the call to write the message in the module. First of all is confirmed
 * that the log level we want to write is less or equal to the level defined when we create the
 * destination.
 *
 * @param module The module that the message references.
 * @param level The level of the message.
 * @param message The message.
 * @param function The function in the file where the log comes from.
 * @param file The file that logs.
 * @param line The line in the file where the log comes from.
 */
extern void QLog_(const QString& module, QLogger::LogLevel level, const QString& message, const QString& function,
                  const QString& file = QString(), int line = -1);

}

// Macros for writing log messages

#ifndef QLog_Trace
/**
 * @brief Used to store Trace level messages.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define QLog_Trace(module, message)                                                                                 \
      QLogger::QLoggerManager::getInstance()->enqueueMessage(module, QLogger::LogLevel::Trace, message,                \
                                                             QString::fromLatin1(__FUNCTION__),                        \
                                                             QString::fromLatin1(__FILE__), __LINE__)
#endif

#ifndef QLog_Debug
/**
 * @brief Used to store Debug level messages.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define QLog_Debug(module, message)                                                                                 \
      QLogger::QLoggerManager::getInstance()->enqueueMessage(module, QLogger::LogLevel::Debug, message,                \
                                                             QString::fromLatin1(__FUNCTION__),                        \
                                                             QString::fromLatin1(__FILE__), __LINE__)
#endif

#ifndef QLog_Info
/**
 * @brief Used to store Info level messages.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define QLog_Info(module, message)                                                                                  \
      QLogger::QLoggerManager::getInstance()->enqueueMessage(module, QLogger::LogLevel::Info, message,                 \
                                                             QString::fromLatin1(__FUNCTION__),                        \
                                                             QString::fromLatin1(__FILE__), __LINE__)
#endif

#ifndef QLog_Warning
/**
 * @brief Used to store Warning level messages.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define QLog_Warning(module, message)                                                                               \
      QLogger::QLoggerManager::getInstance()->enqueueMessage(module, QLogger::LogLevel::Warning, message,              \
                                                             QString::fromLatin1(__FUNCTION__),                        \
                                                             QString::fromLatin1(__FILE__), __LINE__)
#endif

#ifndef QLog_Error
/**
 * @brief Used to store Error level messages.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define QLog_Error(module, message)                                                                                 \
      QLogger::QLoggerManager::getInstance()->enqueueMessage(module, QLogger::LogLevel::Error, message,                \
                                                             QString::fromLatin1(__FUNCTION__),                        \
                                                             QString::fromLatin1(__FILE__), __LINE__)
#endif

#ifndef QLog_Fatal
/**
 * @brief Used to store Fatal level messages.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define QLog_Fatal(module, message)                                                                                 \
      QLogger::QLoggerManager::getInstance()->enqueueMessage(module, QLogger::LogLevel::Fatal, message,                \
                                                             QString::fromLatin1(__FUNCTION__),                        \
                                                             QString::fromLatin1(__FILE__), __LINE__)
#endif

// Macros for adding destinations

#ifndef QLog_AddDest
/**
 * @brief Used to add a new destination with the default destination folder.
 * @param module The module that the message references.
 * @param level The log level of the module.
 */
#   define QLog_AddDest(module, level) QLogger::QLoggerManager::getInstance()->addDestination(QString(), module, level)
/**
 * @brief Used to add a new destination with the default destination folder in a QML plugin.
 * @param module The module that the message references.
 * @param level The log level of the module.
 */
#   define QLog_AddDestPlugin(module, level)                                                                           \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLogger::QLoggerManager::getInstance()->addDestination(QString(), module, level);                             \
      }
#endif

#ifndef QLog_AddDestObj
/**
 * @brief Used to add a new destination with the default destination folder.
 * @param level The log level of the module.
 */
#   define QLog_AddDestObj(level)                                                                                      \
      QLogger::QLoggerManager::getInstance()->addDestination(QString(),                                                \
                                                             QString::fromLatin1(metaObject()->className()), level)

#   define QLog_AddDest_qObj(baseClass, level)                                                                         \
      QLogger::QLoggerManager::getInstance()->addDestination(                                                          \
          QString(), QString::fromLatin1(baseClass::staticMetaObject.className()), level)
/**
 * @brief Used to add a new destination with the default destination folder in a QML plugin.
 * @param level The log level of the module.
 */
#   define QLog_AddDestObjPlugin(level)                                                                                \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLogger::QLoggerManager::getInstance()->addDestination(                                                       \
             QString(), QString::fromLatin1(metaObject()->className()), level);                                        \
      }

#   define QLog_AddDest_qObjPlugin(baseClass, level)                                                                   \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLogger::QLoggerManager::getInstance()->addDestination(                                                       \
             QString(), QString::fromLatin1(baseClass::staticMetaObject.className()), level);                          \
      }
#endif

#ifndef QLog_AddDestDflt
/**
 * @brief Used to add a new destination with the default destination folder and level.
 */
#   define QLog_AddDestDflt()                                                                                          \
      QLogger::QLoggerManager::getInstance()->addDestination(QString(), QString::fromLatin1(metaObject()->className()))

#   define QLog_AddDest_qDflt(baseClass)                                                                               \
      QLogger::QLoggerManager::getInstance()->addDestination(                                                          \
          QString(), QString::fromLatin1(baseClass::staticMetaObject.className()))

/**
 * @brief Used to add a new destination with the default destination folder and level in a QML plugin.
 */
#   define QLog_AddDestDfltPlugin()                                                                                    \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLogger::QLoggerManager::getInstance()->addDestination(QString(),                                             \
                                                                QString::fromLatin1(metaObject()->className()));       \
      }

#   define QLog_AddDest_qDfltPlugin(baseClass)                                                                         \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLogger::QLoggerManager::getInstance()->addDestination(                                                       \
             QString(), QString::fromLatin1(baseClass::staticMetaObject.className()));                                 \
      }
#endif

// Macros for writing log messages with QObject's class name as module name

#ifndef L_LOGGING_MACROS_QOBJECT
#   define L_LOGGING_MACROS_QOBJECT
/**
 * @brief Used to store messages to the module referenced by the QObject class name or the base class name.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define L_TRACE_Obj(message) QLog_Trace(QString::fromLatin1(metaObject()->className()), message)
#   define L_DEBUG_Obj(message) QLog_Debug(QString::fromLatin1(metaObject()->className()), message)
#   define L_INFO_Obj(message) QLog_Info(QString::fromLatin1(metaObject()->className()), message)
#   define L_WARN_Obj(message) QLog_Warning(QString::fromLatin1(metaObject()->className()), message)
#   define L_ERROR_Obj(message) QLog_Error(QString::fromLatin1(metaObject()->className()), message)
#   define L_FATAL_Obj(message) QLog_Fatal(QString::fromLatin1(metaObject()->className()), message)

#   define L_TRACE_qObj(baseClass, message)                                                                            \
      QLog_Trace(QString::fromLatin1(baseClass::staticMetaObject.className()), message)
#   define L_DEBUG_qObj(baseClass, message)                                                                            \
      QLog_Debug(QString::fromLatin1(baseClass::staticMetaObject.className()), message)
#   define L_INFO_qObj(baseClass, message)                                                                             \
      QLog_Info(QString::fromLatin1(baseClass::staticMetaObject.className()), message)
#   define L_WARN_qObj(baseClass, message)                                                                             \
      QLog_Warning(QString::fromLatin1(baseClass::staticMetaObject.className()), message)
#   define L_ERROR_qObj(baseClass, message)                                                                            \
      QLog_Error(QString::fromLatin1(baseClass::staticMetaObject.className()), message)
#   define L_FATAL_qObj(baseClass, message)                                                                            \
      QLog_Fatal(QString::fromLatin1(baseClass::staticMetaObject.className()), message)
/**
 * @brief Used to store messages to the module referenced by the class name or the base class name
 * in a QML plugin.
 * @param module The module that the message references.
 * @param message The message.
 */
#   define L_TRACE_ObjP(message)                                                                                       \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Trace(QString::fromLatin1(metaObject()->className()), message);                                          \
      }
#   define L_DEBUG_ObjP(message)                                                                                       \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Debug(QString::fromLatin1(metaObject()->className()), message);                                          \
      }
#   define L_INFO_ObjP(message)                                                                                        \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Info(QString::fromLatin1(metaObject()->className()), message);                                           \
      }
#   define L_WARN_ObjP(message)                                                                                        \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Warning(QString::fromLatin1(metaObject()->className()), message);                                        \
      }
#   define L_ERROR_ObjP(message)                                                                                       \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Error(QString::fromLatin1(metaObject()->className()), message);                                          \
      }
#   define L_FATAL_ObjP(message)                                                                                       \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Fatal(QString::fromLatin1(metaObject()->className()), message);                                          \
      }

#   define L_TRACE_qObjP(baseClass, message)                                                                           \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Trace(QString::fromLatin1(baseClass::staticMetaObject.className()), message);                            \
      }
#   define L_DEBUG_qObjP(baseClass, message)                                                                           \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Debug(QString::fromLatin1(baseClass::staticMetaObject.className()), message);                            \
      }
#   define L_INFO_qObjP(baseClass, message)                                                                            \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Info(QString::fromLatin1(baseClass::staticMetaObject.className()), message);                             \
      }
#   define L_WARN_qObjP(baseClass, message)                                                                            \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Warning(QString::fromLatin1(baseClass::staticMetaObject.className()), message);                          \
      }
#   define L_ERROR_qObjP(baseClass, message)                                                                           \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Error(QString::fromLatin1(baseClass::staticMetaObject.className()), message);                            \
      }
#   define L_FATAL_qObjP(baseClass, message)                                                                           \
      if (QLogger::QLoggerManager::instanceIsAlive())                                                                  \
      {                                                                                                                \
         QLog_Fatal(QString::fromLatin1(baseClass::staticMetaObject.className()), message);                            \
      }

// Macros for writing log messages with QObject's class name as module name according to the debug/release mode

/**
 * @brief Used to log debug message according to the debug or release mode of the application (e.g.: to log an object
 * destruction only in debug mode).
 */
#   ifndef L_DEBUG_Log
#      ifdef QT_DEBUG
#         define L_DEBUG_Log(message) L_INFO_Obj(QStringLiteral(message))
#      else
#         define L_DEBUG_Log(message) L_DEBUG_Obj(QStringLiteral(message))
#      endif

#      ifdef QT_DEBUG
#         define L_DEBUG_LogP(message) L_INFO_ObjP(QStringLiteral(message))
#      else
#         define L_DEBUG_LogP(message) L_DEBUG_ObjP(QStringLiteral(message))
#      endif
#   endif

#   ifndef L_DEBUG_qLog
#      ifdef QT_DEBUG
#         define L_DEBUG_qLog(baseClass, message) L_INFO_qObj(baseClass, QStringLiteral(message))
#      else
#         define L_DEBUG_qLog(baseClass, message) L_DEBUG_qObj(baseClass, QStringLiteral(message))
#      endif

#      ifdef QT_DEBUG
#         define L_DEBUG_qLogP(baseClass, message) L_INFO_qObjP(baseClass, QStringLiteral(message))
#      else
#         define L_DEBUG_qLogP(baseClass, message) L_DEBUG_qObjP(baseClass, QStringLiteral(message))
#      endif
#   endif

#endif // L_LOGGING_MACROS_QOBJECT
