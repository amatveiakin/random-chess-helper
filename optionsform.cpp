#include <QSettings>

#include "optionsform.h"
#include "ui_optionsform.h"

OptionsForm::OptionsForm (QSettings *applicationSettings, QWidget *parent) :
  QWidget (parent),
  ui (new Ui::OptionsForm)
{
  ui->setupUi (this);

  appSettings = applicationSettings;
  ui->forceOppositeColoredBishopsCheckBox->setChecked (appSettings->value ("forceOppositeColoredBishops", false).toBool ());
  ui->forceKingBetweenRooksCheckBox      ->setChecked (appSettings->value ("forceKingBetweenRooks",       false).toBool ());
  ui->forceSymmetricPlacingCheckBox      ->setChecked (appSettings->value ("forceSymmetricPlacing",       false).toBool ());

  connect (ui->forceOppositeColoredBishopsCheckBox, SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->forceKingBetweenRooksCheckBox,       SIGNAL (toggled (bool)), SLOT (applySettings ()));
  connect (ui->forceSymmetricPlacingCheckBox,       SIGNAL (toggled (bool)), SLOT (applySettings ()));

  connect (ui->buttonBox, SIGNAL (accepted ()), SLOT (close ()));
}

OptionsForm::~OptionsForm()
{
  delete ui;
}

void OptionsForm::applySettings ()
{
  appSettings->setValue ("forceOppositeColoredBishops",  ui->forceOppositeColoredBishopsCheckBox->isChecked ());
  appSettings->setValue ("forceKingBetweenRooks",        ui->forceKingBetweenRooksCheckBox->      isChecked ());
  appSettings->setValue ("forceSymmetricPlacing",        ui->forceSymmetricPlacingCheckBox->      isChecked ());
}
