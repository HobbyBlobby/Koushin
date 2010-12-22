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

#include "Koushin.h"

#include "actionmanager.h"
#include "actionparser.h"
#include "building.h"
#include "citizen.h"
#include "citizenaction.h"
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

static const char description[] =
    I18N_NOOP("A round based strategy game.");

static const char version[] = "0.1";

int main(int argc, char** argv)
{
    KAboutData about("Koushin", 0, ki18n("Koushin"), version, ki18n(description), KAboutData::License_GPL, ki18n("(C) 2010 Felix Lemke"), KLocalizedString(), 0, "hobbyblobby@arcor.de");
    about.addAuthor(ki18n("Felix Lemke"), KLocalizedString(), "hobbyblobby@arcor.de");
    KCmdLineArgs::init(argc, argv, &about);

    KApplication app;
    qsrand(std::time(0));
    
    Koushin::Player* tester = new Koushin::Player;
    Koushin::Town* town = new Koushin::Town(tester);
//     Koushin::Building* testbldg = new Koushin::Building(town);
    
//     kDebug() << "Holz vorher: " << town->getResources().value(Koushin::ResourceWood).amount;
    
//     QStringList possibleDirs = KStandardDirs::resourceDirs("data");
//     QStringList fileStrings;
    KStandardDirs stdDirs;
    QMap<QString, QString> buildings;
    foreach(QString dirString, stdDirs.resourceDirs("data")) {
      QDir dir(dirString + "koushin/data/buildings/");
      dir.setNameFilters(QStringList() << "*.config");
      foreach(QString entry, dir.entryList()) {
// 	tester->buildingChosen(dir.path() + "/" + entry);
// 	fileStrings << dir.path() + "/" + entry;
	KConfig config(dir.path() + "/" + entry);
	KConfigGroup general(&config, "general");
	buildings.insert(general.readEntry("name", QString()), dir.path() + "/" + entry);
	
// 	KConfigGroup tasks(&config, "tasks");
// 	QStringList taskConfigList = tasks.groupList();
// 	QList<KConfigGroup > taskList;
// 	for (QStringList::const_iterator it = taskConfigList.begin(); it != taskConfigList.end(); ++it) {
// 	  taskList << KConfigGroup(&tasks, *it);
// 	  kDebug() << *it << " aus Config " << dir.path() + "/" + entry;
// 	  KConfigGroup group(&tasks, *it);
// 	}
// 	kDebug() << taskConfigList;
// 	foreach(KConfigGroup group, taskList) {
// 	  kDebug() << group.keyList();
// 	  Koushin::ActionParser parser(testbldg);
// 	  QString recipient = group.readEntry("recipient", QString());
// 	  QString actionName = group.readEntry("action", QString());
// 	  kDebug() << "#######Create Action: " << recipient << " ### und ### " << actionName;
// 	  Koushin::Action* action = parser.parseConfig(group);
// 	  if(action) kDebug() << "######Succes";
// 	  if (action) manager->addAction(action);
// 	}
      }
    }
    tester->setBuildingList(buildings);
//     kDebug() << "Building files parsed: " << fileStrings;
    
//     manager->executeActions();
//     kDebug() << "Holz nachher: " << town->getResources().value(Koushin::ResourceWood).amount;

    QGraphicsScene* scene = new QGraphicsScene;
    QGraphicsView* view = new QGraphicsView;
    view->setScene(scene);
    scene->addItem(town->getTownWidget());
    town->getTownWidget()->scale(50,50);
    view->resize(scene->sceneRect().size().toSize());
    QObject::connect(town->getTownWidget(), SIGNAL(townClicked(QPoint)), tester, SLOT(townClicked(QPoint)));
    
    QDockWidget* tmpView = new QDockWidget;
    QWidget* tmpViewWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    tmpViewWidget->setLayout(layout);
    KoushinGUI::ResourceInfoWidget* infos = new KoushinGUI::ResourceInfoWidget;
    layout->addWidget(infos);
    tester->setResourceInfoWidget(infos);
    QPushButton* endRoundButton = new QPushButton("End Round");
    layout->addWidget(endRoundButton);
    tmpView->setWidget(tmpViewWidget);
    
    QObject::connect(endRoundButton, SIGNAL(clicked(bool)), tester, SLOT(endRound()));
    
    KMainWindow* window = new KMainWindow;
    window->setCentralWidget(view);
    window->addDockWidget(Qt::RightDockWidgetArea, tmpView);
    scene->addItem(new QGraphicsRectItem(scene->sceneRect()));
    
    window->show();
    
    return app.exec();
}
