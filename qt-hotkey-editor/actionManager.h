#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <string>
#include <vector>

static const char* kDefaultShortcutPropertyName = "defaultShortcut";
static const char* kIdPropertyName = "id";
static const std::string kDomainName = "lpapp";

class QAction;

class ActionManager
{
  ActionManager() = delete;
  ~ActionManager() = delete;

public:

  static void registerAction(QAction* action);
  static std::vector<QAction*> registeredActions();
};

#endif
