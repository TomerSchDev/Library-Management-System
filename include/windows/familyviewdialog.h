#ifndef FAMILYVIEWDIALOG_H
#define FAMILYVIEWDIALOG_H

#include <QDialog>
#include <QList>

#include "abstractWindow.h"
#include "../client.h"

namespace Ui {
    class FamilyViewDialog;
}

class FamilyViewDialog : public AbstractWindow
{
    Q_OBJECT

public:
    explicit FamilyViewDialog(QWidget *parent = nullptr);
    void handleEvent(EventType event) override;
    ~FamilyViewDialog() override;

    void setFamilyInfo(const QString& familyName, const QList<Client>& clients);
private slots:

    void on_clientsListWidget_doubleClicked(const QModelIndex& index);

private:
    Ui::FamilyViewDialog *ui{};
    QList<Client> m_clients;
};

#endif // FAMILYVIEWDIALOG_H
