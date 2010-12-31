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

#include <qglobal.h> //for Q_WS_X11
#include "GUI/constructionmenu.h"
#include "GUI/constructionmenu.h"
#include "GUI/gameview.h"

static const char description[] =
    I18N_NOOP("A round based strategy game.");

static const char version[] = "0.2";
//0.1: the start
//0.2: create buildings, draw graphical map, parse first actions, introduce town editor

namespace KoushinGUI {
  class AutoresizeView : public QGraphicsView {
  public:
    AutoresizeView() : QGraphicsView(), m_centralItem(0), testWidget(0), m_rect(new QGraphicsRectItem()) {}
    void setCentralItem(QGraphicsItem* item) {m_centralItem = item;}
    void resizeEvent(QResizeEvent* event) {
      fitInView(m_centralItem->boundingRect(),Qt::KeepAspectRatio);
      if(testWidget && scene()) {
	testWidget->repaint();
	m_rect->setBrush(QBrush(Qt::red));
	m_rect->setRect(testWidget->geometry());
// 	scene()->addItem(m_rect);
      }
    }
    QWidget* testWidget;
  private:
    QGraphicsItem* m_centralItem;
    QGraphicsRectItem* m_rect;
  };
}


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
    kDebug() << "Found file " << townConfigFile;
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

///@todo this will be part of the GameView later:
//     QGraphicsScene* scene = new QGraphicsScene;
//     KoushinGUI::AutoresizeView* view = new KoushinGUI::AutoresizeView;
//     view->setScene(scene);
//     scene->addItem(town->getTownWidget());
//     view->setCentralItem(town->getTownWidget());
//     town->getTownWidget()->scale(40,40);
//     view->resize(scene->sceneRect().size().toSize());
//     view->fitInView(town->getTownWidget()->boundingRect().toRect(), Qt::KeepAspectRatio);
    
    QDockWidget* tmpView = new QDockWidget;
    QWidget* tmpViewWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    tmpViewWidget->setLayout(layout);
    KoushinGUI::ResourceInfoWidget* infos = new KoushinGUI::ResourceInfoWidget;
    layout->addWidget(infos);
    tester->setResourceInfoWidget(infos);
    KoushinGUI::BuildingInfoWidget* buildingInfo = new KoushinGUI::BuildingInfoWidget;
    tester->setBuildingInfoWidget(buildingInfo);
    layout->addWidget(buildingInfo);
    QPushButton* endRoundButton = new QPushButton("End Round");
    layout->addWidget(endRoundButton);
    tmpView->setWidget(tmpViewWidget);
    
    QObject::connect(endRoundButton, SIGNAL(clicked(bool)), tester, SLOT(endRound()));
//end of GameView part.
    
    KMainWindow* window = new KMainWindow;
    window->setMinimumSize(500,500);
    window->show();

    KoushinGUI::GameView* gameView = new KoushinGUI::GameView;
    gameView->showTownView(tester->getTowns().first());
    gameView->changePlayer(tester);
    window->setCentralWidget(gameView);
    window->addDockWidget(Qt::RightDockWidgetArea, tmpView);
//     scene->addItem(new QGraphicsRectItem(scene->sceneRect()));

    KoushinGUI::ConstructionMenu* menu = new KoushinGUI::ConstructionMenu(buildings);
    menu->close();
    tester->setConstructonMenu(menu);
    QObject::connect(menu, SIGNAL(buildingChosen(QString)), tester, SLOT(buildingChosen(QString)));
//     view->testWidget = menu;
    
    tester->testPlayer();
    
    game->startRound();
    return app.exec();
}
