/*
 * filecopyer.h
 *
 *  Created on: Oct 29, 2019
 *      Author: fdubath
 */

#ifndef PHZQTUI_SRC_LIB_FILECOPYER_H_
#define PHZQTUI_SRC_LIB_FILECOPYER_H_

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


#include <QtCore/qstring.h>
#include <QtCore/qobject.h>
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qvector.h>
#include <QtCore/qthread.h>
namespace Euclid {
namespace PhzQtUI {

class FileCopyer : public QObject {
    Q_OBJECT
        Q_PROPERTY(qint64 chunksize READ chunkSize WRITE setChunkSize)
        Q_PROPERTY(QString sourcePaths READ sourcePaths WRITE setSourcePaths)
        Q_PROPERTY(QString destinationPaths READ destinationPaths WRITE setDestinationPaths)

public:
    static const int DEFAULT_CHUNK_SIZE = 1024 * 1024 * 1;

    explicit FileCopyer(QThread*);
    virtual ~FileCopyer() {}

    qint64 chunkSize() const {
        return _chunk;
    }
    void setChunkSize(qint64 ch) {
        _chunk = ch;
    }

    QString sourcePaths() const {
        return src;
    }
    void setSourcePaths(const QString& _src) {
        src = _src;
    }

    QString destinationPaths() const {
        return dst;
    }
    void setDestinationPaths(const QString& _dst) {
        dst = _dst;
    }

    protected slots:
    void copy();


signals:
    void copyProgress(qint64 bytesCopied, qint64 bytesTotal);
    void finished(bool success, QString path);


    private:
        QString src, dst;
        qint64 _chunk;
};


}
}
#endif /* PHZQTUI_SRC_LIB_FILECOPYER_H_ */
