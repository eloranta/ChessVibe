#include "mainwindow.h"

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QFont>
#include <QPainter>
#include <QtGlobal>

namespace {
constexpr int kBoardSize = 8;
constexpr int kSquareSize = 64;
constexpr int kBoardMargin = 28;
constexpr int kBoardOrigin = kBoardMargin;
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

    void setLocationBit(int file, int rank)
    {
        const int index = rank * kBoardSize + file;
        locationBit = (quint64(1) << index);
    }

    quint64 locationBit = 0;

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        QGraphicsSimpleTextItem::mouseReleaseEvent(event);

        const QRectF bounds = boundingRect();
        const QPointF pos = this->pos();
        const QPointF center = pos + QPointF(bounds.width() / 2.0, bounds.height() / 2.0);
        const QPointF localCenter = center - QPointF(kBoardOrigin, kBoardOrigin);

        int file = static_cast<int>(localCenter.x() / kSquareSize);
        int rank = static_cast<int>(localCenter.y() / kSquareSize);
        file = qBound(0, file, kBoardSize - 1);
        rank = qBound(0, rank, kBoardSize - 1);

        const qreal targetX = kBoardOrigin + file * kSquareSize + (kSquareSize - bounds.width()) / 2.0;
        const qreal targetY = kBoardOrigin + rank * kSquareSize + (kSquareSize - bounds.height()) / 2.0;
        setPos(targetX, targetY);
        setLocationBit(file, rank);
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
    const int sceneSize = boardPx + (kBoardMargin * 2);
    scene->setSceneRect(0, 0, sceneSize, sceneSize);
    view->setFixedSize(sceneSize, sceneSize);

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
                kBoardOrigin + file * kSquareSize,
                kBoardOrigin + rank * kSquareSize,
                kSquareSize,
                kSquareSize,
                QPen(Qt::NoPen),
                QBrush(isDark ? dark : light)
            );
            square->setZValue(0);
        }
    }

    QFont coordFont("Segoe UI", 10);
    for (int xPos = 0; xPos < kBoardSize; ++xPos) {
        const QString label = QString(QChar('a' + xPos));
        auto *top = scene->addSimpleText(label, coordFont);
        auto *bottom = scene->addSimpleText(label, coordFont);

        const qreal x = kBoardOrigin + xPos * kSquareSize + (kSquareSize - top->boundingRect().width()) / 2.0;
        top->setPos(x, 4);
        bottom->setPos(x, kBoardOrigin + kBoardSize * kSquareSize + 4);
    }

    for (int yPos = 0; yPos < kBoardSize; ++yPos) {
        const QString label = QString::number(kBoardSize - yPos);
        auto *left = scene->addSimpleText(label, coordFont);
        auto *right = scene->addSimpleText(label, coordFont);

        const qreal y = kBoardOrigin + yPos * kSquareSize + (kSquareSize - left->boundingRect().height()) / 2.0;
        left->setPos(4, y);
        right->setPos(kBoardOrigin + kBoardSize * kSquareSize + 8, y);
    }
}

void MainWindow::addPiece(const QString &label, int file, int rank, bool isWhite)
{
    const QColor textColor = isWhite ? QColor(30, 30, 30) : QColor(230, 230, 230);

    const int x = kBoardOrigin + file * kSquareSize;
    const int y = kBoardOrigin + rank * kSquareSize;

    auto *text = new PieceItem(label);
    QFont font("Segoe UI Symbol", 28);
    text->setFont(font);
    text->setBrush(textColor);
    text->setLocationBit(file, rank);

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
