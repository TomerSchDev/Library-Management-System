#include "familyviewdialog.h"
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
    }
}
