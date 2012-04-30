#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

const int nPieces = 8;

namespace Ui {
class MainWindow;
}

class PieceWidget;
class QBoxLayout;

enum PieceType
{
  UNDEFINED,
  KING,
  QUEEN,
  ROOK,
  BISHOP,
  KNIGHT
};

enum PieceColor
{
  WHITE,
  BLACK
};

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
  PieceWidget* kingW;
  PieceWidget* queenW;
  PieceWidget* rook1W;
  PieceWidget* rook2W;
  PieceWidget* bishop1W;
  PieceWidget* bishop2W;
  PieceWidget* knight1W;
  PieceWidget* knight2W;

  PieceWidget* kingB;
  PieceWidget* queenB;
  PieceWidget* rook1B;
  PieceWidget* rook2B;
  PieceWidget* bishop1B;
  PieceWidget* bishop2B;
  PieceWidget* knight1B;
  PieceWidget* knight2B;

  QBoxLayout* whiteLayout;
  QBoxLayout* blackLayout;

  bool firstRook;
  bool firstBishop;
  bool firstKnight;

  bool eventFilter (QObject *qObj, QEvent *qEvent);

private:
  Ui::MainWindow *ui;
  bool isInitialised;

  void initialize (int pieceSize);
  void clearLayouts ();
  void resetPieceCounters ();
  PieceWidget* getPieceWidget (PieceColor color, PieceType type);

private slots:
  void setupNewPlacing ();
};

#endif // MAINWINDOW_H
