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
  explicit OptionsForm (QSettings* appSettings_, QWidget* parent = 0);
  ~OptionsForm ();

  void updateLayout (QSize size);

signals:
  void settingsChanged ();
  void optionsClosed ();

private:
  QSettings* appSettings;

  Ui::OptionsForm* ui;

  void closeEvent (QCloseEvent* event);

private slots:
  void applySettings ();
};

#endif // OPTIONSFORM_H
