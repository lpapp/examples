#include "keyboardWidget.h"

#include <QAction>
#include <QApplication>
#include <QGuiApplication>
#include <QColor>
#include <QDrag>
#include <QKeySequence>
#include <QMimeData>
#include <QMouseEvent>
#include <QPalette>
#include <QString>

#include <iostream>
#include <vector>

struct Key {
  int key;
  float width = 1;
  float height = 1;
};

using RowKeys = std::vector<Key>;

namespace {
  float kFunctionKeyWidth = 1.042f;
  float kFunctionKeyHeight = 0.8f;
  float kSpaceWidth = 0.5f;
}

KeyButton::KeyButton(const QString& text, QWidget* parent)
  : QPushButton(text, parent)
{
  setFocusPolicy(Qt::NoFocus);
  setAcceptDrops(true);
}

void KeyButton::mousePressEvent(QMouseEvent* event)
{
  std::cout << "TEST MOUSE PRESS EVENT" << std::endl;
  if (event->button() == Qt::LeftButton) {
    dragStartPosition = event->pos();
  }
  QPushButton::mousePressEvent(event);
}

void KeyButton::mouseMoveEvent(QMouseEvent* event)
{
  std::cout << "TEST MOUSE MOVE EVENT" << std::endl;
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }

  if ((event->pos() - dragStartPosition).manhattanLength()
       < QApplication::startDragDistance()) {
    return;
  }

  QDrag* drag = new QDrag(this);
  QMimeData* mimeData = new QMimeData;
  QKeySequence keyButtonSequence = QKeySequence::fromString(text(), QKeySequence::NativeText);
  Qt::KeyboardModifiers modifiers = static_cast<KeyboardWidget*>(parent())->modifiers();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
  QKeySequence keySequence(modifiers | keyButtonSequence[0]);
#else
  QKeySequence keySequence(modifiers | keyButtonSequence[0].key());
#endif
  mimeData->setText(keySequence.toString(QKeySequence::NativeText));
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
  setDown(false);
}

void KeyButton::dragEnterEvent(QDragEnterEvent* event)
{
  KeyButton* keyButton = qobject_cast<KeyButton*>(event->source());
  if (keyButton) {
    event->ignore();
    return;
  }

  if (!event->mimeData()->hasFormat("text/plain")) {
    event->ignore();
    return;
  }

  std::cout << "TEST DRAG ENTER EVENT: " << text().toStdString() << std::endl;
  setPalette(QApplication::palette().color(QPalette::Highlight));
  event->acceptProposedAction();
}

void KeyButton::dragLeaveEvent(QDragLeaveEvent* /*event*/)
{
  std::cout << "TEST DRAG LEAVE EVENT: " << text().toStdString() << std::endl;
  setPalette(QApplication::palette());
}

void KeyButton::dragMoveEvent(QDragMoveEvent* event)
{
  // std::cout << "TEST DRAG MOVE EVENT: " << text().toStdString() << std::endl;
  if (!event->mimeData()->hasFormat("text/plain")) {
    event->ignore();
    return;
  }

  event->acceptProposedAction();
}

void KeyButton::dropEvent(QDropEvent* event)
{
  if (event->source() == this && event->proposedAction() != Qt::CopyAction) {
    return;
  }

  const QMimeData* mime = event->mimeData();
  if (!mime->hasText()) {
    event->ignore();
    return;
  }

  event->acceptProposedAction();

  QString actionIds = mime->text();
  Q_EMIT actionDropped(actionIds, text());
}

std::vector<RowKeys> keyboardLayout {
  {
    {Qt::Key_Escape, kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F1,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F2,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F3,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F4,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F5,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F6,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F7,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F8,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F9,     kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F10,    kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F11,    kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_F12,    kFunctionKeyWidth, kFunctionKeyHeight},
    {Qt::Key_Eject,  kFunctionKeyWidth, kFunctionKeyHeight},
    {0,              kSpaceWidth,       kFunctionKeyHeight},
    {Qt::Key_F13,    1, kFunctionKeyHeight},
    {Qt::Key_F14,    1, kFunctionKeyHeight},
    {Qt::Key_F15,    1, kFunctionKeyHeight},
    {0,              kSpaceWidth},
    {Qt::Key_F16,    1, kFunctionKeyHeight},
    {Qt::Key_F17,    1, kFunctionKeyHeight},
    {Qt::Key_F18,    1, kFunctionKeyHeight},
    {Qt::Key_F19,    1, kFunctionKeyHeight},
  },

  {
    {Qt::Key_paragraph}, {Qt::Key_1}, {Qt::Key_2}, {Qt::Key_3}, {Qt::Key_4},
    {Qt::Key_5}, {Qt::Key_6}, {Qt::Key_7}, {Qt::Key_8}, {Qt::Key_9},
    {Qt::Key_0}, {Qt::Key_Minus}, {Qt::Key_Equal}, {Qt::Key_Backspace, 1.6f},
    {0, kSpaceWidth}, {Qt::Key_Insert}, {Qt::Key_Home}, {Qt::Key_PageUp},
    {0, kSpaceWidth}, {Qt::Key_Clear}, {Qt::Key_Equal}, {Qt::Key_Slash},
    {Qt::Key_Asterisk}
  },
  
  {
    {Qt::Key_Tab, 1.6f}, {Qt::Key_Q}, {Qt::Key_W}, {Qt::Key_E}, {Qt::Key_R},
    {Qt::Key_T}, {Qt::Key_Y}, {Qt::Key_U}, {Qt::Key_I}, {Qt::Key_O},
    {Qt::Key_P}, {Qt::Key_BracketLeft, 1.05f}, {Qt::Key_BracketRight, 1.05f},
    {Qt::Key_Return, 0.9f, 2.0f}, {0, kSpaceWidth}, {Qt::Key_Delete},
    {Qt::Key_End}, {Qt::Key_PageDown}, {0, kSpaceWidth}, {Qt::Key_7},
    {Qt::Key_8}, {Qt::Key_9}, {Qt::Key_Minus},
  },

  {
    {Qt::Key_CapsLock, 1.7f}, {Qt::Key_A}, {Qt::Key_S}, {Qt::Key_D},
    {Qt::Key_F}, {Qt::Key_G}, {Qt::Key_H}, {Qt::Key_J}, {Qt::Key_K},
    {Qt::Key_L}, {Qt::Key_Semicolon}, {Qt::Key_Apostrophe},
    {Qt::Key_Backslash}, {0, 0.9f}, {0, kSpaceWidth}, {0}, {0}, {0},
    {0, kSpaceWidth}, {Qt::Key_4}, {Qt::Key_5}, {Qt::Key_6}, {Qt::Key_Plus},
  },

  {
    {Qt::ShiftModifier, 1.3f}, {Qt::Key_QuoteLeft}, {Qt::Key_Z}, {Qt::Key_X},
    {Qt::Key_C}, {Qt::Key_V}, {Qt::Key_B}, {Qt::Key_N}, {Qt::Key_M},
    {Qt::Key_Less}, {Qt::Key_Greater}, {Qt::Key_Slash}, {Qt::ShiftModifier, 2.3f},
    {0, kSpaceWidth}, {0}, {Qt::Key_Up}, {0}, {0, kSpaceWidth}, {Qt::Key_1},
    {Qt::Key_2}, {Qt::Key_3}, {Qt::Key_Enter, 1.0f, 2.0f}
  },

  {
#ifdef __APPLE__
    {Qt::MetaModifier, 1.5f}, {Qt::AltModifier, 1.3f}, {Qt::ControlModifier, 1.5f},
#else
    {Qt::ControlModifier, 1.5f}, {Qt::MetaModifier, 1.3f}, {Qt::AltModifier, 1.5f},
#endif
    {Qt::Key_Space, 6.0f},
#ifdef __APPLE__
    {Qt::ControlModifier, 1.5f}, {Qt::AltModifier, 1.3f}, {Qt::MetaModifier, 1.5f},
#else
    {Qt::AltModifier, 1.5f}, {Qt::MetaModifier, 1.3f}, {Qt::ControlModifier, 1.5f},
#endif
    {0, kSpaceWidth}, {Qt::Key_Left}, {Qt::Key_Down}, {Qt::Key_Right},
    {0, kSpaceWidth}, {Qt::Key_0, 2.0f}, {Qt::Key_Period}
  }
};

KeyboardWidget::KeyboardWidget(QWidget* parent)
  : QWidget(parent)
{
  setAcceptDrops(true);
  for (auto& keyboardRow : keyboardLayout) {
    std::vector<KeyButton*> keyboardRowButtons;
    for (auto& key : keyboardRow) {
      if (key.key) {
        QKeySequence keySequence(key.key);
        QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
        if (keySequenceString == "Meta+") {
          keySequenceString.clear();
        }

        for (const auto& modifier : {"Shift", "Ctrl", "Alt"}) {
          QString modifierString = QString(modifier) + "+";
          if (modifierString == keySequenceString) {
            keySequenceString = modifier;
          }
        }

        KeyButton* button = new KeyButton(keySequenceString, this);

        _buttonsMap.insert({key.key, button});
        keyboardRowButtons.push_back(button);

        Qt::KeyboardModifiers allModifiers;
        allModifiers.setFlag(Qt::ShiftModifier, true);
        allModifiers.setFlag(Qt::MetaModifier, true);
        allModifiers.setFlag(Qt::AltModifier, true);
        allModifiers.setFlag(Qt::ControlModifier, true);
        const bool isModifier = allModifiers.testFlag(static_cast<Qt::KeyboardModifier>(key.key));
        if (isModifier) {
          connect(button, &QAbstractButton::clicked, [this, key, button](){
            _modifiers.setFlag(static_cast<Qt::KeyboardModifier>(key.key), !_modifiers.testFlag(static_cast<Qt::KeyboardModifier>(key.key)));
            const bool enabled = _modifiers.testFlag(static_cast<Qt::KeyboardModifier>(key.key));
            button->setPalette(enabled ? QApplication::palette().color(QPalette::Text) : palette());
            highlightShortcuts();
          });
        }
        else {
          connect(button, &KeyButton::actionDropped, [this](const QString& actionId, const QKeySequence& keySequence){
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            Q_EMIT actionDropped(actionId, QKeySequence(_modifiers | keySequence[0]));
#else
            Q_EMIT actionDropped(actionId, QKeySequence(_modifiers | keySequence[0].key()));
#endif
          });
        }
      }
    }
    _buttons.push_back(keyboardRowButtons);
  }

  resizeButtons();
}

void KeyboardWidget::resizeButtons()
{
  static constexpr float kMultiplier = 37.5f;
  // const QSize currentSize = size();
  float row = 0;
  float width = 0;
  for (size_t i = 0; i < keyboardLayout.size(); ++i) {
    float column = 0;
    for (size_t j = 0, buttonColumn = 0; j < keyboardLayout[i].size(); ++j) {
      Key key = keyboardLayout[i][j];
      if (key.key) {
        _buttons[i][buttonColumn++]->setGeometry(kMultiplier * column, kMultiplier * row,
          kMultiplier * key.width, kMultiplier * key.height);
      }
      column += key.width;
    }
    width = std::max(width, column);
    ++row;
  }
  setMinimumSize(kMultiplier * width, kMultiplier * row);
}

void KeyboardWidget::resizeEvent(QResizeEvent* /*event*/)
{
  resizeButtons();
}

void KeyboardWidget::highlightShortcuts()
{
  resetHighlights();

  for (const QAction* action : _actions) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int key = action->shortcut()[0] - static_cast<int>(_modifiers);
#else
    int key = action->shortcut()[0].toCombined() - static_cast<int>(_modifiers);
#endif
    if (_buttonsMap.count(key)) {
      _buttonsMap[key]->setPalette(QApplication::palette().color(QPalette::Text));
      _buttonsMap[key]->setToolTip(action->text());
    }
  }
}

void KeyboardWidget::resetHighlights()
{
  for (const auto& button : _buttonsMap) {
    const bool enabled = _modifiers.testFlag(static_cast<Qt::KeyboardModifier>(button.first));
    if (!enabled) {
      button.second->setPalette(palette());
      button.second->setToolTip(QString());
    }
  }
}

void KeyboardWidget::setActions(const std::vector<QAction*> actions)
{
  _actions = actions;

  highlightShortcuts();
}

Qt::KeyboardModifiers KeyboardWidget::modifiers() const
{
  return _modifiers;
}
