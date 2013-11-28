#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStringList>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dialog = new Dialog;
    removeRuleDialog = new RemoveRuleDialog;

    // retrieve rules
    {
        _storage = LocalStorage::instance();
        QStringList ruleList;

        foreach (IRule *rule, _storage->rules())
        {
            ruleList << QString("%1.  %2 --> %3").arg(rule->id()).arg(rule->from(), rule->to());
        }

        ui->ruleListView->setModel(new QStringListModel(ruleList));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_newRuleButton_clicked()
{
    dialog->show();
}

void MainWindow::on_removeRuleButton_clicked()
{
    removeRuleDialog->show();
    ui->ruleListView->show();
}
