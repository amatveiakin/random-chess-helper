// TODO: Delete
#include <QDebug>



#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>

#include <QDateTime>
#include <QLayout>
#include <QResizeEvent>
#include <QtSvg/QSvgWidget>

// TODO: Allow user to change it
const bool forceDifferentBishops = true;
const bool forceKingBetweenRooks = true;

class PieceWidget : public QSvgWidget
{
public:
  PieceWidget (const QString& file, int widgetSize, QWidget* parent)
    : QSvgWidget (file, parent)
  {
    setMinimumSize (widgetSize, widgetSize);
    setMaximumSize (widgetSize, widgetSize);
  }
};

MainWindow::MainWindow (QWidget *parent)
  : QMainWindow (parent), ui (new Ui::MainWindow)
{
  ui->setupUi (this);

  kingW    = 0;
  queenW   = 0;
  rook1W   = 0;
  rook2W   = 0;
  bishop1W = 0;
  bishop2W = 0;
  knight1W = 0;
  knight2W = 0;

  kingB    = 0;
  queenB   = 0;
  rook1B   = 0;
  rook2B   = 0;
  bishop1B = 0;
  bishop2B = 0;
  knight1B = 0;
  knight2B = 0;

  whiteLayout = 0;
  blackLayout = 0;
}

MainWindow::~MainWindow ()
{
  delete ui;
}

void MainWindow::setOrientation (ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
  // If the version of Qt on the device is < 4.7.2, that attribute won't work
  if (orientation != ScreenOrientationAuto) {
    const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
    if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
      qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
      return;
    }
  }
#endif // Q_OS_SYMBIAN

  Qt::WidgetAttribute attribute;
  switch (orientation) {
#if QT_VERSION < 0x040702
  // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
  case ScreenOrientationLockPortrait:
    attribute = static_cast<Qt::WidgetAttribute>(128);
    break;
  case ScreenOrientationLockLandscape:
    attribute = static_cast<Qt::WidgetAttribute>(129);
    break;
  default:
  case ScreenOrientationAuto:
    attribute = static_cast<Qt::WidgetAttribute>(130);
    break;
#else // QT_VERSION < 0x040702
  case ScreenOrientationLockPortrait:
    attribute = Qt::WA_LockPortraitOrientation;
    break;
  case ScreenOrientationLockLandscape:
    attribute = Qt::WA_LockLandscapeOrientation;
    break;
  default:
  case ScreenOrientationAuto:
    attribute = Qt::WA_AutoOrientation;
    break;
#endif // QT_VERSION < 0x040702
  };
  setAttribute(attribute, true);

  ui->whitePiecesWidget->installEventFilter (this);
}

void MainWindow::showExpanded ()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
  showFullScreen();
#elif defined(Q_WS_MAEMO_5)
  showMaximized();
#else
  show();
#endif
}

bool MainWindow::eventFilter (QObject *qObj, QEvent *qEvent)
{
  if (qObj == ui->whitePiecesWidget)
  {
    if (qEvent->type () == QEvent::Resize)
    {
      if (!isInitialised)
        // TODO: Fix strange layout underfull bug
        initialize (static_cast <QResizeEvent*>(qEvent)->size ().width () / nPieces + 1);
    }
    return false;
  }
  else
    return QMainWindow::eventFilter(qObj, qEvent);
}

void MainWindow::initialize (int pieceSize)
{
  srand (QDateTime::currentDateTime().toMSecsSinceEpoch ());

  whiteLayout = new QHBoxLayout;
  blackLayout = new QHBoxLayout;

  whiteLayout->setSpacing (0);
  blackLayout->setSpacing (0);

  kingW    = new PieceWidget ("../chess_pieces/Chess_klt45.svg", pieceSize, this);
  queenW   = new PieceWidget ("../chess_pieces/Chess_qlt45.svg", pieceSize, this);
  rook1W   = new PieceWidget ("../chess_pieces/Chess_rlt45.svg", pieceSize, this);
  rook2W   = new PieceWidget ("../chess_pieces/Chess_rlt45.svg", pieceSize, this);
  bishop1W = new PieceWidget ("../chess_pieces/Chess_blt45.svg", pieceSize, this);
  bishop2W = new PieceWidget ("../chess_pieces/Chess_blt45.svg", pieceSize, this);
  knight1W = new PieceWidget ("../chess_pieces/Chess_nlt45.svg", pieceSize, this);
  knight2W = new PieceWidget ("../chess_pieces/Chess_nlt45.svg", pieceSize, this);

  kingB    = new PieceWidget ("../chess_pieces/Chess_kdt45.svg", pieceSize, this);
  queenB   = new PieceWidget ("../chess_pieces/Chess_qdt45.svg", pieceSize, this);
  rook1B   = new PieceWidget ("../chess_pieces/Chess_rdt45.svg", pieceSize, this);
  rook2B   = new PieceWidget ("../chess_pieces/Chess_rdt45.svg", pieceSize, this);
  bishop1B = new PieceWidget ("../chess_pieces/Chess_bdt45.svg", pieceSize, this);
  bishop2B = new PieceWidget ("../chess_pieces/Chess_bdt45.svg", pieceSize, this);
  knight1B = new PieceWidget ("../chess_pieces/Chess_ndt45.svg", pieceSize, this);
  knight2B = new PieceWidget ("../chess_pieces/Chess_ndt45.svg", pieceSize, this);

  connect (ui->generatePushButton, SIGNAL (clicked ()), SLOT (setupNewPlacing ()));

  ui->whitePiecesWidget->setLayout (whiteLayout);
  ui->blackPiecesWidget->setLayout (blackLayout);
}

void MainWindow::clearLayouts ()
{
  whiteLayout->removeWidget (kingW);
  whiteLayout->removeWidget (queenW);
  whiteLayout->removeWidget (rook1W);
  whiteLayout->removeWidget (rook2W);
  whiteLayout->removeWidget (bishop1W);
  whiteLayout->removeWidget (bishop2W);
  whiteLayout->removeWidget (knight1W);
  whiteLayout->removeWidget (knight2W);

  blackLayout->removeWidget (kingB);
  blackLayout->removeWidget (queenB);
  blackLayout->removeWidget (rook1B);
  blackLayout->removeWidget (rook2B);
  blackLayout->removeWidget (bishop1B);
  blackLayout->removeWidget (bishop2B);
  blackLayout->removeWidget (knight1B);
  blackLayout->removeWidget (knight2B);
}

void MainWindow::resetPieceCounters ()
{
  firstRook   = true;
  firstBishop = true;
  firstKnight = true;
}

PieceWidget* MainWindow::getPieceWidget (PieceColor color, PieceType type)
{
  if (color == WHITE)
  {
    switch (type)
    {
    case KING:
      return kingW;
    case QUEEN:
      return queenW;
    case ROOK:
      return firstRook   ? (firstRook = false,   rook1W  ) : rook2W;
    case BISHOP:
      return firstBishop ? (firstBishop = false, bishop1W) : bishop2W;
    case KNIGHT:
      return firstKnight ? (firstKnight = false, knight1W) : knight2W;
    case UNDEFINED:
      abort ();
    }
  }
  else
  {
    switch (type)
    {
    case KING:
      return kingB;
    case QUEEN:
      return queenB;
    case ROOK:
      return firstRook   ? (firstRook = false,   rook1B  ) : rook2B;
    case BISHOP:
      return firstBishop ? (firstBishop = false, bishop1B) : bishop2B;
    case KNIGHT:
      return firstKnight ? (firstKnight = false, knight1B) : knight2B;
    case UNDEFINED:
      abort ();
    }
  }
  abort ();
}

static void addPieceAtPosition (PieceType* pieces, PieceType newPiece, int pos)
{
  for (int i = 0; i < nPieces; i++)
  {
    if (pieces[i] == UNDEFINED)
    {
      if (pos == 0)
      {
        pieces[i] = newPiece;
        return;
      }
      pos--;
    }
  }
  abort ();
}

static void generatePlacing (PieceType* pieces)
{
  std::fill (pieces, pieces + nPieces, UNDEFINED);

  if (forceDifferentBishops)
  {
    pieces [(rand () % 4) * 2]     = BISHOP;
    pieces [(rand () % 4) * 2 + 1] = BISHOP;
  }
  else
  {
    addPieceAtPosition (pieces, BISHOP, rand () % 8);
    addPieceAtPosition (pieces, BISHOP, rand () % 7);
  }

  addPieceAtPosition (pieces, QUEEN,  rand () % 6);
  addPieceAtPosition (pieces, KNIGHT, rand () % 5);
  addPieceAtPosition (pieces, KNIGHT, rand () % 4);

  if (forceKingBetweenRooks)
    addPieceAtPosition (pieces, KING, 1);
  else
    addPieceAtPosition (pieces, KING, rand () % 3);

  addPieceAtPosition (pieces, ROOK, 0);
  addPieceAtPosition (pieces, ROOK, 0);
}

// TODO: fix
void MainWindow::setupNewPlacing ()
{
  PieceType pieces[nPieces];
  generatePlacing (pieces);

  clearLayouts ();

  resetPieceCounters ();
  for (int i = 0; i < nPieces; i++)
    whiteLayout->addWidget (getPieceWidget (WHITE, pieces[i]));

  resetPieceCounters ();
  for (int i = 0; i < nPieces; i++)
    blackLayout->addWidget (getPieceWidget (BLACK, pieces[i]));
}
