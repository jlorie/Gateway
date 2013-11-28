#include "RemoveRuleDialog.hpp"
#include "ui_RemoveRuleDialog.h"

#include <storage/LocalStorage.hpp>

using Gateway::LocalStorage;

RemoveRuleDialog::RemoveRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RemoveRuleDialog)
{
    ui->setupUi(this);
}

RemoveRuleDialog::~RemoveRuleDialog()
{
    delete ui;
}

void RemoveRuleDialog::on_buttonBox_accepted()
{
    LocalStorage *storage = LocalStorage::instance();
    storage->removeRule(ui->idRuleLineEdit->text().toLongLong());
}
