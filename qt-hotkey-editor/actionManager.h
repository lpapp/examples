#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QAction>
#include <QObject>

#include <vector>

class ActionManager : public QObject
{
  Q_OBJECT

public:
  ActionManager(QObject* parent);
  ~ActionManager() override = default;

  void registerAction(QAction* action);
  std::vector<QAction*> registeredActions() const;

private:
  std::vector<QAction*> _actions;
};

#endif
