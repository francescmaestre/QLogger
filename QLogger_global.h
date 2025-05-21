/**
 * @file QLogger_global.h
 * @class QLogger_global
 * @author Benoît MOUFLIN
 * @date 2020-11-02
 *
 * @brief The QLogger_global header
 *
 * @module QLogger
 * @note This file contains the macro used to expose symbols from this library
 * to outside callers.
 *
 * Copyright (c) 2020 AphaseDev. All rights reserved.
 * https://github.com/AphaseDev
 */
#ifndef QLOGGER_GLOBAL_H
#define QLOGGER_GLOBAL_H

#include <QtCore/qglobal.h>

#if (!defined(QT_STATIC) && !defined(QLOGGER_STATIC))
#   if defined(QLOGGER_LIBRARY)
#      define QLOGGERSHARED_EXPORT Q_DECL_EXPORT
#   else
#      define QLOGGERSHARED_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define QLOGGERSHARED_EXPORT
#endif

#endif // QLOGGER_GLOBAL_H
