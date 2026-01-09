#include "mainwindow.h"

#include <QGraphicsRectItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QPainter>

namespace {
constexpr int kBoardSize = 8;
constexpr int kSquareSize = 64;
constexpr int kPieceSize = 48;
constexpr int kPieceInset = (kSquareSize - kPieceSize) / 2;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setFrameStyle(QFrame::NoFrame);

    const int boardPx = kBoardSize * kSquareSize;
    scene->setSceneRect(0, 0, boardPx, boardPx);
    view->setFixedSize(boardPx, boardPx);

    setCentralWidget(view);
    setWindowTitle("ChessVibe");

    setupBoard();
    setupPieces();
}

void MainWindow::setupBoard()
{
    const QColor light(240, 217, 181);
    const QColor dark(181, 136, 99);

    for (int rank = 0; rank < kBoardSize; ++rank) {
        for (int file = 0; file < kBoardSize; ++file) {
            const bool isDark = (rank + file) % 2 == 1;
            auto *square = scene->addRect(
                file * kSquareSize,
                rank * kSquareSize,
                kSquareSize,
                kSquareSize,
                QPen(Qt::NoPen),
                QBrush(isDark ? dark : light)
            );
            square->setZValue(0);
        }
    }
}

void MainWindow::addPiece(const QString &label, int file, int rank, bool isWhite)
{
    const QColor textColor = isWhite ? QColor(30, 30, 30) : QColor(230, 230, 230);

    const int x = file * kSquareSize;
    const int y = rank * kSquareSize;

    auto *text = new QGraphicsSimpleTextItem(label);
    QFont font("Segoe UI Symbol", 28);
    text->setFont(font);
    text->setBrush(textColor);
    text->setZValue(1);
    text->setFlag(QGraphicsItem::ItemIsMovable, true);
    text->setFlag(QGraphicsItem::ItemIsSelectable, true);

    const QRectF textBounds = text->boundingRect();
    const qreal centerX = x + (kSquareSize - textBounds.width()) / 2.0;
    const qreal centerY = y + (kSquareSize - textBounds.height()) / 2.0;
    text->setPos(centerX, centerY);
    scene->addItem(text);
}

void MainWindow::setupPieces()
{
    const QStringList blackBackRank = {"♜", "♞", "♝", "♛", "♚", "♝", "♞", "♜"};
    const QStringList whiteBackRank = {"♖", "♘", "♗", "♕", "♔", "♗", "♘", "♖"};
    const QString blackPawn = "♟";
    const QString whitePawn = "♙";

    for (int file = 0; file < kBoardSize; ++file) {
        addPiece(blackBackRank[file], file, 0, false);
        addPiece(blackPawn, file, 1, false);
        addPiece(whitePawn, file, 6, true);
        addPiece(whiteBackRank[file], file, 7, true);
    }
}
