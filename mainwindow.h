#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
  explicit MainWindow (QWidget *parent = 0);
  virtual ~MainWindow ();

public slots:
  void applySettings ();
  void showOptions ();
  void hideOptions ();

private:
  PieceType whitePieces [nPieces];
  PieceType blackPieces [nPieces];

  QSvgRenderer* whiteRenderers [nPieceTypes];
  QSvgRenderer* blackRenderers [nPieceTypes];

  QImage* whiteImages [nPieceTypes];
  QImage* blackImages [nPieceTypes];

  QSettings* appSettings;

  Ui::MainWindow* ui;
  QStackedLayout* mainLayout;
  OptionsForm* optionsForm;

  PieceType* getPieces (bool white);
  QSvgRenderer** getRenderers (bool white);
  QImage** getImages (bool white);
  QWidget* getDrawWidget (bool white);

  void resizeEvent (QResizeEvent* qEvent);
  bool eventFilter (QObject* qObj, QEvent* qEvent);

  void repaintWidget (bool white);
  void updateLayout (QSize size);
  void clearPosition ();
  void generateOnePlayerPlacing (PieceType* pieces);
  void generatePlacing ();

private slots:
  void setupNewPlacing ();
};

#endif // MAINWINDOW_H
