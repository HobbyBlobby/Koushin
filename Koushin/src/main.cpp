#include <ctime>

#include <QMap>
// #include <QGraphicsScene>

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KConfigGroup>
#include <KConfig>
#include <KStandardDirs>
#include <KMainWindow>

#include "actionmanager.h"
#include "actionparser.h"
#include "building.h"
#include "citizen.h"
#include "player.h"
#include "town.h"
#include <kdebug.h>
#include "GUI/townwidget.h"
#include <QDir>
#include <QStringList>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QObject>
#include <QDockWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include "GUI/resourceinfowidget.h"
#include <QPushButton>
#include "game.h"
#include "GUI/buildinginfowidget.h"
#include <QToolBar>

#include <qglobal.h> //for Q_WS_X11
#include "GUI/constructionmenu.h"
#include "GUI/constructionmenu.h"
#include "GUI/gameview.h"

static const char description[] =
    I18N_NOOP("A round based strategy game.");

static const char version[] = "0.2";
//0.1: the start
//0.2: create buildings, draw graphical map, parse first actions, introduce town editor


int main(int argc, char** argv)
{
    KAboutData about("Koushin", 0, ki18n("Koushin"), version, ki18n(description), KAboutData::License_GPL, ki18n("(C) 2010 Felix Lemke"), KLocalizedString(), 0, "hobbyblobby@arcor.de");
    about.addAuthor(ki18n("Felix Lemke"), KLocalizedString(), "hobbyblobby@arcor.de");
    KCmdLineArgs::init(argc, argv, &about);

#ifdef Q_WS_X11
    QApplication::setGraphicsSystem("raster");
#endif //Q_WS_X11

    KApplication app;
    qsrand(std::time(0));
    
    Koushin::Game* game = new Koushin::Game;
    game->addPlayer("Felix");
    
    Koushin::Player* tester = game->getPlayers().first();
    
    KStandardDirs stdDirs;
    QString townConfigFile = stdDirs.findResource("data", "koushin/data/towns/town.config");
    Koushin::Town* town = new Koushin::Town(tester, new KConfig(townConfigFile));

///@todo later: prefer building downloaded or created by the user (in ~/.kde4/share/apps/Koushin/...) :
    QMap<QString, QString> buildings;
    foreach(QString dirString, stdDirs.resourceDirs("data")) {
      QDir dir(dirString + "koushin/data/buildings/");
      dir.setNameFilters(QStringList() << "*.config");
      foreach(QString entry, dir.entryList()) {
	KConfig config(dir.path() + "/" + entry);
	KConfigGroup general(&config, "general");
	buildings.insert(general.readEntry("name", QString()), dir.path() + "/" + entry);
      }
    }
    tester->setBuildingList(buildings); ///@todo should be part of the game instead of the player

    KMainWindow* window = new KMainWindow;
    window->setMinimumSize(700,500);
    window->show();

    KoushinGUI::GameView* gameView = new KoushinGUI::GameView;
    window->setCentralWidget(gameView);

    gameView->showTownView(tester->getTowns().first());
    gameView->changePlayer(tester);
    
    window->addToolBar(Qt::BottomToolBarArea, new QToolBar()); //draw toolbar to resize the gameView
    
    game->startRound();
    return app.exec();
}
