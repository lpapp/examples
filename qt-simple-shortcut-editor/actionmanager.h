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
  static void registerActions(std::vector<QAction*> actions, const std::string& context, const std::string& category);

  static QAction* registerAction(const std::string& name, const std::string& shortcut, const std::string& context, const std::string& category);
  static QAction* registerAction(const std::string& name, int shortcut, const std::string& context, const std::string& category);

  static QAction* registerAction(const std::string& name, const std::vector<std::string>& shortcuts, const std::string& context, const std::string& category);

  static QAction* registerAction(const std::string& name, const std::vector<int>& shortcuts, const std::string& context, const std::string& category);

  static QAction* getAction(const std::string& id);
  static std::string getId(QAction* action);
  static std::string getContext(QAction* action);
  static std::string getCategory(QAction* action);
  static QKeySequence getDefaultShortcut(QAction* action);
  static QList<QKeySequence> getDefaultShortcuts(QAction* action);
};

#endif
