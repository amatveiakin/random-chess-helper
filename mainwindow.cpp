#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>

#include <QDateTime>
#include <QLayout>
#include <QPainter>
#include <QResizeEvent>
#include <QSettings>
#include <QtSvg/QSvgRenderer>

#include "optionsform.h"


const QString companyName     = "Andrei's Soft";
const QString applicationName = "Random chess helper";

const QColor whiteBackcolor = 0xd18b47;
const QColor blackBackcolor = 0xffce9e;


MainWindow::MainWindow (QWidget *parent)
  : QMainWindow (parent), ui (new Ui::MainWindow)
{
  srand (QDateTime::currentDateTime().toMSecsSinceEpoch ());

  ui->setupUi (this);

  clearPosition ();

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

  ui->buttonsLayout->setStretch (0, 1);
  ui->buttonsLayout->setStretch (1, 2);
  ui->buttonsLayout->setStretch (2, 1);

  optionsForm = 0;
  appSettings = new QSettings (companyName, applicationName);

  ui->whitePiecesWidget->installEventFilter (this);
  ui->blackPiecesWidget->installEventFilter (this);

  connect (ui->generatePushButton, SIGNAL (clicked ()), SLOT (setupNewPlacing ()));
  connect (ui->optionsPushButton,  SIGNAL (clicked ()), SLOT (showOptions ()));
  connect (ui->quitPushButton,     SIGNAL (clicked ()), SLOT (close ()));
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
  delete optionsForm;
  delete appSettings;
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

void MainWindow::applySettings ()
{
  clearPosition ();
  updateLayout (size ());
}

void MainWindow::updateLayout (QSize size)
{
  double generateButtionHeightCoeff;
  double otherButtionsHeightCoeff;

  if (appSettings->value ("bughouseMode").toBool ()) {
    generateButtionHeightCoeff = 1. / 4.;
    otherButtionsHeightCoeff   = 1. / 5.;
  }
  else {
    generateButtionHeightCoeff = 1. / 3.;
    otherButtionsHeightCoeff   = 1. / 4.;
  }

  ui->generatePushButton->setMinimumHeight (size.height () * generateButtionHeightCoeff);
  ui->optionsPushButton-> setMinimumHeight (size.height () * otherButtionsHeightCoeff);
  ui->quitPushButton->    setMinimumHeight (size.height () * otherButtionsHeightCoeff);
}

void MainWindow::resizeEvent (QResizeEvent* qEvent)
{
  updateLayout (qEvent->size ());
}

static QRect shrinkToSquare (const QRect& rect)
{
  int size = qMin (rect.width (), rect.height ());
  QPoint center = rect.center ();
  return QRect (center.x () - size / 2, center.y () - size / 2, size, size);
}

// TODO: make images square
static void updateImages (QSvgRenderer** renderers, QImage** images, QSize newSize)
{
  if (!images[0] || images[0]->size () != newSize) {
    for (int i = 0; i < nPieceTypes; i++) {
      delete images[i];
      images[i] = new QImage (newSize.width (), newSize.height (), QImage::Format_ARGB32_Premultiplied);
      QPainter curPainter (images[i]);
      images[i]->fill (Qt::transparent);
      renderers[i]->render (&curPainter, shrinkToSquare (QRect (0, 0, newSize.width (), newSize.height ())));
    }
  }
}

static void renderOneSide (PieceType* pieces, QPainter* targetPainter, QRect targetRect, QSvgRenderer** renderers, QImage** images, bool rotatedPieces, bool invertedCells)
{
  if (pieces[0] == UNDEFINED)
    return;

  QSize pieceSize (targetRect.width () / nPieces, targetRect.height ());

  updateImages (renderers, images, pieceSize);

  for (int i = 0; i < nPieces; i++)
  {
    QRect cellRect = QRect (QPoint (i * pieceSize.width (), 0) + targetRect.topLeft (), pieceSize);
    targetPainter->fillRect (cellRect, (i % 2) ^ invertedCells ? blackBackcolor : whiteBackcolor);
    targetPainter->drawImage (cellRect.topLeft (), images [pieces[i]]->mirrored (rotatedPieces, rotatedPieces));
  }

  targetPainter->setPen (Qt::black);
  targetPainter->drawRect (targetRect.adjusted (0, 0, -1, -1));
}

PieceType* MainWindow::getPieces (bool white)
{
  return white ? whitePieces : blackPieces;
}

QSvgRenderer** MainWindow::getRenderers (bool white)
{
  return white ? whiteRenderers : blackRenderers;
}

QImage** MainWindow::getImages (bool white)
{
  return white ? whiteImages : blackImages;
}

QWidget* MainWindow::getDrawWidget (bool white)
{
  return white ? ui->whitePiecesWidget : ui->blackPiecesWidget;
}

// TODO: make rectangles change layout to horizontal when it's appropriate
void MainWindow::repaintWidget (bool white)
{
  QWidget* targetWidget = getDrawWidget (white);
  QPainter targetPainter (targetWidget);

  if (appSettings->value ("bughouseMode").toBool ()) {
    int height = targetWidget->height () / 2;
    int widgetWidth = targetWidget->width ();
    int width = qBound (widgetWidth * 0.6, height * 8.0, widgetWidth * 0.8);

    renderOneSide (getPieces (white),  &targetPainter, QRect (0,                   0,      width, height), getRenderers (white),  getImages (white),  !white, !white);
    renderOneSide (getPieces (!white), &targetPainter, QRect (widgetWidth - width, height, width, height), getRenderers (!white), getImages (!white), !white,  white);
  }
  else
    renderOneSide (getPieces (white), &targetPainter, QRect (QPoint (), targetWidget->size ()), getRenderers (white), getImages (white), !white, !white);
}

bool MainWindow::eventFilter (QObject* qObj, QEvent* qEvent)
{
  if (qObj == ui->whitePiecesWidget)
  {
    if (qEvent->type () == QEvent::Paint)
    {
      repaintWidget (true);
      return true;
    }
    else
      return false;
  }
  else if (qObj == ui->blackPiecesWidget)
  {
    if (qEvent->type () == QEvent::Paint)
    {
      repaintWidget (false);
      return true;
    }
    else
      return false;
  }
  else
    return QMainWindow::eventFilter(qObj, qEvent);
}

void MainWindow::clearPosition ()
{
  std::fill (whitePieces, whitePieces + nPieces, UNDEFINED);
  std::fill (blackPieces, blackPieces + nPieces, UNDEFINED);
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

  if (appSettings->value ("oppositeColoredBishops").toBool ())
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

  if (appSettings->value ("kingBetweenRooks").toBool ())
    addPieceAtPosition (pieces, KING, 1);
  else
    addPieceAtPosition (pieces, KING, rand () % 3);

  addPieceAtPosition (pieces, ROOK, 0);
  addPieceAtPosition (pieces, ROOK, 0);
}

void MainWindow::generatePlacing ()
{
  generateOnePlayerPlacing (whitePieces);

  if (appSettings->value ("symmetricPlacing").toBool ())
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

void MainWindow::showOptions ()
{
  if (!optionsForm)
    optionsForm = new OptionsForm (appSettings, this);
  optionsForm->show ();
}
