/********************************************************************************************
 *                                                                                          *
 *   Auteur : FoxiesCuties                                                                  *
 *   Sujet Officiel : http://www.aeriagames.com/forums/fr/viewtopic.php?t=209434            *
 *   Licence : GPLv3 ( gpl-3.0.txt/gpl-3.0.fr.txt à la racine des sources )                 *
 *                                                                                          *
 ********************************************************************************************/

#include "ThemeTab.hpp"

ThemeTab::ThemeTab(QString edenpath)
{
    m_edenPath = edenpath;


    //m_themeLocalFile = new QFile(QCoreApplication::applicationDirPath()+"/logs/themes_install.list");
    m_themeOnlineFile = new QFile(QCoreApplication::applicationDirPath()+"/logs/themes.list");

    m_themeOnlineInfosFile = new QFile(QCoreApplication::applicationDirPath()+"/logs/themes.info");
    //m_themeLocalInfosFile = new QFile(QCoreApplication::applicationDirPath()+"/logs/themes_install.info");

    m_pathsFile = new QFile(QCoreApplication::applicationDirPath()+"/logs/paths.lst");
        m_pathsFile->open(QIODevice::ReadOnly | QIODevice::Text);
    m_pathsSets = new QSettings(m_pathsFile->fileName(), QSettings::IniFormat);
    m_pathsString = m_pathsSets->value("Paths/EdenEternal").toString();


    setStyleSheet("QWidget{border-radius: 0x;}*{background-color: #555;}");

    localList = new QListWidget;
        localList->setFixedWidth(200);
        localList->setStyleSheet("QListWidget{background-color: #555;}");

    onlineList = new QListWidget;
        onlineList->setFixedWidth(200);
        onlineList->setStyleSheet("QListWidget{background-color: #555;}");

    addButton = new QPushButton("< AJOUTER");
        addButton->setFixedSize(100,30);
        addButton->setStyleSheet("QPushButton{background-color: #444;}"
                               "QPushButton:hover{background-color: #666;}");

    delButton = new QPushButton(" SUPPRIMER >");
        delButton->setFixedSize(100,30);
        delButton->setStyleSheet("QPushButton{background-color: #444;}"
                               "QPushButton:hover{background-color: #666;}");

    majButton = new QPushButton("Update");
        majButton->setFixedSize(100,30);
        majButton->setStyleSheet("QPushButton{background-color: #464;}"
                               "QPushButton:hover{background-color: #484;}");

    createInfoTheme();

    QVBoxLayout *vbox = new QVBoxLayout;
        vbox->setContentsMargins(0, 50, 0, 0);
        vbox->addWidget(addButton, 0, Qt::AlignCenter);
        vbox->addWidget(delButton, 0, Qt::AlignCenter);
        vbox->addWidget(m_infoTheme, 1, Qt::AlignBottom);

    QVBoxLayout *vboxr = new QVBoxLayout;
        vboxr->addWidget(onlineList);
        vboxr->addWidget(majButton);

    QHBoxLayout *hbox = new QHBoxLayout;
        hbox->setContentsMargins(0, 50, 0, 10);
        hbox->addWidget(localList);
        hbox->addLayout(vbox);
        hbox->addLayout(vboxr);

    setLayout(hbox);

    connect(addButton, SIGNAL(clicked()), this, SLOT(addTheme()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delTheme()));
    connect(majButton, SIGNAL(clicked()), this, SLOT(updateListThemes()));

    connect(localList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(infosThemes(QListWidgetItem*)));
    connect(onlineList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(infosThemes(QListWidgetItem*)));

    listThemes();
}

//-----METHODS
void ThemeTab::createInfoTheme()
{
    QLabel *lab1 = new QLabel("Auteur :");
    m_authorLabel = new QLabel("");

    QLabel *lab2 = new QLabel ("Version :");
    m_versionLabel = new QLabel("");

    QLabel *lab3 = new QLabel ("Details :");
    m_infosLabel = new QTextEdit("");

    QGridLayout *grid = new QGridLayout;
        grid->addWidget(lab1, 0, 0, Qt::AlignTop);
        grid->addWidget(m_authorLabel, 0, 1);
        grid->addWidget(lab2, 1, 0, Qt::AlignTop);
        grid->addWidget(m_versionLabel, 1, 1);
        grid->addWidget(lab3, 2, 0, Qt::AlignTop);
        grid->addWidget(m_infosLabel, 2, 1);

    m_infoTheme = new QGroupBox("  Info du theme  ");
        m_infoTheme->setLayout(grid);
        m_infoTheme->setAlignment(Qt::AlignHCenter);
        m_infoTheme->setFixedSize(200, 150);

        m_infoTheme->setStyleSheet("QGroupBox{border:0; background-color: #555;}"
                                   "QGroupBox::title{background-color: #333; border-radius:0; border:0; border-top: 2px solid #555;}");
}

//------SLOTS
void ThemeTab::addTheme()
{
    if(onlineList->currentIndex().row() != -1)
    {
        if(!isUpdated(onlineList->currentItem()->text()) && QDir(m_pathsString+"/themes/"+onlineList->currentItem()->text()).exists())
        {
            QMessageBox hintu;
                hintu.setIcon(QMessageBox::Information);
                hintu.setFixedWidth(250);
                hintu.setText("Voulez vous mettre à jour le thème " + onlineList->currentItem()->text());
                hintu.setInformativeText("de la version " + version(VType::Local) + " vers la version " + version(VType::Online) + " ?");
                hintu.addButton("Oui", QMessageBox::AcceptRole);
                hintu.addButton("Non", QMessageBox::RejectRole);

            if(hintu.exec() == 0)
            {
                installTheme(onlineList->currentItem()->text());
                onlineList->takeItem(onlineList->currentRow());
            }
        }
        else
        {
            QMessageBox hint;
                hint.setIcon(QMessageBox::Information);
                hint.setText("Le théme " + onlineList->currentItem()->text() + " sera ajouter au jeu ?");
                hint.addButton("Oui", QMessageBox::AcceptRole);
                hint.addButton("Non", QMessageBox::RejectRole);

            if(hint.exec() == 0)
            {
                installTheme(onlineList->currentItem()->text());
                localList->addItem(onlineList->takeItem(onlineList->currentRow()));
            }
        }
    }
    else
    {
        QMessageBox hint;
            hint.setIcon(QMessageBox::Warning);
            hint.setText("VEUILLEZ SELECTIONNEZ UN THEME EN PREMIER");
            hint.exec();
    }
}

void ThemeTab::installTheme(QString nameTheme)
{
    m_themeName = nameTheme;

    m_themeOnlineInfosFile->open(QIODevice::ReadOnly | QIODevice::Text);

    if(m_themeOnlineInfosFile->exists())
    {
        QSettings onlineInfosSettings(m_themeOnlineInfosFile->fileName(), QSettings::IniFormat);

        QSslConfiguration sslconfig = QSslConfiguration::defaultConfiguration();

        if(!sslconfig.isNull())
        {
            sslconfig.setProtocol(QSsl::TlsV1_2);

            QNetworkRequest netReq(QUrl(onlineInfosSettings.value(nameTheme+"/Link").toString(), QUrl::StrictMode));
                netReq.setSslConfiguration(sslconfig);

            QNetworkAccessManager *manager = new QNetworkAccessManager;
                manager->get(netReq);

            connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(extractTheme(QNetworkReply*)));
        }



        /*QSettings onlineInfosSettings(m_themeOnlineInfosFile->fileName(), QSettings::IniFormat);

        QUrl urlVersion(onlineInfosSettings.value(nameTheme+"/Link").toString(), QUrl::StrictMode);

        QNetworkRequest netReq(urlVersion);

        QNetworkAccessManager *manager = new QNetworkAccessManager;
            manager->get(netReq);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(extractTheme(QNetworkReply*)));*/
    }
}
void ThemeTab::extractTheme(QNetworkReply* sevenTheme)
{
    QFile *info = new QFile(QCoreApplication::applicationDirPath()+"/CacheThemes/EdenEternal/"+m_themeName+".7z");
        info->open(QIODevice::WriteOnly);
        info->write(sevenTheme->readAll());
        info->close();

    m_themeExtract = new ThemeExtract(m_edenPath, info->fileName());
    m_themeExtract->start();
}

void ThemeTab::delTheme()
{
    if(localList->currentIndex().row() != -1)
    {
        QMessageBox hint;
            hint.setText("Le théme " + localList->currentItem()->text() + " sera retirer du jeu ?");
            hint.addButton("Oui", QMessageBox::AcceptRole);
            hint.addButton("Non", QMessageBox::RejectRole);

        if(hint.exec() == 0)
        {
            removeTheme(localList->currentItem()->text());
            onlineList->addItem(localList->takeItem(localList->currentRow()));
        }
    }
    else
    {
        QMessageBox hint;
            hint.setText("VEUILLEZ SELECTIONNEZ UN THEME EN PREMIER");
            hint.exec();
    }
}
void ThemeTab::removeTheme(QString nameTheme)
{
    QDir directoryTodelete(m_edenPath+"/themes/"+nameTheme);

    directoryTodelete.removeRecursively();
}

void ThemeTab::updateListThemes()
{
    if(majButton->isEnabled())
    {
        majButton->setEnabled(false);
        majButton->setText("Connexion...");
        majButton->setStyleSheet("QPushButton{background-color: #844;}"
                               "QPushButton:hover{background-color: #844;}");
    }

    QSslConfiguration sslconfig = QSslConfiguration::defaultConfiguration();

    if(!sslconfig.isNull())
    {
        sslconfig.setProtocol(QSsl::TlsV1_2);

        QNetworkRequest netReq(QUrl(m_settings.THEME_LIST_URL, QUrl::StrictMode));
            netReq.setSslConfiguration(sslconfig);

        QNetworkAccessManager *manager = new QNetworkAccessManager;
            manager->get(netReq);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(repListThemes(QNetworkReply*)));
    }
}
void ThemeTab::repListThemes(QNetworkReply *listThemesfile)
{
    m_themeOnlineFile->open(QIODevice::WriteOnly);
        m_themeOnlineFile->write(listThemesfile->readAll());
        m_themeOnlineFile->close();

    updateInfosThemes();
}
void ThemeTab::updateInfosThemes()
{
    QSslConfiguration sslconfig = QSslConfiguration::defaultConfiguration();

    if(!sslconfig.isNull())
    {
        sslconfig.setProtocol(QSsl::TlsV1_2);

        QNetworkRequest netReq(QUrl(m_settings.THEME_INFO_URL, QUrl::StrictMode));
            netReq.setSslConfiguration(sslconfig);

        QNetworkAccessManager *manager = new QNetworkAccessManager;
            manager->get(netReq);

        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(repInfosThemes(QNetworkReply*)));
    }
}
void ThemeTab::repInfosThemes(QNetworkReply *listInfosfile)
{
    QFile *infoD = new QFile(QCoreApplication::applicationDirPath()+"/logs/themes.info");
        infoD->open(QIODevice::WriteOnly);
        infoD->write(listInfosfile->readAll());
        infoD->close();

    listThemes();
}

void ThemeTab::infosThemes(QListWidgetItem* item)
{
    if(item->listWidget() == onlineList)
    {
        m_themeOnlineInfosFile->open(QIODevice::ReadOnly | QIODevice::Text);

        if(m_themeOnlineInfosFile->exists())
        {
            QSettings onlineInfosSettings(m_themeOnlineInfosFile->fileName(), QSettings::IniFormat);

            m_authorLabel->setText(onlineInfosSettings.value(item->text()+"/Author").toString());
            m_versionLabel->setText(onlineInfosSettings.value(item->text()+"/Version").toString());
            m_infosLabel->setText(onlineInfosSettings.value(item->text()+"/Info").toString());

            m_themeOnlineInfosFile->close();
        }
    }
    else if(item->listWidget() == localList)
    {
        QFile *m_themeLoc = new QFile(m_pathsString+"/themes/"+localList->currentItem()->text()+"/theme.info");
                m_themeLoc->open(QIODevice::ReadOnly | QIODevice::Text);

        if(m_themeLoc->exists())
        {
            QSettings localInfosSettings(m_themeLoc->fileName(), QSettings::IniFormat);

            m_authorLabel->setText(localInfosSettings.value("Author").toString());
            m_versionLabel->setText(localInfosSettings.value("Version").toString());
            m_infosLabel->setText(localInfosSettings.value("Info").toString());

            m_themeLoc->close();
        }
    }
}

bool ThemeTab::isUpdated(QString string)
{
    //ONLINE
    m_themeOnlineInfosFile->open(QIODevice::ReadOnly | QIODevice::Text);

    if(m_themeOnlineInfosFile->exists())
    {
        QSettings onlineInfosSettings(m_themeOnlineInfosFile->fileName(), QSettings::IniFormat);
        m_onlineVersion = onlineInfosSettings.value(string+"/Version").toInt();
        m_themeOnlineInfosFile->close();
    }

    //LOCAL
    QFile *m_themeLoc = new QFile(m_pathsString+"/themes/"+string+"/theme.info");
            m_themeLoc->open(QIODevice::ReadOnly | QIODevice::Text);

    if(m_themeLoc->exists())
    {
        QSettings localInfosSettings(m_themeLoc->fileName(), QSettings::IniFormat);

        m_localVersion = localInfosSettings.value("Version").toInt();

        m_themeLoc->close();
    }

    //CONDITION
    if(m_localVersion == m_onlineVersion)
    {
        return true;
    }
    else if(m_localVersion < m_onlineVersion)
    {
        return false;
    }
}

void ThemeTab::listThemes()
{
    if(!majButton->isEnabled())
    {
        majButton->setEnabled(true);
        majButton->setText("Update");
        majButton->setStyleSheet("QPushButton{background-color: #464;}"
                               "QPushButton:hover{background-color: #484;}");
    }

    //LOCAL
    QDir edenThemes(m_edenPath+"/themes");
        edenThemes.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);

    QStringList defaultThemes;
        defaultThemes << "Coffee" << "Blue" << "Green" << "Purple" << "Red";

    localList->clear();

    for (int i = 0; i < edenThemes.entryList().size(); ++i)
    {
        if(!defaultThemes.contains(edenThemes.entryList().at(i)))
        {
            localList->addItem(edenThemes.entryList().at(i));
        }
    }

    //ONLINE
    m_themeOnlineFile->open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream *streamD = new QTextStream(m_themeOnlineFile->readAll(), QIODevice::ReadOnly);

    m_themeOnlineFile->close();

    onlineList->clear();

    while(!streamD->atEnd())
    {
        QString itTheme = streamD->readLine();

        if(!edenThemes.entryList().contains(itTheme))
        {
            onlineList->addItem(itTheme);
        }
        else
        {
            if(!isUpdated(itTheme))
                onlineList->addItem(new QListWidgetItem(QIcon(":/icons/tabs/upd"), itTheme));
        }
    }
}


QString ThemeTab::version(VType type)
{
    QString result;

    if(type == VType::Local)
    {
        QFile *m_themeLoc = new QFile(m_pathsString+"/themes/"+onlineList->currentItem()->text()+"/theme.info");
                m_themeLoc->open(QIODevice::ReadOnly | QIODevice::Text);

        if(m_themeLoc->exists())
        {
            QSettings localInfosSettings(m_themeLoc->fileName(), QSettings::IniFormat);

            result = localInfosSettings.value("Version").toString();

            m_themeLoc->close();
        }
    }
    else if(type == VType::Online)
    {
        m_themeOnlineInfosFile->open(QIODevice::ReadOnly | QIODevice::Text);

        if(m_themeOnlineInfosFile->exists())
        {
            QSettings onlineInfosSettings(m_themeOnlineInfosFile->fileName(), QSettings::IniFormat);

            result = onlineInfosSettings.value(onlineList->currentItem()->text()+"/Version").toString();

            m_themeOnlineInfosFile->close();
        }
    }

    return result;
}

