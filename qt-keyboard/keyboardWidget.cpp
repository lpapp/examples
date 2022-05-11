#include "keyboardWidget.h"

struct Key {
  int key;
  int width = 1;
  int height = 1;
};

using RowKeys = std::vector<Key>;

std::vector<RowKeys> keyboardLayout {
    { {Qt::Key_1}, {Qt::Key_2}, {Qt::Key_3}, {Qt::Key_4}, {Qt::Key_5}, {Qt::Key_6},
      {Qt::Key_7}, {Qt::Key_8}, {Qt::Key_9}, {Qt::Key_0}, {Qt::Key_Backspace} },

    { {Qt::Key_Tab}, {Qt::Key_Q}, {Qt::Key_W}, {Qt::Key_E}, {Qt::Key_R}, {Qt::Key_T},
      {Qt::Key_Z}, {Qt::Key_U}, {Qt::Key_I}, {Qt::Key_O}, {Qt::Key_P} },

    { {Qt::Key_A}, {Qt::Key_S}, {Qt::Key_D}, {Qt::Key_F}, {Qt::Key_G}, {Qt::Key_H},
      {Qt::Key_J}, {Qt::Key_K}, {Qt::Key_L} },

    { {Qt::Key_Shift}, {Qt::Key_Y}, {Qt::Key_X}, {Qt::Key_C}, {Qt::Key_V}, {Qt::Key_B},
      {Qt::Key_N}, {Qt::Key_M}, {Qt::Key_Enter} },

    { {Qt::Key_Control}, {Qt::Key_Meta}, {Qt::Key_Alt}, {Qt::Key_Space, 6} }
};

KeyboardWidget::KeyboardWidget(QWidget *parent) : QWidget(parent) {
	int row = 0;
	for (auto& keyboardRow : keyboardLayout) {
		int column = 0;
		for (auto& key : keyboardRow) {
			QKeySequence keySequence(key.key);
			QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
			QPushButton *button = new QPushButton(keySequenceString, this);
			button->setGeometry(48 * column, 48 * row, 48 * key.width, 48 * key.height);
			++column;
		}
		++row;
	}
}
