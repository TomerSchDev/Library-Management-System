#ifndef CLIENTDETAILDIALOG_H
#define CLIENTDETAILDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include "client.h"
#include "library.h"

namespace Ui {
    class ClientDetailDialog;
}


class ClientDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientDetailDialog(const Client& client, Library* library, QWidget *parent = nullptr);
    ~ClientDetailDialog() override;

private slots:
    void on_newBorrowButton_clicked();
    void on_editInfoButton_clicked();
    void on_saveCloseButton_clicked();
    void on_returnBookButton_clicked();
    void on_extendBorrowButton_clicked();
    void on_infoTable_cellDoubleClicked(int row, int column);

private:
    Ui::ClientDetailDialog *ui;
    Client m_client;
    QVector<BorrowRecord> m_borrowRecords;
    Library* m_library = Library::instance();
    void setupUI();
    void loadBorrowRecords();
    void updateBorrowTable();
};

#endif // CLIENTDETAILDIALOG_H
