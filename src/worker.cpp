//
// Created by Volkov Sergey on 07/03/2026.
//

#include "worker.hpp"

#include "QFile"
#include "QDir"

QString FileXorWorker::resolveOutputFilename(const QDir& outputDir, const QString& filename)
{
    const QString basename = QFileInfo(filename).completeBaseName();
    const QString extension = QFileInfo(filename).suffix();

    QString variant;
    int cnt = 1;
    do
    {
        QString incremented = QString("%1_%2").arg(basename).arg(cnt) + "." + extension;
        variant = outputDir.filePath(incremented);
        cnt++;
    } while (QFile::exists(variant));

    return variant;
}

void FileXorWorker::process()
{
    QDir inputDir(m_jobConfig.inputPath);
    QDir outputDir(m_jobConfig.outputPath);
    QStringList files = inputDir.entryList({m_jobConfig.fileMasks}, QDir::Files);

    int current = 0;
    for (const QString& filename : files)
    {
        QString fullPath = inputDir.filePath(filename);
        QString outputPath = outputDir.filePath(filename);

        if (!m_jobConfig.overwriteExisting && QFile::exists(outputPath))
        {
            outputPath = resolveOutputFilename(outputDir, filename);
        }

        if (!processFile(fullPath, outputPath))
        {
            emit error("Failed to process file " + fullPath);
        }

        if (m_jobConfig.removeInputFiles) QFile::remove(fullPath);
        emit fileCompleted(filename);
        emit progressUpdated(++current, files.size());
        //todo: emit fileFinished for progressbar
    }
    emit finished();
}

bool FileXorWorker::processFile(const QString& inputFilePath, const QString& outputFilePath)
{
    QFile inputFile(inputFilePath);
    if (!inputFile.open(QIODevice::ReadOnly))
    {
        emit error("Failed to open input file " + inputFilePath);
        return false;
    }

    //possible issue with giant files unless reading in chunks?
    QByteArray data = inputFile.readAll();
    inputFile.close();

    //respects endianness!
    const quint8* xorValue = reinterpret_cast<const quint8*>(&m_jobConfig.xorValue);
    for (qsizetype i = 0; i < data.size(); ++i)
    {
        data[i] ^= xorValue[i%8];
    }

    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        emit error("Failed to open output file " + outputFilePath);
        return false;
    }

    outputFile.write(data);
    return true;
}
