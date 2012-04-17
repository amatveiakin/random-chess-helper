#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

const int nPieces = 8;

namespace Ui {
class MainWindow;
}

class QSvgRenderer;

enum PieceType
{
  UNDEFINED,
  KING,
  QUEEN,
  ROOK,
  BISHOP,
  KNIGHT
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
  QSvgRenderer* kingW;
  QSvgRenderer* queenW;
  QSvgRenderer* rookW;
  QSvgRenderer* bishopW;
  QSvgRenderer* knightW;

  QSvgRenderer* kingB;
  QSvgRenderer* queenB;
  QSvgRenderer* rookB;
  QSvgRenderer* bishopB;
  QSvgRenderer* knightB;

  QPainter* whitePainter;
  QPainter* blackPainter;

private:
  Ui::MainWindow *ui;
  bool isInitialised;

  QSvgRenderer* getWhitePieceRenderer (PieceType type);
  QSvgRenderer* getBlackPieceRenderer (PieceType type);

private slots:
  void setupNewPlacing ();
};

#endif // MAINWINDOW_H
