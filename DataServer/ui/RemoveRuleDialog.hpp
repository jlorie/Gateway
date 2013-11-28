#ifndef REMOVERULEDIALOG_HPP
#define REMOVERULEDIALOG_HPP

#include <QDialog>

namespace Ui {
class RemoveRuleDialog;
}

class RemoveRuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveRuleDialog(QWidget *parent = 0);
    ~RemoveRuleDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::RemoveRuleDialog *ui;
};

#endif // REMOVERULEDIALOG_HPP
