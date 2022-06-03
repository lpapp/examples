#include "hotkeyEditorWidget.h"

#include <QAction>
#include <QApplication>
#include <QPalette>

void setApplication(QApplication& application)
{
    QPalette palette(QColor(50, 50, 50));
    palette.setBrush(QPalette::Button, QColor(77, 77, 77));
    palette.setBrush(QPalette::Base, QColor(58, 58, 58));
    palette.setBrush(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setBrush(QPalette::Highlight, QColor(0xf7, 0x93, 0x1e));
    palette.setBrush(QPalette::HighlightedText, QColor(0x1e, 0x1e, 0x1e));

    QColor textColor(200, 200, 200);
    palette.setBrush(QPalette::Text, textColor);
    palette.setBrush(QPalette::WindowText, textColor);
    palette.setBrush(QPalette::BrightText, textColor);
    palette.setBrush(QPalette::ButtonText, textColor);
    palette.setBrush(QPalette::Active, QPalette::Text, textColor);
    palette.setBrush(QPalette::Active, QPalette::WindowText, textColor);
    palette.setBrush(QPalette::Active, QPalette::BrightText, textColor);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, textColor);

    application.setPalette(palette);
    application.setStyle("Fusion");
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  setApplication(app);
  HotkeyEditorWidget* hotkeyEditorWidget = new HotkeyEditorWidget; 

  HotkeysMap hotkeys;
  constexpr int maxContexts = 5;
  constexpr int maxCategories = 3;
  constexpr int maxActions = 1000;
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
