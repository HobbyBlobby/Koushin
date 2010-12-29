#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KMainWindow>
#include <KStandardDirs>
#include <KConfigGroup>
#include <QGraphicsView>
#include <qgraphicsitem.h>
#include <QObject>
#include <KDebug>
#include <QListWidget>

#include "editor.h"
#include "field.h"
#include <QDockWidget>

int main(int argc, char** argv) {
  KAboutData about("Koushin_TownEditor", 0, ki18n("Koushin Town Editor"), "0.1", ki18n("Town editor for Koushin"), KAboutData::License_GPL, ki18n("(C) 2010 Felix Lemke"), KLocalizedString(), 0, "hobbyblobby@arcor.de");
  about.addAuthor(ki18n("Felix Lemke"), KLocalizedString(), "hobbyblobby@arcor.de");
  KCmdLineArgs::init(argc, argv, &about);
  
  KApplication app;
  KMainWindow* window = new KMainWindow;

  KStandardDirs dirs;
  QString fileName = dirs.findResource("data", "koushin/data/towns/town.config");
  KConfig* config = new KConfig(fileName);
  KConfigGroup imageGroup = config->group("Image");
  
  QString imageName = imageGroup.readEntry("image", QString());
  QString imageFile = dirs.findResource("data", "koushin/data/towns/" + imageName);
  
  QGraphicsView* view = new QGraphicsView;
  QGraphicsScene* scene = new QGraphicsScene;
  QGraphicsPixmapItem* image = new QGraphicsPixmapItem(imageFile);
  scene->addItem(image);
  view->setScene(scene);
  view->fitInView(image->boundingRect());
  
  Koushin_TownEditor::Editor* editor = new Koushin_TownEditor::Editor;

  int fieldsHorizontal = imageGroup.readEntry("fieldsHorizontal", int(-1));
  int fieldsVertical = imageGroup.readEntry("fieldsVertical", int(-1));
  
  int fieldSize = 1;
  if(fieldsHorizontal + fieldsVertical == -2) {
    fieldsHorizontal = 10;
    fieldSize = image->boundingRect().width() / fieldsHorizontal;
    fieldsVertical = image->boundingRect().height() / fieldSize;
  } else if(fieldsHorizontal == -1) {
    fieldSize = image->boundingRect().height() / fieldsVertical;
    fieldsHorizontal = image->boundingRect().width() / fieldSize;
  } else if(fieldsVertical == -1) {
    fieldSize = image->boundingRect().width() / fieldsHorizontal;
    fieldsVertical = image->boundingRect().height() / fieldSize;
  } else {
    fieldSize = qMin(image->boundingRect().width() / fieldsHorizontal, image->boundingRect().height() / fieldsVertical);
  }
  editor->m_fieldSize = fieldSize;
  for(int i = 0; i < fieldsHorizontal; ++i) {
    for(int j = 0; j < fieldsVertical; ++j) {
      Koushin_TownEditor::EditorField* rect = new Koushin_TownEditor::EditorField(i*fieldSize, j*fieldSize, fieldSize, fieldSize);
      rect->m_editor = editor;
      rect->setOpacity(0.2);
      rect->setBrush(QBrush(Qt::cyan));
      scene->addItem(rect);
    }
  }
  QStringList dataDirs = dirs.findDirs("data", "");
  QString homeDir;
  foreach(QString dir, dataDirs)
    if(dir.contains("/home/")) homeDir = dir;
  kDebug() << "home dir " << homeDir;
  KConfig* newConfig = new KConfig(homeDir + "koushin/data/towns/town.config");
  KConfigGroup newImageGroup = newConfig->group("Image");
  foreach(QString key, imageGroup.keyList())
    newImageGroup.writeEntry(key, imageGroup.readEntry(key, QString()));;
  newConfig->sync();
  
  KConfigGroup* fieldGroup = new KConfigGroup(newConfig->group("fields"));
//   KConfigGroup* forstGroup = new KConfigGroup(fieldGroup->group("forestField"));
  editor->m_config = fieldGroup;
  
  window->setCentralWidget(view);

  QStringList fieldTypes = QStringList() << "plainField" << "fieldWithForest" << "fieldWithRocks" << "fieldWithWater" << "fieldNotUsable";
  QListWidget* list = new QListWidget;
  list->addItems(fieldTypes);
  editor->m_fieldTypes = fieldTypes;
  list->setSelectionMode(QAbstractItemView::SingleSelection);
  QObject::connect(list, SIGNAL(itemClicked(QListWidgetItem*)), editor, SLOT(fieldTypeChanged(QListWidgetItem*)));

  QDockWidget* rightWidget = new QDockWidget;
  rightWidget->setWidget(list);
  window->addDockWidget(Qt::RightDockWidgetArea, rightWidget);
  
  window->show();
  return app.exec();
}