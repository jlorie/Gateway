#include "Dialog.hpp"
#include "ui_Dialog.h"

#include <storage/LocalStorage.hpp>

using Gateway::LocalStorage;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_buttonBox_accepted()
{
    LocalStorage *storage = LocalStorage::instance();
    storage->newRule(ui->fromLineEdit->text(),ui->toLineEdit->text());
}

void Dialog::on_buttonBox_rejected()
{
    close();
}
