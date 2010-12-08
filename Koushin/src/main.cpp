#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

#include <KConfigGroup>
#include <KConfig>
#include <KStandardDirs>

#include "Koushin.h"

#include "citizen.h"
#include "citizenaction.h"
#include "actionmanager.h"
#include "actionparser.h"
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

//create full person
    Koushin::Citizen man(0);
    Koushin::ActionManager manager;
    kDebug() << ((man.getCondition() && Koushin::citizenIsHungry) ? "Ist hungrig" : "Ist satt");
//make person hungry
    Koushin::CitizenAction action(&man, QString("addCondition"));
    Koushin::citizenCondition condition(Koushin::citizenIsHungry);
    action.addParameter((void*)&condition);
    manager.addAction(&action);
    manager.executeActions();
    kDebug() << ((man.getCondition() && Koushin::citizenIsHungry) ? "Ist hungrig" : "Ist satt");
//make person again full
    Koushin::CitizenAction action2(&man, QString("removeCondition"));
    action2.addParameter((void*)&condition);
    manager.addAction(&action2);
    manager.executeActions();
    kDebug() << ((man.getCondition() && Koushin::citizenIsHungry) ? "Ist hungrig" : "Ist satt");
    
    KConfig config(KStandardDirs::locate("data", "koushin/data/buildings/test.cfg"));
    KConfigGroup tasks(&config, "tasks");
    QStringList taskConfigList = tasks.groupList();
    QList<KConfigGroup > taskList;
    for (QStringList::const_iterator it = taskConfigList.begin(); it != taskConfigList.end(); ++it) {
      taskList << KConfigGroup(&tasks, *it);
    }
    Koushin::ActionParser parser;
    foreach(KConfigGroup group, taskList) {
      parser.parseConfig(group);
//       kDebug() << group.readEntry("recipient", QString());
    }
    
//     Koushin::ActionParser parser;
//     parser.addLine("do population makeHealthy");
    
    return app.exec();
}
