#include "keyboardWidget.h"

#include <QtWidgets/QPushButton>

#include <QtGui/QColor>
#include <QtGui/QKeySequence>
#include <QtGui/QPalette>
#include <QtGui/QResizeEvent>

#include <QtCore/QString>

#include <vector>

struct Key {
  int key;
  float width = 1;
  float height = 1;
};

using RowKeys = std::vector<Key>;

namespace {
  constexpr float kFunctionKeyWidth = 1.042f;
  constexpr float kFunctionKeyHeight = 0.8f;
  constexpr float kSpaceWidth = 0.5f;
  constexpr float kMultiplier = 37.5f;
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
    {Qt::Key_Shift, 1.3f}, {Qt::Key_QuoteLeft}, {Qt::Key_Z}, {Qt::Key_X},
    {Qt::Key_C}, {Qt::Key_V}, {Qt::Key_B}, {Qt::Key_N}, {Qt::Key_M},
    {Qt::Key_Less}, {Qt::Key_Greater}, {Qt::Key_Slash}, {Qt::Key_Shift, 2.3f},
    {0, kSpaceWidth}, {0}, {Qt::Key_Up}, {0}, {0, kSpaceWidth}, {Qt::Key_1},
    {Qt::Key_2}, {Qt::Key_3}, {Qt::Key_Enter, 1.0f, 2.0f}
  },

  {
    {Qt::Key_Meta, 1.5f}, {Qt::Key_Alt, 1.3f}, {Qt::Key_Control, 1.5f},
    {Qt::Key_Space, 6.0f}, {Qt::Key_Control, 1.5f}, {Qt::Key_Alt, 1.3f},
    {Qt::Key_Meta, 1.5f}, {0, kSpaceWidth}, {Qt::Key_Left}, {Qt::Key_Down},
    {Qt::Key_Right}, {0, kSpaceWidth}, {Qt::Key_0, 2.0f}, {Qt::Key_Period}
  }
};

KeyboardWidget::KeyboardWidget(QWidget *parent) : QWidget(parent) {
  float row = 0;
  float width = 0;
  for (auto& keyboardRow : keyboardLayout) {
    float column = 0;
    std::vector<QPushButton*> keyboardRowButtons;
    for (auto& key : keyboardRow) {
      if (key.key) {
        QKeySequence keySequence(key.key);
        QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
        QPushButton *button = new QPushButton(keySequenceString, this);
        button->setFocusPolicy(Qt::NoFocus);
        button->setGeometry(kMultiplier * column, kMultiplier * row, 
                            kMultiplier * key.width, kMultiplier * key.height);
        _buttonsMap.insert({keySequenceString, button});
        keyboardRowButtons.push_back(button);
      }
      column += key.width;
    }
    width = std::max(width, column);
    ++row;
    _buttons.push_back(keyboardRowButtons);
  }
  // setFixedSize(kMultiplier * width, kMultiplier * row);
}

void KeyboardWidget::setHotkeys(const std::vector<QKeySequence>& hotkeys, const QColor& color)
{
  for (const auto& button : _buttonsMap) {
    button.second->setPalette(palette());
  }

  for (const QKeySequence& keySequence : hotkeys) {
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
    if (_buttonsMap.count(keySequenceString)) {
      _buttonsMap[keySequenceString]->setPalette(color);
    }
  }
}

#include <iostream>

void KeyboardWidget::resizeEvent(QResizeEvent *event)
{
  std::cout << "TEST RESIZING THE KEYBOARD, WIDTH: " << event->size().width() << ", HEIGHT: " <<  event->size().height() << std::endl;
  for (size_t row = 0; row < _buttons.size(); ++row) {
    for (size_t column = 0; column < _buttons[row].size(); ++column) {
      QPushButton* button = _buttons[row][column];
      Key key = keyboardLayout[row][column];
      button->setGeometry(button->geometry().x() / 5, button->geometry().y() / 5, key.width * event->size().width()/5, key.height * event->size().height() / 5);
    }
  }
}
