#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

const int nPieces = 8;

namespace Ui {
class MainWindow;
}

class QSvgRenderer;
class QSettings;

enum PieceType
{
  KING,
  QUEEN,
  ROOK,
  BISHOP,
  KNIGHT,
  UNDEFINED
};

const int nPieceTypes = UNDEFINED;

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  enum ScreenOrientation {
    ScreenOrientationLockPortrait,
    ScreenOrientationLockLandscape,
    ScreenOrientationAuto
  };

  explicit MainWindow (QWidget *parent = 0);
  virtual ~MainWindow ();

  // Note that this will only have an effect on Symbian and Fremantle.
  void setOrientation (ScreenOrientation orientation);

  void showExpanded ();

protected:
  PieceType whitePieces [nPieces];
  PieceType blackPieces [nPieces];

  QSvgRenderer* whiteRenderers [nPieceTypes];
  QSvgRenderer* blackRenderers [nPieceTypes];

  QImage* whiteImages [nPieceTypes];
  QImage* blackImages [nPieceTypes];

  QSettings* appSetting;

  void resizeEvent (QResizeEvent* qEvent);
  bool eventFilter (QObject* qObj, QEvent* qEvent);

  void generateOnePlayerPlacing (PieceType* pieces);
  void generatePlacing ();

private:
  Ui::MainWindow* ui;
  bool isInitialised;

private slots:
  void setupNewPlacing ();
  void saveSettings ();
  void loadSettings ();
};

#endif // MAINWINDOW_H
