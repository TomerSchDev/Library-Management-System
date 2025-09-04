#include "windows/familyviewdialog.h"

#include <qabstractitemmodel.h>

#include "windows/clientDetailDialog.h"
#include "../ui/ui_familyviewdialog.h"

FamilyViewDialog::FamilyViewDialog(QWidget *parent) :
    AbstractWindow(parent),
    ui(new Ui::FamilyViewDialog)
{
    ui->setupUi(this);
}

void FamilyViewDialog::handleEvent(const EventType event)
{
    if (event == EventType::ClientsUpdated || event == EventType::FamiliesUpdated) {
        // Refresh the family info if needed
        // This requires storing the current family name and clients
        // For simplicity, we will just clear the list here
        ui->clientsListWidget->clear();
        m_clients.clear();
        const QString familyName = ui->familyNameLabel->text().remove("Family: ").trimmed();
        const QList<Client> clients = Library::instance()->getClientsByFamilyName(familyName);
        setFamilyInfo(familyName,clients);
    }
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

        // Assuming clientName is the full string representation of the client
        // You'll need to find the actual Client object from the library
        for (const auto& client : m_clients) {
            if (client.toString() == clientName) {
                ClientDetailDialog dialog(client,Library::instance(),this);
                dialog.exec();
                return;
            }
        }
    }
}
