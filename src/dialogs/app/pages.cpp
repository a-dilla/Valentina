/************************************************************************
 **
 **  @file   pages.cpp
 **  @author Roman Telezhinsky <dismine@gmail.com>
 **  @date   12 2, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "pages.h"
#include "../../options.h"

ConfigurationPage::ConfigurationPage(QWidget *parent):
    QWidget(parent), autoSaveCheck(0), autoTime(0), langCombo(0), osOptionCheck(0), langChanged(false)
{
    QGroupBox *saveGroup = SaveGroup();
    QGroupBox *langGroup = LangGroup();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(saveGroup);
    mainLayout->addWidget(langGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void ConfigurationPage::Apply()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                       QApplication::applicationName());
    settings.setValue("configuration/autosave/state", autoSaveCheck->isChecked());
    settings.setValue("configuration/autosave/time", autoTime->value());
    //settings.setValue("configuration/osSeparator", osOptionCheck->isChecked());
    if (langChanged)
    {
        QString locale = qvariant_cast<QString>(langCombo->itemData(langCombo->currentIndex()));
        settings.setValue("configuration/locale", locale);
        langChanged = false;
        QString text = QString(tr("Setup user interface language updated and will be used the next time start") + " " +
                               QApplication::applicationName());
        QMessageBox::information(this, QApplication::applicationName(), text);
    }
}

void ConfigurationPage::LangChenged()
{
    langChanged = true;
}

QGroupBox *ConfigurationPage::SaveGroup()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                       QApplication::applicationName());

    QGroupBox *saveGroup = new QGroupBox(tr("Save"));

    autoSaveCheck = new QCheckBox(tr("Auto-save modified pattern"));
    bool autoSaveValue = settings.value("configuration/autosave/state", 1).toBool();
    autoSaveCheck->setChecked(autoSaveValue);

    QLabel *intervalLabel = new QLabel(tr("Interval:"));

    autoTime = new QSpinBox();
    bool ok = true;
    qint32 autoTimeValue = settings.value("configuration/autosave/time", 5).toInt(&ok);
    if (ok == false)
    {
        autoTimeValue = 5;
    }
    autoTime->setRange(1, 60);
    autoTime->setValue(autoTimeValue);
    autoTime->setSuffix(tr("min"));

    QHBoxLayout *autosaveLayout = new QHBoxLayout;
    autosaveLayout->addWidget(autoSaveCheck);
    autosaveLayout->addWidget(intervalLabel);
    autosaveLayout->addWidget(autoTime);

    QVBoxLayout *saveLayout = new QVBoxLayout;
    saveLayout->addLayout(autosaveLayout);
    saveGroup->setLayout(saveLayout);
    return saveGroup;
}

QGroupBox *ConfigurationPage::LangGroup()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                       QApplication::applicationName());

    QGroupBox *langGroup = new QGroupBox(tr("Language"));
    QLabel *guiLabel = new QLabel(tr("GUI language"));
    langCombo = new QComboBox;

    // format systems language
    QString defaultLocale = QLocale::system().name();       // e.g. "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "de"
    QString checkedLocale = settings.value("configuration/locale", defaultLocale).toString();

    QString m_langPath = QApplication::applicationDirPath();
    m_langPath.append(translationsPath);
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("valentina_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i)
    {
        // get locale extracted by filename
        QString locale;
        locale = fileNames[i];                  // "valentina_de.qm"
        locale.truncate(locale.lastIndexOf('.'));   // "valentina_de"
        locale.remove(0, locale.indexOf('_') + 1);   // "de"

        QString lang = QLocale(locale).nativeLanguageName();
        QIcon ico(QString("%1/%2.png").arg("://icon/flags").arg(locale));

        langCombo->addItem(ico, lang, locale);
    }

    QIcon ico(QString("%1/%2.png").arg("://icon/flags").arg("en"));
    QString lang = QLocale("en").nativeLanguageName();
    langCombo->addItem(ico, lang, "en");

    // set default translators and language checked
    qint32 index = langCombo->findData(checkedLocale);
    if (index != -1)
    {
        langCombo->setCurrentIndex(index);
    }
    connect(langCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &ConfigurationPage::LangChenged);

    QHBoxLayout *guiLangLayout = new QHBoxLayout;
    guiLangLayout->addWidget(guiLabel);
    guiLangLayout->addWidget(langCombo);

    QLabel *separatorLabel = new QLabel(tr("Decimal separator parts"));

    osOptionCheck = new QCheckBox(tr("With OS options (.)"));
    //bool osOptionValue = settings.value("configuration/osSeparator", 1).toBool();
    //osOptionCheck->setChecked(osOptionValue);
    osOptionCheck->setEnabled(false);

    QHBoxLayout *separatorLayout = new QHBoxLayout;
    separatorLayout->addWidget(separatorLabel);
    separatorLayout->addWidget(osOptionCheck);

    QVBoxLayout *langLayout = new QVBoxLayout;
    langLayout->addLayout(guiLangLayout);
    langLayout->addLayout(separatorLayout);
    langGroup->setLayout(langLayout);
    return langGroup;
}


PatternPage::PatternPage(QWidget *parent):
    QWidget(parent), userName(0), graphOutputCheck(0), undoneCount(0)
{
    QGroupBox *userGroup = UserGroup();
    QGroupBox *graphOutputGroup = GraphOutputGroup();
    QGroupBox *undoneGroup = UndoneGroup();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(userGroup);
    mainLayout->addWidget(graphOutputGroup);
    mainLayout->addWidget(undoneGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void PatternPage::Apply()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                       QApplication::applicationName());
    settings.setValue("pattern/user", userName->text());
    //settings.setValue("pattern/graphicalOutput", graphOutputCheck->isChecked());
    settings.setValue("pattern/undone", undoneCount->value());
}

QGroupBox *PatternPage::UserGroup()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
                       QApplication::applicationName());

    QGroupBox *userGroup = new QGroupBox(tr("User"));
    QLabel *nameLabel = new QLabel(tr("User name"));

    userName = new QLineEdit;
#ifdef Q_OS_WIN
    QString user = settings.value("pattern/user", QString::fromLocal8Bit(qgetenv("USERNAME").constData())).toString();
#else
    QString user = settings.value("pattern/user", QString::fromLocal8Bit(qgetenv("USER").constData())).toString();
#endif
    userName->setText(user);

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(userName);

    QVBoxLayout *userLayout = new QVBoxLayout;
    userLayout->addLayout(nameLayout);
    userGroup->setLayout(userLayout);
    return userGroup;
}

QGroupBox *PatternPage::GraphOutputGroup()
{
//    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
//                       QApplication::applicationName());

    QGroupBox *graphOutputGroup = new QGroupBox(tr("Graphical output"));

    graphOutputCheck = new QCheckBox(tr("Use antialiasing"));
    //bool graphOutputValue = settings.value("pattern/graphicalOutput", 1).toBool();
    //graphOutputCheck->setChecked(graphOutputValue);
    graphOutputCheck->setEnabled(false);

    QHBoxLayout *graphLayout = new QHBoxLayout;
    graphLayout->addWidget(graphOutputCheck);

    QVBoxLayout *graphOutputLayout = new QVBoxLayout;
    graphOutputLayout->addLayout(graphLayout);
    graphOutputGroup->setLayout(graphOutputLayout);
    return graphOutputGroup;
}

QGroupBox *PatternPage::UndoneGroup()
{
    //    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(),
    //                       QApplication::applicationName());

    QGroupBox *undoneGroup = new QGroupBox(tr("Undone"));
    QLabel *undoneLabel = new QLabel(tr("Count steps"));
    undoneCount = new QSpinBox;
//    bool ok = true;
//    qint32 count = settings.value("pattern/undone", 100).toInt(&ok);
//    if (ok == false)
//    {
//        count = 100;
//    }
//    undoneCount->setValue(count);
    undoneCount->setEnabled(false);

    QHBoxLayout *countLayout = new QHBoxLayout;
    countLayout->addWidget(undoneLabel);
    countLayout->addWidget(undoneCount);

    QVBoxLayout *undoneLayout = new QVBoxLayout;
    undoneLayout->addLayout(countLayout);
    undoneGroup->setLayout(undoneLayout);
    return undoneGroup;
}
