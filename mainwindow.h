#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>

class PieceItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupBoard();
    void addPieces();
    void addPiece(PieceItem *item);

    QGraphicsScene *scene = nullptr;
    QGraphicsView *view = nullptr;
    QLabel *turnLabel = nullptr;
};

#endif // MAINWINDOW_H
