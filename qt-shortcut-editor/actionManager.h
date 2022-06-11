#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <string>
#include <vector>

#include <QKeySequence>
#include <QList>

class QAction;

static const char* kDefaultShortcutPropertyName = "defaultShortcut";
static const char* kIdPropertyName = "id";
// TODO: add an override for registering third-party actions with a string
// parameter to differentiate them from foundry and other third-party actions
// created via plugins and python interfaces.
static const std::string kDomainName = "foundry";

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

  static std::string getId(QAction* action);
  static QKeySequence getDefaultShortcut(QAction* action);
  static QList<QKeySequence> getDefaultShortcuts(QAction* action);
};

#endif
