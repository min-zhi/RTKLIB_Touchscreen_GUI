// RTKBASE is a GUI interface for RTKLIB made for the Raspberry pî and a touchscreen
//   Copyright (C) 2016
//   David ENSG PPMD-2016 (first rtkbase release)
//   Francklin N'guyen van <francklin2@wanadoo.fr>
//   Sylvain Poulain <sylvain.poulain@giscan.com>
//   Vladimir ENSG PPMD-2017 (editable configuration)
//   Saif Aati ENSG PPMD-2018 (post processing)
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include <QInputDialog>
#include <QTimer>
#include <QThread>
#include <QButtonGroup>
#include "threadheuresysteme.h"
#include <iostream>
#include <stdlib.h>
#include <QVector>
//#include  <vector>
#include "magraphicsscene.h"
#include <options1.h>
#include "MainthreadStr2str.h"
#include "affichestr2str.h"
#include "optionsstr2str.h"
#include "affichesolutions.h"
#include <QGridLayout>
#include "choixconfig.h"
#include "optionsstartatboot.h"

QString StartupMode;
QString ModeatBoot;
QString RoverConfig;



using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QImage image("../RTKBASE/images/rtklib.jpg");
    ui->Rtkliblabel->setPixmap(QPixmap::fromImage(image));
    QPixmap ensgicone("../RTKBASE/images/ensg_icone.jpg");
    QPixmap newPixmap1 = ensgicone.scaled(QSize(105,135),Qt::KeepAspectRatio);
    ui->ensgIconlabel->setPixmap(newPixmap1);
    QPixmap watermark("../RTKBASE/images/Raspberry-Pi.jpg");
    QPixmap newPixmap = watermark.scaled(QSize(65,65),Qt::KeepAspectRatio);
    ui->ensglabel->setPixmap(newPixmap);

    // --------- voir partie "print solution" de RTKCVR pour récupérer données Time / FIX-FLOAT-... / Position...
    // https://openclassrooms.com/courses/la-programmation-systeme-en-c-sous-unix/les-threads-3 //
    //-----------------------------------Affichage heure système---------------------------------------------//

    //QPixmap bkgnd("../RTKBASE/images/ensg_fond.jpg");

    QPixmap bkgnd("../RTKBASE/images/ensg_fond.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    ui->BasestationButton->setIcon(QIcon("../RTKBASE/images/base.jpg"));
    ui->BasestationButton->setIconSize(QSize(80,80));

    ui->RoverButton->setIcon(QIcon("../RTKBASE/images/rover.jpg"));
    ui->RoverButton->setIconSize(QSize(120,120));

    ThreadHeureSysteme* AfficheHeureSysteme = new ThreadHeureSysteme();
    AfficheHeureSysteme->start();

    QObject::connect(AfficheHeureSysteme,SIGNAL(heureretournee(int)),this,SLOT(AfficheHeureSyst(int)));

    //--------------CONNEXION DES BOUTONS---------------------------------------------------------
    QObject::connect(ui->QuitterButton,SIGNAL(clicked()),qApp,SLOT(quit()));
    QObject::connect(ui->RoverButton,SIGNAL(clicked()),this,SLOT(OuvreRover()));
    QObject::connect(ui->BasestationButton,SIGNAL(clicked()),this,SLOT(OuvreAfficheSolutiosStr2str()));
    QObject::connect(ui->HelpButton,SIGNAL(clicked()),this,SLOT(OuvreHelp()));
    QObject::connect(ui->PowerOFFButton,SIGNAL(clicked()),this,SLOT(Shutdown()));
    QObject::connect(ui->Post_Processing_Button,SIGNAL(clicked()),this,SLOT(Post_Processing()));
    QObject::connect(ui->StartupOption_Button,SIGNAL(clicked()),this,SLOT(Startup_Options()));



    // Open startup options configuration file

        {
        int i=1;
        QStringList list;
        QString fileName = "saveoptionstartatboot.txt";
        QFile readoption(fileName);
        readoption.open(QIODevice::ReadOnly | QIODevice::Text);
        //---------verifier ouverture fichier......
        QTextStream flux(&readoption);
        QString ligne;
        while(! flux.atEnd())
        {
           ligne = flux.readLine();
           //traitement de la ligne
           qDebug()<<ligne;
           list<<ligne;
           i=i+1;
       }


StartupMode = list[0];
ModeatBoot = list[1];
RoverConfig = list[2];

        readoption.close();

    }

  if(StartupMode=="ON")
  {
  if (ModeatBoot=="ROVER")
     {
  if(RoverConfig == "SINGLE")
  {
    ChoixConfig Start;
    Start.ouvreSingle();
   }
  if(RoverConfig == "SBAS")
  {
    ChoixConfig Start;
    Start.ouvreSBAS();
   }
  if(RoverConfig == "DGPS")
  {
    ChoixConfig Start;
    Start.ouvreDGPS();
   }
  if(RoverConfig == "PPP STATIC")
  {
    ChoixConfig Start;
    Start.ouvrePPP();
   }
  if(RoverConfig == "RTK STATIC")
  {
    ChoixConfig Start;
    Start.ouvreRTKstatic();
   }
  if(RoverConfig == "RTK KINE")
  {
    ChoixConfig Start;
    Start.ouvreRTKkinematic();
   }
  if(RoverConfig == "CUSTOM 1")
  {
    ChoixConfig Start;
    Start.OuvreConfig1();
   }
  }

if (ModeatBoot=="BASE")
{

    OptionsStr2str Start1;
    Start1.exec();
}



  }





}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::AfficheHeureSyst(int A)
{
    int h=0;
    long int m=0;
    long int s=0;
    h=A/3600;
    m=(A-(h*3600))/60;
    s=A-h*3600-m*60;
    ui->TimeRetourEdit->setText(QString("%1 h %2 min %3 ").arg(h).arg(m).arg(s));
}

void MainWindow::OuvreOptions()
{
    //plutôt ouvrir une QDialog modale? (ie tant qu'elle n'est pas fermee on ne peut pas revenir sur la principale)
    // http://irmatden.developpez.com/tutoriels/dialog/

}

void MainWindow::OuvreRover()
{
    /*
    Options1 OptionsRover;
    OptionsRover.exec();*/
    /*Options1 afficheOptions1;

    afficheOptions1.setModal(true);
    afficheOptions1.setWindowFlags(Qt::FramelessWindowHint);

    afficheOptions1.exec();*/

    ChoixConfig FenetreChoixConfigRover(0);
    //FenetreChoixConfigRover->setModal(true);
    //FenetreChoixConfigRover.setWindowFlags(Qt::FramelessWindowHint);
    FenetreChoixConfigRover.exec();


}

void MainWindow::OuvreAfficheSolutiosStr2str()
{
    OptionsStr2str afficheOptionsStr2str;
    afficheOptionsStr2str.setModal(true);
    afficheOptionsStr2str.setWindowFlags(Qt::FramelessWindowHint);

    afficheOptionsStr2str.exec();

   /* AfficheStr2str affichesolutionsStr2str;
    affichesolutionsStr2str.exec();*/
}

void MainWindow::Shutdown()
{

    QProcess process;
    process.startDetached("sudo halt");

}

void MainWindow::OuvreHelp()
{
    QDesktopServices::openUrl(QUrl("../RTKBASE/doc/manual_RTKBASE.pdf"));
    //this->hide();
    //qApp->quit();//Removed : quit button close RTKBASE
}



void MainWindow::Post_Processing()                     //added by ENSG student SAIF AATI
{

   MyDialog  affichemydialog;
    affichemydialog.setModal(true);
    affichemydialog.setWindowFlags(Qt::FramelessWindowHint);
    affichemydialog.setWindowState(Qt::WindowFullScreen);
    affichemydialog.exec();
}

void MainWindow::Startup_Options()
{
    optionsstartatboot StartupOptions;
    StartupOptions.setModal(true);
    StartupOptions.setWindowFlags(Qt::FramelessWindowHint);
    StartupOptions.setWindowState(Qt::WindowFullScreen);
    StartupOptions.exec();

}

