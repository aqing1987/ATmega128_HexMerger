#include "dialog.h"
#include "ui_dialog.h"

#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // add minimize and max button for the dialog
    setWindowFlags(Qt::Dialog
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);

    ui->openApp->setEnabled(false);
    ui->hexMerge->setEnabled(false);
    ui->hexMergeAs->setEnabled(false);

    setWindowTitle("ATmega128 Hex Merger");
    bootPath = "";
    appPath = "";
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_openBoot_clicked()
{
    QDir dir;
    QString fileName;

    // if hex file has been added
    if (ui->lineEditBoot->text().endsWith(".hex")) {
        fileName = ui->lineEditBoot->text();
    }
    else {
        if (bootPath.isNull()) {
            fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Boot"),
                                                    dir.currentPath(),
                                                    tr("hex (*.hex)"));
        }
        else {
            fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Boot"),
                                                    bootPath,
                                                    tr("hex (*.hex)"));
        }

    }

    if (fileName.isNull()) {
        return;
    }
    else {
        ui->lineEditBoot->clear();
        bootPath = dir.filePath(fileName);
        ui->lineEditBoot->setText(bootPath);
    }

    // judge if the file is boot file
    QFile file(fileName);

    if (file.isOpen()) {
        file.close();
    }
    if ( !file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    if (line.isNull()) {
        ui->lineEditBoot->setText("file empty err.");
        return;
    }
    else {
        if ( !line.endsWith("EC")) {
            QMessageBox *msgBox = new
                    QMessageBox(QMessageBox::Information, "boot open",
                                "boot file is invalid!",
                                QMessageBox::Yes|QMessageBox::No);
            msgBox->exec();

            ui->lineEditBoot->setText("");
            ui->openApp->setEnabled(false);

        }
        else {
            ui->openApp->setEnabled(true);
        }
    }

    file.close();
}

void Dialog::on_openApp_clicked()
{
    QDir dir;
    QString fileName;

    // if hex file has been added
    if (ui->lineEditApp->text().endsWith(".hex")) {
        fileName = ui->lineEditApp->text();
    }
    else {
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open App"),
                                                bootPath,
                                                tr("hex (*.hex)"));

    }

    if (fileName.isNull()) {
        return;
    }
    else {
        ui->lineEditApp->clear();
        appPath = dir.filePath(fileName);
        ui->lineEditApp->setText(appPath);
    }

    // judge if the file is app file
    QFile file(fileName);

    if (file.isOpen()) {
        file.close();
    }
    if ( !file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    if (line.isNull()) {
        ui->lineEditApp->setText("file empty err.");
        return;
    }
    else {
        if ( !line.endsWith("FC")) {
            QMessageBox *msgBox = new
                    QMessageBox(QMessageBox::Information, "app open",
                                "App file is invalid!",
                                QMessageBox::Yes|QMessageBox::No);
            msgBox->exec();

            ui->lineEditApp->setText("");
            ui->hexMerge->setEnabled(false);
        }
        else {
            ui->hexMerge->setEnabled(true);
            ui->hexMergeAs->setEnabled(true);
        }
    }

    file.close();
}

void Dialog::on_hexMerge_clicked()
{
    QString fileName, saveName;
    QString tmpBootPath, tmpAppPath;

    // generate the final file name
    tmpBootPath = bootPath;
    tmpAppPath = appPath;
    saveName = tmpAppPath.left(tmpAppPath.length() - 4);
    saveName += "_with_";
    saveName += tmpBootPath.right(tmpBootPath.length() - tmpBootPath.lastIndexOf("/") - 1);

    fileName = saveName;

    QFile appFile(appPath);
    QFile bootFile(bootPath);
    QFile saveFile(fileName);

    // file judge
    if (appFile.isOpen()) {
        appFile.close();
    }
    if (bootFile.isOpen()) {
        bootFile.close();
    }
    if (saveFile.isOpen()) {
        saveFile.close();
    }


    // file open
    if (!appFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return;
    }
    if (!bootFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return;
    }
    if ( !saveFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return;
    }

    // generate stream
    QTextStream bootIn(&bootFile);
    QTextStream appIn(&appFile);
    QTextStream saveOut(&saveFile);

    // copy boot file first
    QString copyLine = bootIn.readLine();
    while ( (copyLine != ":00000001FF") && (!copyLine.isNull()))
    {
        saveOut << copyLine << endl;
        copyLine = bootIn.readLine();
    }
    bootIn.flush();
    bootFile.close();

    // then, copy app file
    copyLine = "";
    copyLine = appIn.readLine();
    while (!copyLine.isNull())
    {
        saveOut << copyLine << endl;
        copyLine = appIn.readLine();
    }
    appIn.flush();
    appFile.close();

    // close the final hex file
    saveOut.flush();
    saveFile.close();

    QMessageBox *msgBox = new
            QMessageBox(QMessageBox::Information, "S-Hex",
                        "Hex generated success!",
                        QMessageBox::Yes|QMessageBox::No);
    msgBox->exec();

    ui->lineEditBoot->setText("");
    ui->lineEditApp->setText("");

    ui->openApp->setEnabled(false);
    ui->hexMerge->setEnabled(false);
    ui->hexMergeAs->setEnabled(false);
}

void Dialog::on_hexMergeAs_clicked()
{
    QString fileName, saveName;
    QString tmpBootPath, tmpAppPath;

    // generate the final file name
    tmpBootPath = bootPath;
    tmpAppPath = appPath;
    saveName = tmpAppPath.left(tmpAppPath.length() - 4);
    saveName += "_with_";
    saveName += tmpBootPath.right(tmpBootPath.length() - tmpBootPath.lastIndexOf("/") - 1);

    // select save file path
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save hex"),
                                            saveName,
                                            tr("hex (*.hex)"));

    QFile appFile(appPath);
    QFile bootFile(bootPath);
    QFile saveFile(fileName);

    // file judge
    if (appFile.isOpen()) {
        appFile.close();
    }
    if (bootFile.isOpen()) {
        bootFile.close();
    }
    if (saveFile.isOpen()) {
        saveFile.close();
    }


    // file open
    if (!appFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return;
    }
    if (!bootFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return;
    }
    if ( !saveFile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        return;
    }

    // generate stream
    QTextStream bootIn(&bootFile);
    QTextStream appIn(&appFile);
    QTextStream saveOut(&saveFile);

    // copy boot file first
    QString copyLine = bootIn.readLine();
    while ( (copyLine != ":00000001FF") && (!copyLine.isNull()))
    {
        saveOut << copyLine << endl;
        copyLine = bootIn.readLine();
    }
    bootIn.flush();
    bootFile.close();

    // then, copy app file
    copyLine = "";
    copyLine = appIn.readLine();
    while (!copyLine.isNull())
    {
        saveOut << copyLine << endl;
        copyLine = appIn.readLine();
    }
    appIn.flush();
    appFile.close();

    // close the final hex file
    saveOut.flush();
    saveFile.close();

    QMessageBox *msgBox = new
            QMessageBox(QMessageBox::Information, "S-Hex",
                        "Hex generated success!",
                        QMessageBox::Yes|QMessageBox::No);
    msgBox->exec();

    ui->lineEditBoot->setText("");
    ui->lineEditApp->setText("");

    ui->openApp->setEnabled(false);
    ui->hexMerge->setEnabled(false);
    ui->hexMergeAs->setEnabled(false);
}
