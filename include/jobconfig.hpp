//
// Created by Volkov Sergey on 07/03/2026.
//

#pragma once

#include <QString>

struct JobConfig
{
    QString inputPath;
    QString outputPath;
    QStringList fileMasks;
    quint64 xorValue = 0;
    bool removeInputFiles = false;
    bool overwriteExisting = true;
};