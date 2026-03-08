//
// Created by Volkov Sergey on 07/03/2026.
//

#pragma once

#include <QObject>
#include <QDir>
#include <QString>

#include "jobconfig.hpp"

class FileXorWorker : public QObject {
    Q_OBJECT
    //possible other option: make a separate class for handling files
    //that would call the worker function using threadpool on files
public:
    explicit FileXorWorker(JobConfig config, QObject *parent = nullptr)
    : QObject(parent), m_jobConfig(std::move(config)) {}

public slots:
    void process();

    signals:
    void finished();
    void error(const QString &message);

private:
    bool processFile(const QString& inputFilePath, const QString& outputFilePath);
    static QString resolveOutputFilename(const QDir& outputDir, const QString& filename);
    JobConfig m_jobConfig;

};

