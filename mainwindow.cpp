#include "mainwindow.h"
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QPainter>
#include <QtGlobal>

#define white true
#define balck false

namespace {
constexpr int kBoardSize = 8;
constexpr int kSquareSize = 64;
constexpr int kBoardMargin = 28;
constexpr int kBoardOrigin = kBoardMargin;
}

class PieceItem : public QGraphicsSimpleTextItem
{
public:
    explicit PieceItem(int x, int y, const QString &label)
        : QGraphicsSimpleTextItem(label), xPosition(x), yPosition(y)
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
        const QPointF localCenter = center - QPointF(kBoardOrigin, kBoardOrigin);

        int xPos = static_cast<int>(localCenter.x() / kSquareSize);
        int yPos = static_cast<int>(localCenter.y() / kSquareSize);
        xPos = qBound(0, xPos, kBoardSize - 1);
        yPos = qBound(0, yPos, kBoardSize - 1);

        const qreal targetX = kBoardOrigin + xPos * kSquareSize + (kSquareSize - bounds.width()) / 2.0;
        const qreal targetY = kBoardOrigin + yPos * kSquareSize + (kSquareSize - bounds.height()) / 2.0;
        setPos(targetX, targetY);
    }
public:
    int xPosition;
    int yPosition;
};

class Pawn : public PieceItem
{
public:
    explicit Pawn(int x, int y, const QString &label)
        : PieceItem(x, y, label)
    {
    }
};

class Knight : public PieceItem
{

};

class Bishop : public PieceItem
{

};

class Rook : public PieceItem
{

};

class Queen : public PieceItem
{

};

class King : public PieceItem
{

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

    for (int yPos = 0; yPos < kBoardSize; ++yPos) {
        for (int xPos = 0; xPos < kBoardSize; ++xPos) {
            const bool isDark = (yPos + xPos) % 2 == 1;
            auto *square = scene->addRect(
                kBoardOrigin + xPos * kSquareSize,
                kBoardOrigin + yPos * kSquareSize,
                kSquareSize,
                kSquareSize,
                QPen(Qt::NoPen),
                QBrush(isDark ? dark : light)
            );
            square->setZValue(0);
        }
    }

    // ✅add labels
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

void MainWindow::addPiece(const QString &label, int xPos, int yPos, bool isWhite)
{
    const QColor textColor = isWhite ? QColor(230, 230, 230) : QColor(30, 30, 30);

    const int x = kBoardOrigin + xPos * kSquareSize;
    const int y = kBoardOrigin + yPos * kSquareSize;

    auto *text = new PieceItem(0, 0, label);
    QFont font("Segoe UI Symbol", 28);
    text->setFont(font);
    text->setBrush(textColor);
    auto *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(4);
    shadow->setOffset(0, 0);
    shadow->setColor(isWhite ? QColor(20, 20, 20) : QColor(235, 235, 235));
    text->setGraphicsEffect(shadow);

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

    // for (int xPos = 0; xPos < kBoardSize; ++xPos) {
    //     addPiece(blackBackRank[xPos], xPos, 0, false);
    //     addPiece(blackPawn, xPos, 1, false);
    //     addPiece(whitePawn, xPos, 6, true);
    //     addPiece(whiteBackRank[xPos], xPos, 7, true);
    // }

    // ✅add white Pawns
    for (int x = 0; x < kBoardSize; ++x) {
        addPiece(whitePawn, x, 6, true);
    }
    // ✅add black Pawns
    // for (int x = 0; x < kBoardSize; ++x) {
    //     addPiece(blackPawn, x, 1, false);
    // }
 }
