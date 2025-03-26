#include "QLogger.h"

#include "QLoggerWriter.h"

#include <QDateTime>
#include <QDir>
#include <QThread>
#include <QEvent>
#include <QDebug>

Q_DECLARE_METATYPE(QLogger::LogLevel)
Q_DECLARE_METATYPE(QLogger::LogMode)
Q_DECLARE_METATYPE(QLogger::LogFileDisplay)
Q_DECLARE_METATYPE(QLogger::LogMessageDisplay)

namespace QLogger
{

void QLog_(const QString& module, LogLevel level, const QString& message, const QString& function, const QString& file,
           int line)
{
   QLoggerManager::getInstance()->enqueueMessage(module, level, message, function, file, line);
}

static const int QUEUE_LIMIT = 100;

// --- QLoggerManager ---

QLoggerManager::QLoggerManager()
   : QObject()
{
   this->setObjectName(QStringLiteral("QLoggerManager"));

   connect(this, &QLoggerManager::_startEnqueueMessage, this, &QLoggerManager::_enqueueMessage, Qt::QueuedConnection);
}

static QLoggerManager* INSTANCE = nullptr;

QLoggerManager* QLoggerManager::getInstance()
{
   if (!INSTANCE)
   {
      INSTANCE = new QLoggerManager();

      if (!INSTANCE->isPaused())
      {
         QThread* instanceThread = getInstanceThread();
         if (INSTANCE->thread() != instanceThread)
         {
            INSTANCE->moveToThread(instanceThread);
         }
         instanceThread->start();
      }
   }

   return INSTANCE;
}

QThread* QLoggerManager::getInstanceThread()
{
   static QThread INSTANCETHREAD;
   INSTANCETHREAD.setObjectName(QLatin1String("QLogger thread")); // Set the name of the thread

   return &INSTANCETHREAD;
}

bool QLoggerManager::instanceIsAlive()
{
   return INSTANCE != nullptr;
}

bool QLoggerManager::addDestination(const QString& fileDest, const QString& module, LogLevel level,
                                    const QString& fileFolderDestination, LogMode mode, LogFileDisplay fileSuffixIfFull,
                                    LogMessageDisplays messageOptions)
{
   QMutexLocker lock(&mMutex);

   if (!mModuleDest.contains(module))
   {
      const auto log = createWriter(fileDest, level, fileFolderDestination, mode, fileSuffixIfFull, messageOptions);
      log->moveToThread(getInstanceThread());

      mModuleDest.insert(module, log);

      startWriter(module, log, mode);

      return true;
   }

   return false;
}

bool QLoggerManager::addDestination(const QString& fileDest, const QStringList& modules, LogLevel level,
                                    const QString& fileFolderDestination, LogMode mode, LogFileDisplay fileSuffixIfFull,
                                    LogMessageDisplays messageOptions)
{
   QMutexLocker lock(&mMutex);
   bool allAdded = false;

   for (const auto& module : modules)
   {
      if (!mModuleDest.contains(module))
      {
         const auto log = createWriter(fileDest, level, fileFolderDestination, mode, fileSuffixIfFull, messageOptions);
         log->moveToThread(getInstanceThread());

         mModuleDest.insert(module, log);

         startWriter(module, log, mode);

         allAdded = true;
      }
   }

   return allAdded;
}

QLoggerWriter* QLoggerManager::createWriter(const QString& fileDest, LogLevel level,
                                            const QString& fileFolderDestination, LogMode mode,
                                            LogFileDisplay fileSuffixIfFull, LogMessageDisplays messageOptions) const
{
   const auto lFileDest = fileDest.isEmpty() ? mDefaultFileDestination : fileDest;
   const auto lLevel = level == LogLevel::Default ? mDefaultLevel : level;
   const auto lFileFolderDestination = fileFolderDestination.isEmpty()
       ? mDefaultFileDestinationFolder
       : QDir::fromNativeSeparators(fileFolderDestination);
   const auto lMode = mode == LogMode::Default ? mDefaultMode : mode;
   const auto lFileSuffixIfFull
       = fileSuffixIfFull == LogFileDisplay::Default ? mDefaultFileSuffixIfFull : fileSuffixIfFull;
   const auto lMessageOptions = messageOptions == LogMessageDisplays() ? mDefaultMessageOptions : messageOptions;

   const auto log
       = new QLoggerWriter(lFileDest, lLevel, lFileFolderDestination, lMode, lFileSuffixIfFull, lMessageOptions);

   log->setMaxFileSize(mDefaultMaxFileSize);
   log->stop(mIsStop);

   return log;
}

void QLoggerManager::startWriter(const QString& module, QLoggerWriter* log, LogMode mode)
{
   Q_UNUSED(log)
   Q_UNUSED(mode)
   if (mLogNewDestination)
   {
      this->enqueueMessage(module, LogLevel::Info, QStringLiteral("Adding destination!"), QString(), QString(), -1);
   }
}

void QLoggerManager::clearFileDestinationFolder(const QString& fileFolderDestination, int days)
{
   QDir dir(fileFolderDestination + QStringLiteral("/logs"));

   if (!dir.exists())
   {
      return;
   }

   dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

   const auto list = dir.entryInfoList();
   const auto now = QDateTime::currentDateTime();

   for (const auto& fileInfoIter : list)
   {
      if (fileInfoIter.lastModified().daysTo(now) >= days)
      {
         // remove file
         dir.remove(fileInfoIter.fileName());
      }
   }
}

/**
 * @public static
 */
void QLoggerManager::initializeLoggerConsole(LogLevel level, bool debugModeOnly)
{
   // Setup QLogger
   auto l_manager = QLoggerManager::getInstance();
   l_manager->setDefaultFileSuffixIfFull(LogFileDisplay::Number);
   l_manager->setDefaultMessageOptions(LogMessageDisplay::Default3); ///< Not display Function, File and Line
   // Default mode
   if (debugModeOnly)
   {
#ifdef QT_DEBUG
      l_manager->setDefaultMode(LogMode::OnlyConsole); ///< Messages are only displayed in console
#else
      l_manager->setDefaultMode(LogMode::Disabled);
#endif
   }
   else
   {
      l_manager->setDefaultMode(LogMode::OnlyConsole); ///< Messages are only displayed in console
   }
   // Default level
   l_manager->setDefaultLevel(level);
}

LogLevel QLoggerManager::getModuleLevel(const QString& module)
{
   QMutexLocker lock(&mMutex);
   auto* l_logWriter = mModuleDest.value(module, nullptr);
   if (l_logWriter)
   {
      return l_logWriter->getLevel();
   }
   return LogLevel::Default;
}

void QLoggerManager::setModuleLogLevel(const QString& module, LogLevel level)
{
   QMutexLocker lock(&mMutex);
   auto* l_logWriter = mModuleDest.value(module, nullptr);
   if (l_logWriter)
   {
      l_logWriter->setLogLevel(level);
   }
}

LogMessageDisplays QLoggerManager::getModuleMessageOptions(const QString& module)
{
   QMutexLocker lock(&mMutex);
   auto* l_logWriter = mModuleDest.value(module, nullptr);
   if (l_logWriter)
   {
      return l_logWriter->getMessageOptions();
   }
   return LogMessageDisplay::Default;
}

void QLoggerManager::setModuleMessageOptions(const QString& module, LogMessageDisplays messageOptions)
{
   QMutexLocker lock(&mMutex);
   auto* l_logWriter = mModuleDest.value(module, nullptr);
   if (l_logWriter)
   {
      l_logWriter->setMessageOptions(messageOptions);
   }
}

QString QLoggerManager::getModuleFileDestination(const QString& module)
{
   QMutexLocker lock(&mMutex);
   auto* l_logWriter = mModuleDest.value(module, nullptr);
   if (l_logWriter)
   {
      return l_logWriter->getFileDestination();
   }
   return QString();
}

void QLoggerManager::setDefaultFileDestinationFolder(const QString& fileDestinationFolder)
{
   mDefaultFileDestinationFolder = QDir::fromNativeSeparators(fileDestinationFolder);
}

void QLoggerManager::writeAndDequeueMessages(const QString& module)
{
   QMutexLocker lock(&mMutex);

   const auto logWriter = mModuleDest.value(module, nullptr);

   if (logWriter && !logWriter->isStop())
   {
      const auto values = mNonWriterQueue.values(module);

      for (const auto& vals : values)
      {
         const auto level = qvariant_cast<LogLevel>(vals.at(2).toInt());

         if (logWriter->getLevel() <= level)
         {
            const auto datetime = vals.at(0).toDateTime();
            const auto threadId = vals.at(1).toString();
            const auto function = vals.at(3).toString();
            const auto file = vals.at(4).toString();
            const auto line = vals.at(5).toInt();
            const auto message = vals.at(6).toString();

            logWriter->write(datetime, threadId, module, level, function, file, line, message);
         }
      }

      mNonWriterQueue.remove(module);
   }
}

void QLoggerManager::enqueueMessage(const QString& module, LogLevel level, const QString& message,
                                    const QString& function, const QString& file, int line)
{
   const auto threadId
       = QStringLiteral("%1").arg((quintptr)QThread::currentThread(), QT_POINTER_SIZE /** 2*/, // ignores first 00000
                                  16, QLatin1Char('0'));

   emit _startEnqueueMessage(module, level, message, function, file, line, threadId);
}

void QLoggerManager::_enqueueMessage(const QString& module, LogLevel level, const QString& message,
                                     const QString& function, const QString& file, int line, const QString& threadId)
{
   {
      QMutexLocker lock(&mMutex);
      const auto logWriter = mModuleDest.value(module, nullptr);
      const auto isLogEnabled = logWriter && logWriter->getMode() != LogMode::Disabled && !logWriter->isStop();

      if (logWriter && isLogEnabled && logWriter->getLevel() <= level)
      {
         const auto fileName = file.mid(file.lastIndexOf(QLatin1Char('/')) + 1);

         writeAndDequeueMessages(module);

         logWriter->write(QDateTime::currentDateTime(), threadId, module, level, function, fileName, line, message);
      }
      else if (!logWriter && mNonWriterQueue.count(module) < QUEUE_LIMIT)
      {
         const auto fileName = file.mid(file.lastIndexOf(QLatin1Char('/')) + 1);

         if (mDefaultMode != LogMode::OnlyFile)
         {
            qWarning().noquote().nospace() << "No module for message [" << module << "][" << message << "]";
         }
         mNonWriterQueue.insert(module,
                                { QDateTime::currentDateTime(), threadId, QVariant::fromValue<LogLevel>(level),
                                  function, fileName, line, message });
      }
   }
}

void QLoggerManager::pause()
{
   QMutexLocker lock(&mMutex);

   mIsStop = true;

   for (auto& logWriter : mModuleDest)
      logWriter->stop(mIsStop);
}

void QLoggerManager::resume()
{
   QMutexLocker lock(&mMutex);

   mIsStop = false;

   for (auto& logWriter : mModuleDest)
      logWriter->stop(mIsStop);
}

void QLoggerManager::overwriteLogMode(LogMode mode)
{
   QMutexLocker lock(&mMutex);

   setDefaultMode(mode);

   for (auto& logWriter : mModuleDest)
      logWriter->setLogMode(mode);
}

void QLoggerManager::overwriteLogLevel(LogLevel level)
{
   QMutexLocker lock(&mMutex);

   setDefaultLevel(level);

   for (auto& logWriter : mModuleDest)
      logWriter->setLogLevel(level);
}

void QLoggerManager::overwriteMaxFileSize(int maxSize)
{
   QMutexLocker lock(&mMutex);

   setDefaultMaxFileSize(maxSize);

   for (auto& logWriter : mModuleDest)
      logWriter->setMaxFileSize(maxSize);
}

QVector<QString> QLoggerManager::waitPostedMessageProcessedFinished()
{
   // qDebug() << "#" << QThread::currentThread();
   QVector<QString> oldFiles;
   {
      QMutexLocker locker(&mMutex);

      for (auto i = mModuleDest.cbegin(), end = mModuleDest.cend(); i != end; ++i)
         writeAndDequeueMessages(i.key());

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
      for (auto dest : qAsConst(mModuleDest))
#else
      for (auto dest : std::as_const(mModuleDest))
#endif
      {
         dest->stop(true);

         oldFiles.append(dest->getFileDestinationFolder());
      }

      qDeleteAll(mModuleDest);
      mModuleDest.clear();
   }

   return oldFiles;
}

void QLoggerManager::closeLogger()
{
   QVector<QString> oldFiles;

   // Invoke in QLogger's thread
   QMetaObject::invokeMethod(
       this, "waitPostedMessageProcessedFinished",
       Qt::BlockingQueuedConnection, // When the method gets called, previous pending message events have been processed
       Q_RETURN_ARG(QVector<QString>, oldFiles));

   this->pause();

   if (!mNewLogsFolder.isEmpty() && mNewLogsFolder != mDefaultFileDestinationFolder)
   {
      for (const auto& oldDestination : oldFiles)
      {
         QDir dir(oldDestination);

         auto entryList = dir.entryList(QDir::Files | QDir::System | QDir::Hidden);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
         for (const auto& filePath : qAsConst(entryList))
#else
         for (const auto& filePath : std::as_const(entryList))
#endif
         {
            auto destination = mNewLogsFolder;
            if (!destination.endsWith(QLatin1Char('/')))
            {
               destination.append(QLatin1Char('/'));
            }
            destination.append(filePath.split(QLatin1Char('/')).last());

            QDir().rename(oldDestination + QLatin1Char('/') + filePath, destination);

            if (dir.isEmpty())
            {
               dir.removeRecursively();
            }
         }
      }
   }

   getInstanceThread()->quit();
   getInstanceThread()->wait();
}

void QLoggerManager::deleteLogger()
{
   // qDebug("# QLoggerManager deleteLogger");
   delete INSTANCE;
   INSTANCE = nullptr;
}

QLoggerManager::~QLoggerManager()
{
#ifdef QT_DEBUG
   qDebug("# QLoggerManager destruction");
#endif
}

}
