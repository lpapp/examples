#include "actionManager.h"

ActionManager::ActionManager(QObject* parent)
  : QObject(parent)
{
}

std::vector<QAction*> ActionManager::registeredActions() const
{
  return _actions;
}

void ActionManager::registerAction(QAction* action)
{
  _actions.push_back(action);
}
