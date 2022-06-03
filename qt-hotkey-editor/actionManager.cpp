#include "actionManager.h"

#include <QAction>

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
