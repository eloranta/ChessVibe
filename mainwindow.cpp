#include "mainwindow.h"
#include <QApplication>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QPainter>
#include <QtGlobal>
#include <QtMultimedia/QSoundEffect>
#include <QUrl>

#define white true
#define black false

namespace {
constexpr int kBoardSize = 8;
constexpr int kSquareSize = 64;
constexpr int kBoardMargin = 28;
constexpr int kBoardOrigin = kBoardMargin;
}

class PieceItem : public QGraphicsSimpleTextItem
{
public:
    explicit PieceItem(int x, int y, bool isWhite)
        : QGraphicsSimpleTextItem(), xPosition(x), yPosition(y), isWhite(isWhite)
    {
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setZValue(1);
        firstMove = true;
    }
protected:
    bool isSquareOccupied(int x, int y) const
    {
        for (auto piece : pieces){
            if (x == piece->xPosition && y == piece->yPosition)
                return true;
        }
        return false;
    }

    static QSoundEffect *moveSound()
    {
        static QSoundEffect *effect = nullptr;
        if (!effect) {
            effect = new QSoundEffect;
            effect->setSource(QUrl::fromLocalFile("c:/zown/valid.wav"));
            effect->setVolume(1.0f);
        }
        return effect;
    }

    static QSoundEffect *invalidSound()
    {
        static QSoundEffect *effect = nullptr;
        if (!effect) {
            effect = new QSoundEffect;
            effect->setSource(QUrl::fromLocalFile("c:/zown/invalid.wav"));
            effect->setVolume(1.0f);
        }
        return effect;
    }

    static void playMoveSound()
    {
        QSoundEffect *effect = moveSound();
        if (effect->source().isEmpty()) {
            return;
        }
        effect->play();
    }

    static void playInvalidMoveSound()
    {
        QSoundEffect *effect = invalidSound();
        if (effect->source().isEmpty()) {
            return;
        }
        effect->play();
    }

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
        if (isValidMove(xPos, yPos))
        {
            setPos(targetX, targetY);
            if (xPos != xPosition || yPos != yPosition) {
                xPosition = xPos;
                yPosition = yPos;
                playMoveSound();
                turn = !turn;
                firstMove = false;
                return;
            }
        }
        const qreal resetX = kBoardOrigin + xPosition * kSquareSize + (kSquareSize - bounds.width()) / 2.0;
        const qreal resetY = kBoardOrigin + yPosition * kSquareSize + (kSquareSize - bounds.height()) / 2.0;
        setPos(resetX, resetY);
        playInvalidMoveSound();
    }
protected:
    virtual bool isValidMove(int , int)
    {
        if (isWhite == turn)
            return true;
        return false;
    }
public:
    int xPosition;
    int yPosition;
    bool isWhite;
    static bool turn;
    bool firstMove;
    static QList<PieceItem *> pieces;
};

class Pawn : public PieceItem
{
public:
    explicit Pawn(int x, int y, bool isWhite)
        : PieceItem(x, y, isWhite)
    {
        setText(isWhite ? "\u2659" : "\u265F");
    }
    bool isValidMove(int x, int y) override
    {
        if (!PieceItem::isValidMove(x, y))
            return false;

        if (x == xPosition)
        {
            int dx = isWhite ? -1 : 1;
            if (y == yPosition + dx) {
                return !isSquareOccupied(x, y);
            }
            if (firstMove)
            {
                const int step1 = yPosition + dx;
                const int step2 = yPosition + (dx * 2);
                if (y == step2) {
                    return !isSquareOccupied(x, step1) && !isSquareOccupied(x, y);
                }
            }
        }
        return false;
    }
};

class Knight : public PieceItem
{
public:
    explicit Knight(int x, int y, bool isWhite)
        : PieceItem(x, y, isWhite)
    {
        setText(isWhite ? "\u265E" : "\u2658");
    }
    bool isValidMove(int x, int y) override
    {
        if (!PieceItem::isValidMove(x, y)) {
            return false;
        }

        if (isSquareOccupied(x, y)) {
            return false;
        }

        const int dx = qAbs(x - xPosition);
        const int dy = qAbs(y - yPosition);
        return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
    }
  };

class Bishop : public PieceItem
{
public:
    explicit Bishop(int x, int y, bool isWhite)
        : PieceItem(x, y, isWhite)
    {
        setText(isWhite ? "\u265D" : "\u2657");
    }

    bool isValidMove(int x, int y) override
    {
        if (!PieceItem::isValidMove(x, y)) {
            return false;
        }

        const int dx = x - xPosition;
        const int dy = y - yPosition;
        if (qAbs(dx) != qAbs(dy) || dx == 0) {
            return false;
        }

        const int stepX = (dx > 0) ? 1 : -1;
        const int stepY = (dy > 0) ? 1 : -1;
        const int steps = qAbs(dx);
        for (int i = 1; i < steps; ++i) {
            if (isSquareOccupied(xPosition + i * stepX, yPosition + i * stepY)) {
                return false;
            }
        }

        return !isSquareOccupied(x, y);
    }
};

class Rook : public PieceItem
{
public:
    explicit Rook(int x, int y, bool isWhite)
        : PieceItem(x, y, isWhite)
    {
        setText(isWhite ? "\u265C" : "\u2656");
    }
};

class Queen : public PieceItem
{
public:
    explicit Queen(int x, int y, bool isWhite)
        : PieceItem(x, y, isWhite)
    {
        setText(isWhite ? "\u265B" : "\u2655");
    }
};

class King : public PieceItem
{
public:
    explicit King(int x, int y, bool isWhite)
        : PieceItem(x, y, isWhite)
    {
        setText(isWhite ? "\u265A" : "\u2654");
    }

};

bool PieceItem::turn = white;
QList<PieceItem *> PieceItem::pieces;

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

    addPieces();
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

void MainWindow::addPiece(PieceItem *item)
{
    const QColor textColor = item->isWhite ? QColor(230, 230, 230) : QColor(30, 30, 30);

    const int x = kBoardOrigin + item->xPosition * kSquareSize;
    const int y = kBoardOrigin + item->yPosition * kSquareSize;

    QFont font("Segoe UI Symbol", 28);
    font.setBold(true);
    item->setFont(font);
    item->setBrush(textColor);
    auto *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(4);
    shadow->setOffset(0, 0);
    shadow->setColor(item->isWhite ? QColor(20, 20, 20) : QColor(235, 235, 235));
    item->setGraphicsEffect(shadow);

    const QRectF textBounds = item->boundingRect();
    const qreal centerX = x + (kSquareSize - textBounds.width()) / 2.0;
    const qreal centerY = y + (kSquareSize - textBounds.height()) / 2.0;
    item->setPos(centerX, centerY);
    scene->addItem(item);
    PieceItem::pieces.append(item);
}

void MainWindow::addPieces()
{
    // ✅add Pawns
    for (int x = 0; x < kBoardSize; ++x) {
        addPiece(new Pawn(x, 6, white));
        addPiece(new Pawn(x, 1, black));
    }
    // ✅add Rooks
    addPiece(new Rook(0, 7, white));
    addPiece(new Rook(7, 7, white));
    addPiece(new Rook(0, 0, black));
    addPiece(new Rook(7, 0, black));
    // ✅add Knights
    addPiece(new Knight(1, 7, white));
    addPiece(new Knight(6, 7, white));
    addPiece(new Knight(1, 0, black));
    addPiece(new Knight(6, 0, black));
    // ✅add Bishops
    addPiece(new Bishop(2, 7, white));
    addPiece(new Bishop(5, 7, white));
    addPiece(new Bishop(2, 0, black));
    addPiece(new Bishop(5, 0, black));
    // ✅add Queens
    addPiece(new Queen(3, 7, white));
    addPiece(new Queen(3, 0, black));
    // ✅add Kings
    addPiece(new King(4, 7, white));
    addPiece(new King(4, 0, black));


 }
