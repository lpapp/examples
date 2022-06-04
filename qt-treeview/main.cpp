#include "hotkeyEditorWidget.h"

#include <QAction>
#include <QApplication>
#include <QPalette>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  HotkeyEditorWidget* hotkeyEditorWidget = new HotkeyEditorWidget; 

  HotkeysMap hotkeys;
  constexpr int maxContexts = 1;
  constexpr int maxCategories = 1;
  constexpr int maxActions = 10;
  for (int contextIndex = 0; contextIndex < maxContexts; ++contextIndex) {
    CategoryHotkeysMap categoryHotkeys;
    QString contextName = "Context" + QString::number(contextIndex);
    for (int categoryIndex = 0; categoryIndex < maxCategories; ++categoryIndex) {
      std::vector<QAction*> _actions;
      QString categoryName = "Category" + QString::number(categoryIndex);
      for (int actionIndex = 0; actionIndex < maxActions; ++actionIndex) {
        QString actionName = "Action" + QString::number(actionIndex);
        QAction* action = new QAction(actionName, hotkeyEditorWidget);
        action->setProperty(kDefaultShortcutPropertyName, QVariant::fromValue(action->shortcut()));
        QStringList stringList{QString::fromStdString(kDomainName), contextName, categoryName, actionName};
        action->setProperty(kIdPropertyName, stringList.join('.'));
        _actions.push_back(action);
      }

      categoryHotkeys.insert({categoryName, _actions});
    }
    hotkeys.insert({contextName, categoryHotkeys});
  }

  hotkeyEditorWidget->setHotkeys(hotkeys);
  hotkeyEditorWidget->show();
  return app.exec();
}
