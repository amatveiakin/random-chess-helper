// TODO: Scale the button-bar so that the cells are not stretched vertically,
//       at least when setting buttons are small.
//       Note. Use ``heightMM ()'', not ``height ()''!

// TOOD: Make the setting buttons role more obvious: they are not looking like
//       toggle buttons on my Nokia 5800

// TODO: Save settings

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>

#include <QDateTime>
#include <QLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QtSvg/QSvgRenderer>

const QColor whiteBackcolor = 0xd18b47;
const QColor blackBackcolor = 0xffce9e;

MainWindow::MainWindow (QWidget *parent)
  : QMainWindow (parent), ui (new Ui::MainWindow)
{
  srand (QDateTime::currentDateTime().toMSecsSinceEpoch ());

  ui->setupUi (this);

  std::fill (whitePieces, whitePieces + nPieces, UNDEFINED);
  std::fill (blackPieces, blackPieces + nPieces, UNDEFINED);

  std::fill (whiteImages, whiteImages + nPieceTypes, (QImage*) 0);
  std::fill (blackImages, blackImages + nPieceTypes, (QImage*) 0);

  whiteRenderers [KING  ] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/white_king.svg"  ), this);
  whiteRenderers [QUEEN ] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/white_queen.svg" ), this);
  whiteRenderers [ROOK  ] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/white_rook.svg"  ), this);
  whiteRenderers [BISHOP] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/white_bishop.svg"), this);
  whiteRenderers [KNIGHT] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/white_knight.svg"), this);

  blackRenderers [KING  ] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/black_king.svg"  ), this);
  blackRenderers [QUEEN ] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/black_queen.svg" ), this);
  blackRenderers [ROOK  ] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/black_rook.svg"  ), this);
  blackRenderers [BISHOP] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/black_bishop.svg"), this);
  blackRenderers [KNIGHT] = new QSvgRenderer (QString::fromLatin1 (":/pictures/pieces/black_knight.svg"), this);

  ui->whitePiecesWidget->installEventFilter (this);
  ui->blackPiecesWidget->installEventFilter (this);

  connect (ui->generatePushButton,                    SIGNAL (clicked ()),     SLOT (setupNewPlacing ()));
  connect (ui->forceOppositeColoredBishopsPushButton, SIGNAL (toggled (bool)), SLOT (updateSetings ()));
  connect (ui->forceKingBetweenRooksPushButton,       SIGNAL (toggled (bool)), SLOT (updateSetings ()));
  connect (ui->forceSymmetricPlacingPushButton,       SIGNAL (toggled (bool)), SLOT (updateSetings ()));

  updateSetings ();
}

MainWindow::~MainWindow ()
{
  delete ui;
  for (int i = 0; i < nPieceTypes; i++) {
    delete whiteRenderers[i];
    delete blackRenderers[i];
    delete whiteImages[i];
    delete blackImages[i];
  }
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

void MainWindow::resizeEvent (QResizeEvent* qEvent)
{
  const double buttonBarSize     = 1. / 3.;
  const int    nSettingButtons   = 3;
  const double settingButtonSize = buttonBarSize / nSettingButtons;

  ui->generatePushButton->                   setMinimumHeight (qEvent->size ().height () * buttonBarSize    );
  ui->forceOppositeColoredBishopsPushButton->setMinimumHeight (qEvent->size ().height () * settingButtonSize);
  ui->forceKingBetweenRooksPushButton->      setMinimumHeight (qEvent->size ().height () * settingButtonSize);
  ui->forceSymmetricPlacingPushButton->      setMinimumHeight (qEvent->size ().height () * settingButtonSize);
}

static QRect shrinkToSquare (const QRect& rect)
{
  int size = qMin (rect.width (), rect.height ());
  QPoint center = rect.center ();
  return QRect (center.x () - size / 2, center.y () - size / 2, size, size);
}

static void renderImages (PieceType* pieces, QWidget* targetWidget, QSvgRenderer** renderers, QImage** images, bool rotatedPieces, bool invertedCells)
{
  if (pieces[0] == UNDEFINED)
    return;

  int pieceWidth  = targetWidget->width () / nPieces;
  int pieceHeight = targetWidget->height ();

  if (!images[0] || images[0]->size () != QSize (pieceWidth, pieceHeight)) {
    for (int i = 0; i < nPieceTypes; i++) {
      QImage curImage (pieceWidth, pieceHeight, QImage::Format_ARGB32_Premultiplied);
      QPainter curPainter (&curImage);
      curImage.fill (Qt::transparent);
      renderers[i]->render (&curPainter, shrinkToSquare (QRect (0, 0, pieceWidth, pieceHeight)));
      delete images[i];
      images[i] = new QImage (curImage.mirrored (rotatedPieces, rotatedPieces));
    }
  }

  QPainter boardPainter (targetWidget);
  for (int i = 0; i < nPieces; i++)
  {
    QRect cellRect = QRect (i * pieceWidth, 0, pieceWidth, pieceHeight);
    boardPainter.fillRect (cellRect, (i % 2) ^ invertedCells ? blackBackcolor : whiteBackcolor);
    boardPainter.drawImage (i * pieceWidth, 0, *images [pieces[i]]);
  }
}

bool MainWindow::eventFilter (QObject* qObj, QEvent* qEvent)
{
  if (qObj == ui->whitePiecesWidget)
  {
    if (qEvent->type () == QEvent::Paint)
    {
      renderImages (whitePieces, ui->whitePiecesWidget, whiteRenderers, whiteImages, false, false);
      return true;
    }
    else
      return false;
  }
  else if (qObj == ui->blackPiecesWidget)
  {
    if (qEvent->type () == QEvent::Paint)
    {
      renderImages (blackPieces, ui->blackPiecesWidget, blackRenderers, blackImages, true, true);
      return true;
    }
    else
      return false;
  }
  else
    return QMainWindow::eventFilter(qObj, qEvent);
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

void MainWindow::generateOnePlayerPlacing (PieceType* pieces)
{
  std::fill (pieces, pieces + nPieces, UNDEFINED);

  if (forceOppositeColoredBishops)
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

void MainWindow::generatePlacing ()
{
  generateOnePlayerPlacing (whitePieces);

  if (forceSymmetricPlacing)
    std::copy (whitePieces, whitePieces + nPieces, blackPieces);
  else
    generateOnePlayerPlacing (blackPieces);
}

void MainWindow::setupNewPlacing ()
{
  generatePlacing ();
  ui->whitePiecesWidget->update ();
  ui->blackPiecesWidget->update ();
}

void MainWindow::updateSetings ()
{
  forceOppositeColoredBishops = ui->forceOppositeColoredBishopsPushButton->isChecked ();
  forceKingBetweenRooks       = ui->forceKingBetweenRooksPushButton->      isChecked ();
  forceSymmetricPlacing       = ui->forceSymmetricPlacingPushButton->      isChecked ();
}
