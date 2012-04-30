#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

const int nPieces = 8;

namespace Ui {
class MainWindow;
}

class QSvgRenderer;
class QSettings;
class QStackedLayout;

class OptionsForm;

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

  void applySettings ();

protected:
  PieceType whitePieces [nPieces];
  PieceType blackPieces [nPieces];

  QSvgRenderer* whiteRenderers [nPieceTypes];
  QSvgRenderer* blackRenderers [nPieceTypes];

  QImage* whiteImages [nPieceTypes];
  QImage* blackImages [nPieceTypes];

  QSettings* appSettings;

  PieceType* getPieces (bool white);
  QSvgRenderer** getRenderers (bool white);
  QImage** getImages (bool white);
  QWidget* getDrawWidget (bool white);
  void repaintWidget (bool white);

  void updateLayout (QSize size);

  void resizeEvent (QResizeEvent* qEvent);
  bool eventFilter (QObject* qObj, QEvent* qEvent);

  void clearPosition ();
  void generateOnePlayerPlacing (PieceType* pieces);
  void generatePlacing ();

private:
  Ui::MainWindow* ui;
  QStackedLayout* mainLayout;
  OptionsForm* optionsForm;
  bool isInitialised;

public slots:
  void showOptions ();
  void hideOptions ();

private slots:
  void setupNewPlacing ();
};

#endif // MAINWINDOW_H
