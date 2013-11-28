#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <AccountManager.hpp>

#include "Dialog.hpp"
#include "RemoveRuleDialog.hpp"

#include <QMainWindow>

using namespace Gateway;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_newRuleButton_clicked();

    void on_removeRuleButton_clicked();

private:
    Ui::MainWindow *ui;
    Dialog *dialog;
    RemoveRuleDialog *removeRuleDialog;

    LocalStorage *_storage;

};

#endif // MAINWINDOW_H
