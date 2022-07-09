#include "actionmanager.h"

#include <QAction>
#include <QApplication>

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

static const char* kDefaultShortcutPropertyName = "defaultShortcuts";
static const char* kIdPropertyName = "id";
// TODO: add an override for registering third-party actions with a string
// parameter to differentiate them from the default domain and other
// third-party actions created via plugins and python interfaces.
static const std::string kDomainName = "shortcuteditor";

enum class Id {
  Domain,
  Context,
  Category,
  Name
};

static const char kIdDelimiter = '.';

std::vector<QAction*> _actions;
std::unordered_map<std::string, QAction*> _idActionHash;

std::vector<QAction*> ActionManager::registeredActions()
{
  return _actions;
}

void ActionManager::registerAction(QAction* action)
{
  action->setProperty(kDefaultShortcutPropertyName, QVariant::fromValue(action->shortcut()));
  _actions.push_back(action);
  _idActionHash.insert({getId(action), action});
}

// TODO: Remove?
void EncodeString(std::string& text)
{
  text.erase(std::remove_if(text.begin(), text.end(), [](char c) { return std::isspace(c); }), text.end());
}

void ActionManager::registerAction(QAction* action, const std::string& context, const std::string& category)
{
  std::string conciseContext = context;
  // EncodeString(conciseContext);

  std::string conciseCategory = category;
  // EncodeString(conciseContext);

  action->setProperty(kIdPropertyName, QString::fromStdString(kDomainName + "." + conciseContext + "." + conciseCategory) + "." + action->text()/*.simplified().remove(' ')*/);
  registerAction(action);
}

QAction* ActionManager::registerAction(const std::string& name, const std::string& shortcut, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()), qApp);
  action->setShortcut(QKeySequence(QString::fromStdString(shortcut)));
  registerAction(action, context, category);
  return action;
}
