#ifndef EDITCLIENTDIALOG_H
#define EDITCLIENTDIALOG_H

#include <QDialog>
#include <QString>
#include <QList>

#include "abstractWindow.h"
#include "../client.h"

namespace Ui {
    class EditClientDialog;
}

class EditClientDialog : public AbstractWindow
{
    Q_OBJECT

public:
    explicit EditClientDialog(const Client& client, const QList<QString>& existingFamilies, QWidget *parent = nullptr);
    ~EditClientDialog() override;
    void handleEvent(EventType event) override;
    QString getName() const;
    QString getSurname() const;
    QString getFamily() const;

private:
    Ui::EditClientDialog *ui;
};

#endif // EDITCLIENTDIALOG_H