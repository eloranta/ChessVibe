#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupBoard();
    void setupPieces();
    void addPiece(const QString &label, int file, int rank, bool isWhite);

    QGraphicsScene *scene = nullptr;
    QGraphicsView *view = nullptr;
};

#endif // MAINWINDOW_H
