#ifndef ADDCLIENTDIALOG_H
#define ADDCLIENTDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>

#include "abstractWindow.h"

namespace Ui {
    class AddClientDialog;
}

class AddClientDialog final : public AbstractWindow
{
    Q_OBJECT

public:
    explicit AddClientDialog(QWidget *parent = nullptr);
    explicit AddClientDialog(const QList<QString>& existingFamilies, QWidget *parent = nullptr);
    ~AddClientDialog() override;

    QString getName() const;
    QString getSurname() const;
    QString getFamily() const;
    void handleEvent(EventType event) override;

private:
    Ui::AddClientDialog *ui;
};

#endif // ADDCLIENTDIALOG_H
