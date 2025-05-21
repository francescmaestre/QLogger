#pragma once

/****************************************************************************************
 ** QLogger is a library to register and print logs into a file.
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

#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#   include <QFlags>
#endif

namespace QLogger
{

/**
 * @brief The LogLevel enum class defines the level of the log message.
 */
enum class LogLevel
{
   Default = -1,
   Trace = 0,
   Debug,
   Info,
   Warning,
   Error,
   Fatal
};

/**
 * @brief The LogMode enum class defines the way to display the log message.
 */
enum class LogMode
{
   Default = -1,
   Disabled = 0,
   OnlyConsole,
   OnlyFile,
   Full
};

/**
 * @brief The LogFileDisplay enum class defines which suffix is set to the log file name
 * in case the file is full.
 */
enum class LogFileDisplay
{
   Default = -1,
   DateTime,
   Number
};

/**
 * @brief The LogTextDisplay enum class defines which elements are written per log message.
 */
enum class LogMessageDisplay : unsigned int
{
   LogLevel = 1 << 0,
   ModuleName = 1 << 1,
   DateTime = 1 << 2,
   ThreadId = 1 << 3,
   Function = 1 << 4,
   File = 1 << 5,
   Line = 1 << 6,
   Message = 1 << 7,

   Default = LogLevel | ModuleName | DateTime | ThreadId | File | Line | Message,
   Default2 = LogLevel | ModuleName | DateTime | ThreadId | File | Function | Message,
   Default3 = LogLevel | ModuleName | DateTime | ThreadId | Function | Message,
   Full = 0xFF
};

Q_DECLARE_FLAGS(LogMessageDisplays, LogMessageDisplay)
Q_DECLARE_OPERATORS_FOR_FLAGS(LogMessageDisplays)

/**
 * @brief The LogMessageDisplayOrder enum class defines the element order written per log message.
 * in case the file is full.
 */
enum class LogMessageDisplayOrder
{
   Default = -1,
   LevelFirst, ///< The order is: [Level][Module][Date][ThreadId]{FileLine}{Function} message (Date as nb of secs)
   DateTimeFirst ///< The order is: Date [Level][ThreadId][Module]{FileLine}{Function} message (Date as string)
};

}
