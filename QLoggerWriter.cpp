#include "QLoggerWriter.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

namespace QLogger
{

QLoggerWriter::QLoggerWriter(const QString &fileDestination, LogLevel level, const QString &fileFolderDestination,
                             LogMode mode, LogFileDisplay fileSuffixIfFull, LogMessageDisplays messageOptions)
   : mFileSuffixIfFull(fileSuffixIfFull)
   , mMode(mode)
   , mLevel(level)
   , mMessageOptions(messageOptions)
{
   mFileDestinationFolder = fileFolderDestination.isEmpty() ? QDir::currentPath() + QStringLiteral("/logs/") : fileFolderDestination;

   if (!mFileDestinationFolder.endsWith(QLatin1Char('/')))
      mFileDestinationFolder.append(QLatin1Char('/'));

   mFileDestination = mFileDestinationFolder + fileDestination;

   QDir dir(mFileDestinationFolder);
   if (fileDestination.isEmpty()) 
   {
      // Generate a filename according to the date
      mFileDestination = dir.filePath(QString::fromLatin1("%1.log").arg(
          QDateTime::currentDateTime().date().toString(QString::fromLatin1("yyyy-MM-dd"))));
   } else if (!fileDestination.contains(QLatin1Char('.'))) {
      // Add default file extension
      mFileDestination.append(QString::fromLatin1(".log"));
   }

   if (mMode == LogMode::Full || mMode == LogMode::OnlyFile)
   {
      dir.mkpath(QStringLiteral("."));
   }
}

void QLoggerWriter::setLogMode(LogMode mode)
{
   mMode = mode;

   if (mMode == LogMode::Full || mMode == LogMode::OnlyFile) 
   {
      QDir dir(mFileDestinationFolder);
      dir.mkpath(QStringLiteral("."));
   }
}

/**
 * @brief Converts the given level in a QString.
 * @param level The log level in LogLevel format.
 * @return The string with the name of the log level.
 */
QString QLoggerWriter::levelToText(const QLogger::LogLevel &level)
{
   switch (level)
   {
      case QLogger::LogLevel::Trace:
         return QStringLiteral("Trace");
      case QLogger::LogLevel::Debug:
         return QStringLiteral("Debug");
      case QLogger::LogLevel::Info:
         return QStringLiteral("Info");
      case QLogger::LogLevel::Warning:
         return QStringLiteral("Warning");
      case QLogger::LogLevel::Error:
         return QStringLiteral("Error");
      case QLogger::LogLevel::Fatal:
         return QStringLiteral("Fatal");
      case QLogger::LogLevel::Default:
         return QStringLiteral("Default");
   }

   return QString();
}

QString QLoggerWriter::renameFileIfFull()
{
   QFile file(mFileDestination);

   // Rename file if it's full
   if (file.size() >= mMaxFileSize)
   {
      QString newName;

      const auto lidx = mFileDestination.lastIndexOf(QLatin1Char('.'));
      const auto fileDestination = mFileDestination.left(lidx);
      const auto fileExtension = mFileDestination.mid(lidx+1);

      if (mFileSuffixIfFull == LogFileDisplay::DateTime) {
         // Suffix is the date
         newName = QStringLiteral("%1_%2.%3")
                   .arg(fileDestination, 
                        QDateTime::currentDateTime().toString(QStringLiteral("dd_MM_yy__hh_mm_ss")),
                        fileExtension);
      } else {
         // Suffix is a number
         newName = generateDuplicateFilename(fileDestination, fileExtension);
      }

      const auto renamed = file.rename(mFileDestination, newName);

      return renamed ? newName : QString();
   }

   return QString();
}

QString QLoggerWriter::generateDuplicateFilename(const QString &fileDestination, const QString& fileExtension,
                                                 int fileSuffixNumber)
{
   QString path(fileDestination);
   if (fileSuffixNumber > 1)
   {
      // Set the new display name
      path = QStringLiteral("%1(%2).%3")
                      .arg(fileDestination, 
                           QString::number(fileSuffixNumber), 
                           fileExtension);
   } else {
      path.append(QLatin1Char('.'));
      path.append(fileExtension);
   }

   // A name already exists, increment the number and check again
   if (QFileInfo::exists(path)) {
       // A name already exists, increment the number and check again
       return generateDuplicateFilename(fileDestination, fileExtension, fileSuffixNumber + 1);
   }

   // No file exists at the given location, so no need to continue
   return path;
}


void QLoggerWriter::_write(QString message)
{
   // Write data to console
   if (mMode == LogMode::OnlyConsole)
   {
      qInfo().noquote() << message;
      return;
   }

   // Write data to file
   QFile file(mFileDestination);

   const auto prevFilename = renameFileIfFull();

   if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   {
      QTextStream out(&file);

      if (!prevFilename.isEmpty()) {
        out << QStringLiteral("Previous log %1\n").arg(prevFilename);
      }

      out << message << " \n";
      
      if (mMode == LogMode::Full) {
          qInfo().noquote() << message;
      }
      file.close();
   }
}

void QLoggerWriter::write(const QDateTime &date, const QString &threadId, const QString &module, LogLevel level,
                          const QString &function, const QString &fileName, int line, const QString &message)
{
   if (mMode == LogMode::Disabled)
      return;

   QString fileLine;
   if (mMessageOptions.testFlag(LogMessageDisplay::File) && mMessageOptions.testFlag(LogMessageDisplay::Line)
       && !fileName.isEmpty() && line > 0 && mLevel <= LogLevel::Debug)
   {
      fileLine = QStringLiteral("{%1:%2}").arg(fileName, QString::number(line));
   }
   else if (mMessageOptions.testFlag(LogMessageDisplay::File)
            && !fileName.isEmpty() && mLevel <= LogLevel::Debug)
   {
      fileLine = QStringLiteral("{%1}").arg(fileName);
   }

   QString text;
   if (mMessageOptions.testFlag(LogMessageDisplay::Default))
   {
      // TODO Add option for defining log message info order
      text = QStringLiteral("%1 [%2][%3][%4]%5 %6")
                  .arg(date.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz")), 
                       levelToText(level), threadId, module, fileLine, message);
   }
   // TODO Implement cases Default2, Default3
   else
   {
      if (mMessageOptions.testFlag(LogMessageDisplay::DateTime)) {
         text.append(QString::fromLatin1("%1 ").arg(date.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss.zzz"))));
      }
      if (mMessageOptions.testFlag(LogMessageDisplay::LogLevel)) {
         text.append(QString::fromLatin1("[%1]").arg(levelToText(level)));
      }
      if (mMessageOptions.testFlag(LogMessageDisplay::ThreadId)) {
         text.append(QString::fromLatin1("[%1]").arg(threadId));
      }
      if (mMessageOptions.testFlag(LogMessageDisplay::ModuleName)) {
         text.append(QString::fromLatin1("[%1]").arg(module));
      }
      if (mMessageOptions.testFlag(LogMessageDisplay::Function) && !function.isEmpty()) {
         text.append(QString::fromLatin1("{%1}").arg(function));
      }
      if (!fileLine.isEmpty())
      {
         if (fileLine.startsWith(QChar::Space)) {
            fileLine = fileLine.right(1);
         }
         text.append(fileLine);
      }
      if (mMessageOptions.testFlag(LogMessageDisplay::Message))
      {
         if (text.isEmpty() || text.endsWith(QChar::Space)) {
            text.append(QString::fromLatin1("%1").arg(message));
         } else {
            text.append(QString::fromLatin1(" %1").arg(message));
         }
      }
   }

   if (!mIsStop) {
       _write(std::move(text));
   }
}

}
