#include <ctime>

#include <QMap>

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KConfigGroup>
#include <KConfig>
#include <KStandardDirs>

#include "Koushin.h"

#include "actionmanager.h"
#include "actionparser.h"
#include "building.h"
#include "citizen.h"
#include "citizenaction.h"
#include "player.h"
#include <kdebug.h>

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
    
/*//create full person
    Koushin::Citizen man(0);
    Koushin::ActionManager manager;
    kDebug() << ((man.getCondition() && Koushin::citizenIsHungry) ? "Ist hungrig" : "Ist satt");
//make person hungry
    Koushin::CitizenAction action(&man, QString("addCondition"));
    QString condition("citizenIsHungry");
    action.addParameter(condition);
    manager.addAction(&action);
    manager.executeActions();
    kDebug() << ((man.getCondition() && Koushin::citizenIsHungry) ? "Ist hungrig" : "Ist satt");
//make person again full
    Koushin::CitizenAction action2(&man, QString("removeCondition"));
    action2.addParameter(condition);
    manager.addAction(&action2);
    manager.executeActions();
    kDebug() << ((man.getCondition() && Koushin::citizenIsHungry) ? "Ist hungrig" : "Ist satt");*/
    
    Koushin::Player* tester = new Koushin::Player;
    Koushin::ActionManager* manager = new Koushin::ActionManager(tester);
    Koushin::Town* town = new Koushin::Town(tester);
    Koushin::Building* testbldg = new Koushin::Building(town);
    
    kDebug() << "Holz vorher: " << town->getResources().value(Koushin::ResourceWood).amount;
    

    KConfig config(KStandardDirs::locate("data", "koushin/data/buildings/test.cfg"));
    KConfigGroup tasks(&config, "tasks");
    QStringList taskConfigList = tasks.groupList();
    QList<KConfigGroup > taskList;
    for (QStringList::const_iterator it = taskConfigList.begin(); it != taskConfigList.end(); ++it) {
      taskList << KConfigGroup(&tasks, *it);
    }
    kDebug() << taskConfigList;
    foreach(KConfigGroup group, taskList) {
      Koushin::ActionParser parser(testbldg);
      QString recipient = group.readEntry("recipient", QString());
      QString actionName = group.readEntry("action", QString());
      kDebug() << "#######Create Action: " << recipient << " ### und ### " << actionName;
      Koushin::Action* action = parser.parseConfig(group);
      if(action) kDebug() << "######Succes";
      if (action) manager->addAction(action);
    }
    
    manager->executeActions();
    kDebug() << "Holz nachher: " << town->getResources().value(Koushin::ResourceWood).amount;


//     return app.exec();
  return 0;
}
