//
// Created by Volkov Sergey on 07/03/2026.
//

#pragma once

#include <QMainWindow>
#include <QTimer>

#include "jobconfig.hpp"


QT_BEGIN_NAMESPACE

namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE

//UI has:
// inputPathLineEdit + inputPathBrowseButton
// outputPathLineEdit + outputPathBrowseButton
// fileMaskLineEdit
// xorValueLineEdit
//
// removeFilesCheckBox
// overwriteFilesCheckBox
// repeatTimerCheckBox + repeatTimerSpinBox
//
// runButton + cancelButton
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;


private slots:
    void onJobFinished();
    void onError(const QString& error);

    //AutoMOC slots
    void on_runButton_clicked();
    void on_cancelButton_clicked();
    void on_inputPathBrowseButton_clicked();
    void on_outputPathBrowseButton_clicked();
    void on_repeatTimerCheckbox_checkStateChanged();

private:
    void startJob();
    bool isValidInputFields() const;
    void fillJobConfig();


    Ui::MainWindow* ui;
    QThread* m_workerThread = nullptr;
    QTimer* m_repeatTimer = nullptr;

    JobConfig m_currentJobConfig;
};
