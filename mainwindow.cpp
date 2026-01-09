#include "mainwindow.h"

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QPainter>

namespace {
constexpr int kBoardSize = 8;
constexpr int kSquareSize = 64;
}

class PieceItem : public QGraphicsSimpleTextItem
{
public:
    explicit PieceItem(const QString &label, QGraphicsItem *parent = nullptr)
        : QGraphicsSimpleTextItem(label, parent)
    {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setZValue(1);
    }

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        QGraphicsSimpleTextItem::mouseReleaseEvent(event);

        const QRectF bounds = boundingRect();
        const QPointF pos = this->pos();
        const QPointF center = pos + QPointF(bounds.width() / 2.0, bounds.height() / 2.0);

        int file = static_cast<int>(center.x() / kSquareSize);
        int rank = static_cast<int>(center.y() / kSquareSize);
        file = qBound(0, file, kBoardSize - 1);
        rank = qBound(0, rank, kBoardSize - 1);

        const qreal targetX = file * kSquareSize + (kSquareSize - bounds.width()) / 2.0;
        const qreal targetY = rank * kSquareSize + (kSquareSize - bounds.height()) / 2.0;
        setPos(targetX, targetY);
    }
};

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

    auto *text = new PieceItem(label);
    QFont font("Segoe UI Symbol", 28);
    text->setFont(font);
    text->setBrush(textColor);

    const QRectF textBounds = text->boundingRect();
    const qreal centerX = x + (kSquareSize - textBounds.width()) / 2.0;
    const qreal centerY = y + (kSquareSize - textBounds.height()) / 2.0;
    text->setPos(centerX, centerY);
    scene->addItem(text);
}

void MainWindow::setupPieces()
{
    const QStringList blackBackRank = {"\u265C", "\u265E", "\u265D", "\u265B", "\u265A", "\u265D", "\u265E", "\u265C"};
    const QStringList whiteBackRank = {"\u2656", "\u2658", "\u2657", "\u2655", "\u2654", "\u2657", "\u2658", "\u2656"};
    const QString blackPawn = "\u265F";
    const QString whitePawn = "\u2659";

    for (int file = 0; file < kBoardSize; ++file) {
        addPiece(blackBackRank[file], file, 0, false);
        addPiece(blackPawn, file, 1, false);
        addPiece(whitePawn, file, 6, true);
        addPiece(whiteBackRank[file], file, 7, true);
    }
}
