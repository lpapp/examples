#include "actionManager.h"

#include <QAction>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

static const char* kDefaultShortcutsPropertyName = "defaultShortcuts";

std::vector<QAction*> _actions;

std::vector<QAction*> ActionManager::registeredActions()
{
  return _actions;
}

void ActionManager::registerAction(QAction* action)
{
  action->setProperty(kDefaultShortcutPropertyName, QVariant::fromValue(action->shortcut()));
  _actions.push_back(action);
}

void EncodeString(std::string& text)
{
  text.erase(std::remove_if(text.begin(), text.end(), std::isspace), text.end());
}

void ActionManager::registerAction(QAction* action, const std::string& context, const std::string& category)
{
  std::string conciseContext = context;
  EncodeString(conciseContext);

  std::string conciseCategory = category;
  EncodeString(conciseContext);

  action->setProperty(kIdPropertyName, QString::fromStdString(kDomainName + "." + conciseContext + "." + conciseCategory) + "." + action->text().simplified().remove(' '));
  registerAction(action);
}

void ActionManager::registerActions(std::vector<QAction*> actions, const std::string& context, const std::string& category)
{
  for (QAction* action : actions) {
    registerAction(action, context, category);
  }
}

QAction* ActionManager::registerAction(const std::string& name, const std::string& shortcut, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()));
  action->setShortcut(QKeySequence(QString::fromStdString(shortcut)));
  registerAction(action, context, category);
  return action;
}

QAction* ActionManager::registerAction(const std::string& name, int shortcut, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()));
  action->setShortcut(QKeySequence(shortcut));
  registerAction(action, context, category);
  return action;
}

QAction* ActionManager::registerAction(const std::string& name, const std::vector<std::string>& shortcuts, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()));
  QList<QKeySequence> keySequences;
  for (const std::string& shortcut : shortcuts) {
    keySequences.push_back(QKeySequence(QString::fromStdString(shortcut)));
  }
  action->setShortcuts(keySequences);
  registerAction(action, context, category);
  return action;
}

QAction* ActionManager::registerAction(const std::string& name, const std::vector<int>& shortcuts, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()));
  QList<QKeySequence> keySequences;
  for (const int shortcut : shortcuts) {
    keySequences.push_back(QKeySequence(shortcut));
  }
  action->setShortcuts(keySequences);
  registerAction(action, context, category);
  return action;
}

std::string ActionManager::getId(QAction* action)
{
  return action->property(kIdPropertyName).toString().toStdString();
}

QKeySequence ActionManager::getDefaultShortcut(QAction* action)
{
  return action->property(kDefaultShortcutPropertyName).value<QKeySequence>();
}

QList<QKeySequence> ActionManager::getDefaultShortcuts(QAction* action)
{
  return action->property(kDefaultShortcutsPropertyName).value<QList<QKeySequence>>();
}
