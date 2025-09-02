#ifndef FAMILYVIEWDIALOG_H
#define FAMILYVIEWDIALOG_H

#include <QDialog>
#include <QList>
#include "client.h"

namespace Ui {
    class FamilyViewDialog;
}

class FamilyViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FamilyViewDialog(QWidget *parent = nullptr);
    ~FamilyViewDialog();

    void setFamilyInfo(const QString& familyName, const QList<Client>& clients);

private:
    Ui::FamilyViewDialog *ui;
};

#endif // FAMILYVIEWDIALOG_H
