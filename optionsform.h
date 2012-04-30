#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <QWidget>

namespace Ui {
class OptionsForm;
}

class QSettings;

class OptionsForm : public QWidget
{
  Q_OBJECT

public:
  explicit OptionsForm (QSettings* applicationSettings, QWidget* parent = 0);
  ~OptionsForm ();

protected:
  QSettings* appSettings;

private:
  Ui::OptionsForm *ui;

private slots:
  void applySettings ();
};

#endif // OPTIONSFORM_H
