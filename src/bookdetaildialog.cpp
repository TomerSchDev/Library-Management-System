#include "bookdetaildialog.h"
#include "../ui/ui_bookdetaildialog.h"
#include <QDate>
#include <QDebug>
#include <QHeaderView>
#include <QTableWidgetItem>

#include "clientDetailDialog.h"

BookDetailDialog::BookDetailDialog(const Book& book, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BookDetailDialog)
    , m_book(book)
    , m_library(library)
{
    ui->setupUi(this);
    setupUI();
    loadBorrowRecords();
    updateTables();
    connect(ui->currentBorrowsTable, &QTableWidget::cellDoubleClicked, this, &BookDetailDialog::onTableDoubleClicked);
    // For item pointer version:
    connect(ui->historyTable, &QTableWidget::itemDoubleClicked, this, &BookDetailDialog::onTableItemDoubleClicked);

}

BookDetailDialog::~BookDetailDialog()
{
    delete ui;
}

void BookDetailDialog::setupUI()
{
    setWindowTitle(QString("Book Details - %1").arg(m_book.title()));
    resize(800, 600);

    // Set up info labels
    ui->titleLabel->setText(m_book.title());
    ui->authorLabel->setText(m_book.author());
    ui->totalCopiesLabel->setText(QString::number(m_book.copies()));
    ui->availableCopiesLabel->setText(QString::number(m_book.availableCopies()));

    // Setup tables
    // Current Borrows table
    ui->currentBorrowsTable->setColumnCount(4);
    ui->currentBorrowsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->currentBorrowsTable->verticalHeader()->setVisible(false);
    ui->currentBorrowsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->currentBorrowsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Borrow History table
    ui->historyTable->setColumnCount(5);
    ui->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->historyTable->verticalHeader()->setVisible(false);
    ui->historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void BookDetailDialog::loadBorrowRecords()
{
    m_borrowRecords.clear();
    m_borrowRecords = m_library->getBorrowRecordsByBookId(m_book.id());
}

void BookDetailDialog::updateTables()
{
    ui->currentBorrowsTable->clearContents();
    ui->currentBorrowsTable->setRowCount(0);
    ui->historyTable->clearContents();
    ui->historyTable->setRowCount(0);

    for (const auto& record : m_borrowRecords) {
        Client client = m_library->getClientById(record.clientId);
        QString clientName = QString("%1 %2").arg(client.name(), client.surname());

        if (record.isReturned) {
            // Populate history table
            int row = ui->historyTable->rowCount();
            ui->historyTable->insertRow(row);

            QDate actualReturnDate = record.returnDate; // Assuming returnDate is the actual return date when isReturned is true.
            int daysLate = 0;
            // You will need to store the expected return date to calculate days late
            // or modify the BorrowRecord struct to include it.
            // For now, we'll assume a fixed loan period or use a placeholder.
            // Let's assume a 14-day loan and calculate days late based on that.
            QDate expectedReturnDate = record.borrowDate.addDays(14);
            if (actualReturnDate > expectedReturnDate) {
                daysLate = expectedReturnDate.daysTo(actualReturnDate);
            }

            ui->historyTable->setItem(row, 0, new QTableWidgetItem(clientName));
            ui->historyTable->setItem(row, 1, new QTableWidgetItem(record.borrowDate.toString("dd/MM/yyyy")));
            ui->historyTable->setItem(row, 2, new QTableWidgetItem(expectedReturnDate.toString("dd/MM/yyyy")));
            ui->historyTable->setItem(row, 3, new QTableWidgetItem(actualReturnDate.toString("dd/MM/yyyy")));
            ui->historyTable->setItem(row, 4, new QTableWidgetItem(QString::number(daysLate)));
        } else {
            // Populate current borrows table
            int row = ui->currentBorrowsTable->rowCount();
            ui->currentBorrowsTable->insertRow(row);

            bool isLate = QDate::currentDate() > record.returnDate;
            QTableWidgetItem* isLateItem = new QTableWidgetItem(isLate ? "Yes" : "No");
            if (isLate) {
                isLateItem->setForeground(QBrush(QColor("red")));
            }

            ui->currentBorrowsTable->setItem(row, 0, new QTableWidgetItem(clientName));
            ui->currentBorrowsTable->setItem(row, 1, new QTableWidgetItem(record.borrowDate.toString("dd/MM/yyyy")));
            ui->currentBorrowsTable->setItem(row, 2, new QTableWidgetItem(record.returnDate.toString("dd/MM/yyyy")));
            ui->currentBorrowsTable->setItem(row, 3, isLateItem);
        }
    }
}

void BookDetailDialog::onBooksUpdated()
{
    updateTables();
}

void BookDetailDialog::onTableDoubleClicked(int row, int column)
{
    if (column != 0) return; // Only respond to client name cell
    if (row < 0 || row >= ui->currentBorrowsTable->rowCount()) return; // Invalid row
    QString clientName = ui->currentBorrowsTable->item(row, 0)->text();
    // Assuming clientName is the full string representation of the client
    // You'll need to find the actual Client object from the library
    for (const auto& record : m_borrowRecords)
    {
        Client client = m_library->getClientById(record.clientId);
        QString clientTmpName = QString("%1 %2").arg(client.name(), client.surname());
        if (clientTmpName == clientName)
        {
            ClientDetailDialog dialog(client, m_library, this);
            dialog.exec();
            return;
        }
    }
}

void BookDetailDialog::onTableItemDoubleClicked(QTableWidgetItem* item)
{
    int row = item->row();
    int column = item->column();
    if (column != 0) return; // Only respond to client name cell
    if (row < 0 || row >= ui->historyTable->rowCount()) return; // Invalid row
    const QString clientName = ui->historyTable->item(row, 0)->text();
    // Assuming clientName is the full string representation of the client
    // You'll need to find the actual Client object from the library
    for (const auto& record : m_borrowRecords)
    {
        Client client = m_library->getClientById(record.clientId);
        if (QString clientTmpName = QString("%1 %2").arg(client.name(), client.surname()); clientTmpName == clientName)
        {
            ClientDetailDialog dialog(client, m_library, this);
            dialog.exec();
            return;
        }
    }

}

void BookDetailDialog::onloadBorrowRecords()
{
    loadBorrowRecords();
    updateTables();
}
