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
    const QColor fill = isWhite ? QColor(245, 245, 245) : QColor(40, 40, 40);
    const QColor outline = isWhite ? QColor(120, 120, 120) : QColor(10, 10, 10);
    const QColor textColor = isWhite ? QColor(30, 30, 30) : QColor(230, 230, 230);

    const int x = file * kSquareSize + kPieceInset;
    const int y = rank * kSquareSize + kPieceInset;

    auto *piece = scene->addEllipse(
        x,
        y,
        kPieceSize,
        kPieceSize,
        QPen(outline, 2),
        QBrush(fill)
    );
    piece->setFlag(QGraphicsItem::ItemIsMovable, true);
    piece->setFlag(QGraphicsItem::ItemIsSelectable, true);
    piece->setZValue(1);

    auto *text = new QGraphicsSimpleTextItem(label, piece);
    QFont font("Times New Roman", 18, QFont::Bold);
    text->setFont(font);
    text->setBrush(textColor);

    const QRectF textBounds = text->boundingRect();
    const qreal centerX = x + (kPieceSize - textBounds.width()) / 2.0;
    const qreal centerY = y + (kPieceSize - textBounds.height()) / 2.0;
    text->setPos(centerX - x, centerY - y);
}

void MainWindow::setupPieces()
{
    const QStringList backRank = {"R", "N", "B", "Q", "K", "B", "N", "R"};

    for (int file = 0; file < kBoardSize; ++file) {
        addPiece(backRank[file], file, 0, false);
        addPiece("P", file, 1, false);
        addPiece("P", file, 6, true);
        addPiece(backRank[file], file, 7, true);
    }
}
