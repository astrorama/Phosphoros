/*
 * filecopyer.cpp
 *
 *  Created on: Oct 29, 2019
 *      Author: fdubath
 */



/*
* This is a minimal example to show thread-based QFile copy operation with progress notfication.
* See here for QFile limitations: https://doc.qt.io/qt-5/qfile.html#platform-specific-issues
* Copyright (C) 2019 Iman Ahmadvand
*
* This is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* It is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*/

#include <QtCore/qdebug.h>
#include "PhzQtUI/filecopyer.h"
#include "ElementsKernel/Logging.h"

namespace Euclid {
namespace PhzQtUI {
static Elements::Logging logger = Elements::Logging::getLogger("fileCopier");

FileCopyer::FileCopyer(QThread* _thread) {

    moveToThread(_thread);
    setChunkSize(DEFAULT_CHUNK_SIZE);

    QObject::connect(_thread, &QThread::started, this, &FileCopyer::copy);
    QObject::connect(this, &FileCopyer::finished, _thread, &QThread::quit);
    QObject::connect(this, &FileCopyer::finished, this, &FileCopyer::deleteLater);
    QObject::connect(_thread, &QThread::finished, _thread, &QThread::deleteLater);
    logger.info() <<"Copier created";
}


void FileCopyer::copy() {
    if (src.length()==0 || dst.length()==0) {
      logger.info() << "source or destination paths are empty!";
        emit finished(false,{""});
        return;
    }

    qint64 total = 0, written = 0;
    total += QFileInfo(src).size();
    logger.info() << total <<" bytes should be write in total";

    logger.info() << "writing with chunk size of " << chunkSize() << " byte";
    QFile srcFile(src);
    QFile dstFile(dst);
    if (QFile::exists(dst)) {
      logger.info() << "file "<< dst.toStdString() << " already exists, overwriting...";
        QFile::remove(dst);
    }

    if (!srcFile.open(QFileDevice::ReadOnly)) {
      logger.warn() << "failed to open " << src.toStdString() << " (error:" << srcFile.errorString().toStdString() << ")";
    } else if (!dstFile.open(QFileDevice::WriteOnly)) {
      logger.warn() << "failed to open " << dst.toStdString() << " (error:" << dstFile.errorString().toStdString() << ")";

    }

    /* copy the content in portion of chunk size */
    qint64 fSize = srcFile.size();
    while (fSize) {
        const auto data = srcFile.read(chunkSize());
        const auto _written = dstFile.write(data);
        if (data.size() == _written) {
            written += _written;
            fSize -= data.size();
            emit copyProgress(written, total);
        } else {
          logger.warn()  << "failed to write to  " << dst.toStdString() << " (error:" << dstFile.errorString().toStdString() << ")";
            fSize = 0;
            break; // skip this operation
        }
    }

    srcFile.close();
    dstFile.close();


    if (total == written) {
      logger.info()  << "progress finished, " << written << " bytes of " << total << " has been written";
        emit finished(true, dst);
    } else {
        emit finished(false, {""});
    }
}

}
}
