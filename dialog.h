#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_openBoot_clicked();
    void on_openApp_clicked();
    void on_hexMerge_clicked();
    void on_hexMergeAs_clicked();

private:
    Ui::Dialog *ui;

    QString bootPath;
    QString appPath;
};

#endif // DIALOG_H
