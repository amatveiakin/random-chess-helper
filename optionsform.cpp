#include "optionsform.h"
#include "ui_optionsform.h"

#include <QSettings>

#include "mainwindow.h"

OptionsForm::OptionsForm (QSettings* appSettings_, MainWindow* mainWindow_, QWidget *parent) :
  QWidget (parent),
  ui (new Ui::OptionsForm)
{
  ui->setupUi (this);

  appSettings = appSettings_;
  mainWindow  = mainWindow_;

  ui->oppositeColoredBishopsCheckBox->setChecked (appSettings->value ("oppositeColoredBishops", true).toBool ());
  ui->kingBetweenRooksCheckBox      ->setChecked (appSettings->value ("kingBetweenRooks",       true).toBool ());
  ui->symmetricPlacingCheckBox      ->setChecked (appSettings->value ("symmetricPlacing",       true).toBool ());
  ui->bughouseModeCheckBox          ->setChecked (appSettings->value ("bughouseMode",               false).toBool ());

  connect (ui->oppositeColoredBishopsCheckBox, SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->kingBetweenRooksCheckBox,       SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->symmetricPlacingCheckBox,       SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->bughouseModeCheckBox,           SIGNAL (toggled (bool)), SLOT (applySettings ()));

  connect (ui->buttonBox, SIGNAL (accepted ()), SLOT (close ()));
}

OptionsForm::~OptionsForm()
{
  delete ui;
}

void OptionsForm::applySettings ()
{
  appSettings->setValue ("oppositeColoredBishops",  ui->oppositeColoredBishopsCheckBox->isChecked ());
  appSettings->setValue ("kingBetweenRooks",        ui->kingBetweenRooksCheckBox->      isChecked ());
  appSettings->setValue ("symmetricPlacing",        ui->symmetricPlacingCheckBox->      isChecked ());
  appSettings->setValue ("bughouseMode",            ui->bughouseModeCheckBox->          isChecked ());
  mainWindow->applySettings ();
}
