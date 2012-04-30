#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <QWidget>

namespace Ui {
class OptionsForm;
}

class QCloseEvent;
class QSettings;

class MainWindow;

class OptionsForm : public QWidget
{
  Q_OBJECT

public:
  explicit OptionsForm (QSettings* appSettings_, MainWindow* mainWindow_, QWidget* parent = 0);
  ~OptionsForm ();

protected:
  QSettings* appSettings;
  MainWindow* mainWindow;

  void closeEvent (QCloseEvent *event);

private:
  Ui::OptionsForm *ui;

private slots:
  void applySettings ();
};

#endif // OPTIONSFORM_H
