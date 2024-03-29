#include "optionsform.h"
#include "ui_optionsform.h"

#include <QCloseEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>

#include "defines.h"
#include "mainwindow.h"


//======================================================================================================================
// public

OptionsForm::OptionsForm (QSettings* appSettings_, QWidget* parent)
  : QWidget (parent)
  , ui (new Ui::OptionsForm)
{
  ui->setupUi (this);

  appSettings = appSettings_;

  ui->oppositeColoredBishopsCheckBox->setChecked (appSettings->value ("oppositeColoredBishops", true).toBool ());
  ui->kingBetweenRooksCheckBox      ->setChecked (appSettings->value ("kingBetweenRooks",       true).toBool ());
  ui->symmetricPlacingCheckBox      ->setChecked (appSettings->value ("symmetricPlacing",       true).toBool ());
  ui->bughouseModeCheckBox          ->setChecked (appSettings->value ("bughouseMode",          false).toBool ());

  connect (ui->oppositeColoredBishopsCheckBox, SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->kingBetweenRooksCheckBox,       SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->symmetricPlacingCheckBox,       SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->bughouseModeCheckBox,           SIGNAL (toggled (bool)), SLOT (applySettings ()));

  connect (ui->buttonBox, SIGNAL (accepted ()), SLOT (close ()));
}

OptionsForm::~OptionsForm ()
{
  delete ui;
}

void OptionsForm::applySettings ()
{
  appSettings->setValue ("oppositeColoredBishops",  ui->oppositeColoredBishopsCheckBox->isChecked ());
  appSettings->setValue ("kingBetweenRooks",        ui->kingBetweenRooksCheckBox->      isChecked ());
  appSettings->setValue ("symmetricPlacing",        ui->symmetricPlacingCheckBox->      isChecked ());
  appSettings->setValue ("bughouseMode",            ui->bughouseModeCheckBox->          isChecked ());
  emit settingsChanged ();
}

void OptionsForm::updateLayout (QSize size)
{
  const int minHeight = qMin (clickableElementSize, size.height () / 6);

  QPushButton* okButton = ui->buttonBox->button (QDialogButtonBox::Ok);
  if (okButton)
    okButton->setMinimumSize (size.width () / 2, minHeight);

  ui->oppositeColoredBishopsCheckBox->setMinimumHeight (minHeight);
  ui->kingBetweenRooksCheckBox      ->setMinimumHeight (minHeight);
  ui->symmetricPlacingCheckBox      ->setMinimumHeight (minHeight);
  ui->bughouseModeCheckBox          ->setMinimumHeight (minHeight);
}


//======================================================================================================================
// private

void OptionsForm::closeEvent (QCloseEvent* event)
{
  event->accept ();
  emit optionsClosed ();
}
