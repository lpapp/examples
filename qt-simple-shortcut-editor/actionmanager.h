#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <string>
#include <vector>

#include <QKeySequence>
#include <QList>

class QAction;

class ActionManager
{
  ActionManager() = delete;
  ~ActionManager() = delete;

public:
  static std::vector<QAction*> registeredActions();

  static void registerAction(QAction* action);
  static void registerAction(QAction* action, const std::string& context, const std::string& category);
  static QAction* registerAction(const std::string& name, const std::string& shortcut, const std::string& context, const std::string& category);

  static std::string getContext(QAction* action);
  static std::string getCategory(QAction* action);
};

#endif
