#ifndef BOOKDETAILDIALOG_H
#define BOOKDETAILDIALOG_H

#include <QDialog>
#include "book.h"
#include "library.h"

class QTableWidgetItem;

namespace Ui {
    class BookDetailDialog;
}

class BookDetailDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookDetailDialog(const Book& book, Library* library, QWidget *parent = nullptr);
    ~BookDetailDialog() override;

private:
    void setupUI();
    void loadBorrowRecords();
    void updateTables();
    void onBooksUpdated();

private:
    Ui::BookDetailDialog *ui;
    Book m_book;
    Library* m_library;
    QList<BorrowRecord> m_borrowRecords;
private slots:
    void onTableDoubleClicked(int row, int column);
    void onTableItemDoubleClicked(QTableWidgetItem *item);
    void onloadBorrowRecords();
};

#endif // BOOKDETAILDIALOG_H
