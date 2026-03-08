//
// Created by Volkov Sergey on 07/03/2026.
//

#include "mainwindow.hpp"
#include "utils.hpp"
#include "jobconfig.hpp"
#include "worker.hpp"
#include "ui_MainWindow.h"

#include <QThread>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//slots

void MainWindow::onJobFinished()
{

    if (m_repeatTimer)
    {
        //timer is singleshot to control timing more precisely
        //and we launch it after every job finish
        m_repeatTimer->start();
    } else
    {
        ui->runButton->setEnabled(true);
    }
}

void MainWindow::onError(const QString& error)
{
    QMessageBox::critical(this, "Error", error);
}

//AutoMOC slots
void MainWindow::on_runButton_clicked()
{
    fillJobConfig();
    startJob();
}

void MainWindow::on_cancelButton_clicked()
{
    if (m_repeatTimer)
    {
        m_repeatTimer->stop();
        m_repeatTimer->deleteLater();
        m_repeatTimer = nullptr;

        ui->repeatTimerCheckbox->setEnabled(false);
        ui->repeatTimerSpinBox->setEnabled(false);
        ui->cancelButton->setEnabled(false);
        ui->runButton->setEnabled(true);
    }
    ui->statusbar->setEnabled(false);
}

void MainWindow::on_inputPathBrowseButton_clicked()
{
    try {
        QString dir = QFileDialog::getExistingDirectory(
            this, "Choose input directory", QApplication::applicationDirPath());
        if (!dir.isEmpty()) {
            ui->inputPathLineEdit->setText(dir);
        }
    } catch (const std::exception &e) {
        onError(e.what());
    }
}

void MainWindow::on_outputPathBrowseButton_clicked()
{
    try {
        QString dir = QFileDialog::getExistingDirectory(
            this, "Choose output directory", QApplication::applicationDirPath());
        if (!dir.isEmpty()) {
            ui->outputPathLineEdit->setText(dir);
        }
    } catch (const std::exception &e) {
        onError(e.what());
    }
}

void MainWindow::on_repeatTimerCheckbox_checkStateChanged()
{
    ui->repeatTimerSpinBox->setEnabled(ui->repeatTimerCheckbox->checkState());
}


//private functions

void MainWindow::startJob()
{
    if (m_currentJobConfig.inputPath.isEmpty() || m_currentJobConfig.outputPath.isEmpty())
    {
        onError("Can't start job without input path or output path");
        return;
    }

    m_workerThread = new QThread(this);
    FileXorWorker* worker = new FileXorWorker(m_currentJobConfig);
    worker->moveToThread(m_workerThread);

    connect(worker, &FileXorWorker::finished, this, &MainWindow::onJobFinished);
    connect(worker, &FileXorWorker::finished, worker, &QObject::deleteLater);
    connect(worker, &FileXorWorker::finished, m_workerThread, &QThread::quit);
    connect(worker, &FileXorWorker::error, this, &MainWindow::onError);
    connect(m_workerThread, &QThread::started, worker, &FileXorWorker::process);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);

    //todo: add progress bar
    //todo: use statusbar for progress
    ui->statusbar->setEnabled(true);
    ui->runButton->setEnabled(false);
    if (ui->repeatTimerCheckbox->isChecked() && ui->repeatTimerCheckbox->isEnabled())
    {
        //timer is created on first job and then reused until cancel is pressed
        m_repeatTimer = new QTimer(this);
        m_repeatTimer->setSingleShot(true);
        m_repeatTimer->setInterval(ui->repeatTimerSpinBox->value()*1000);
        connect(m_repeatTimer, &QTimer::timeout, this, [this]()
        {
            startJob();
        });

        ui->repeatTimerCheckbox->setEnabled(false);
        ui->repeatTimerSpinBox->setEnabled(false);
    }
    m_workerThread->start();
}

bool MainWindow::isValidInputFields() const
{
    //so far only checking if there are any values in necessary line edits
    return !(ui->inputPathLineEdit->text().isEmpty())
        || !(ui->outputPathLineEdit->text().isEmpty())
        || !(ui->xorValueLineEdit->text().isEmpty());
}

void MainWindow::fillJobConfig()
{
    if (!isValidInputFields())
    {
        onError("Invalid input");
        //todo: highlight empty fields
        return;
    }

    auto xorValue = utils::parseXorValue(ui->xorValueLineEdit->text());
    if (!xorValue)
    {
        onError("Failed to parse XOR value");
        return;
    }
    auto fileMasksValue = utils::parseFilenameMask(ui->fileMaskLineEdit->text());

    m_currentJobConfig = {
        .inputPath = ui->inputPathLineEdit->text(),
        .outputPath = ui->outputPathLineEdit->text(),
        .fileMasks = fileMasksValue,
        .xorValue = xorValue.value(),
        .removeInputFiles = ui->removeFilesCheckBox->isChecked(),
        .overwriteExisting = ui->overwriteFilesCheckBox->isChecked(),
    };
}
