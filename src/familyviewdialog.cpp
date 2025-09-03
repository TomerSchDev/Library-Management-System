#include "familyviewdialog.h"

#include <qabstractitemmodel.h>

#include "clientviewdialog.h"
#include "../ui/ui_familyviewdialog.h"

FamilyViewDialog::FamilyViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FamilyViewDialog)
{
    ui->setupUi(this);
}

FamilyViewDialog::~FamilyViewDialog()
{
    delete ui;
}

void FamilyViewDialog::setFamilyInfo(const QString &familyName, const QList<Client> &clients)
{
    ui->familyNameLabel->setText("Family: " + familyName);
    ui->clientCountLabel->setText("Number of Clients: " + QString::number(clients.size()));
    ui->clientsListWidget->clear();
    for (const Client& client : clients) {
        ui->clientsListWidget->addItem(client.toString());
        m_clients.append(client);
    }
}
void FamilyViewDialog::on_clientsListWidget_doubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString clientName = index.data(Qt::DisplayRole).toString();
        ClientViewDialog dialog(this);
        // Assuming clientName is the full string representation of the client
        // You'll need to find the actual Client object from the library
        for (const auto& client : m_clients) {
            if (client.toString() == clientName) {
                dialog.setClient(client);
                dialog.exec();
                return;
            }
        }
    }
}
