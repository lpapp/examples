#include "actionManager.h"

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
static const std::string kDomainName = "lpapp";

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

void ActionManager::registerActions(std::vector<QAction*> actions, const std::string& context, const std::string& category)
{
  for (QAction* action : actions) {
    registerAction(action, context, category);
  }
}

QAction* ActionManager::registerAction(const std::string& name, const std::string& shortcut, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()), qApp);
  action->setShortcut(QKeySequence(QString::fromStdString(shortcut)));
  registerAction(action, context, category);
  return action;
}

QAction* ActionManager::registerAction(const std::string& name, int shortcut, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()), qApp);
  action->setShortcut(QKeySequence(shortcut));
  registerAction(action, context, category);
  return action;
}

QAction* ActionManager::registerAction(const std::string& name, const std::vector<std::string>& shortcuts, const std::string& context, const std::string& category)
{
  QAction* action = new QAction(QObject::tr(name.c_str()), qApp);
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
  QAction* action = new QAction(QObject::tr(name.c_str()), qApp);
  QList<QKeySequence> keySequences;
  for (const int shortcut : shortcuts) {
    keySequences.push_back(QKeySequence(shortcut));
  }
  action->setShortcuts(keySequences);
  registerAction(action, context, category);
  return action;
}

QAction* ActionManager::getAction(const std::string& id)
{
  return _idActionHash[id];
}

std::string ActionManager::getId(QAction* action)
{
  return action->property(kIdPropertyName).toString().toStdString();
}

std::string ActionManager::getContext(QAction* action)
{
  // std::cout << "TEST GET CONTEXT ID 1: " << reinterpret_cast<void*>(action) << std::endl;
  // std::cout << "TEST GET CONTEXT ID 2: " << action->property(kIdPropertyName).toString().toStdString() << std::endl;
  QVariant idVariant = action->property(kIdPropertyName);
  if (!idVariant.isValid() || idVariant.isNull() || !idVariant.canConvert<QString>()) {
    return std::string();
  }

  const QStringList sections = idVariant.toString().split(kIdDelimiter);
  const size_t index = static_cast<int>(Id::Context);
  return ((static_cast<size_t>(sections.size()) <= index) ? std::string() : sections[index].toStdString());
}

std::string ActionManager::getCategory(QAction* action)
{
  return action->property(kIdPropertyName).toString().split(kIdDelimiter)[static_cast<int>(Id::Category)].toStdString();
}

QKeySequence ActionManager::getDefaultShortcut(QAction* action)
{
  return action->property(kDefaultShortcutPropertyName).value<QKeySequence>();
}

QList<QKeySequence> ActionManager::getDefaultShortcuts(QAction* action)
{
  return action->property(kDefaultShortcutPropertyName).value<QList<QKeySequence>>();
}
