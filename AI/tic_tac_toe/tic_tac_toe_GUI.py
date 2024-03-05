import sys
from PyQt5.QtWidgets import QWidget, QPushButton, QApplication, QGridLayout, QSpinBox, QSizePolicy, QLabel, QMessageBox, QInputDialog
from tic_tac_toe import minmax_step, is_game_ended, whoiswinner
import numpy as np


class Example(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        grid = QGridLayout()
        self.setLayout(grid)

        self.state = np.zeros((3, 3), dtype=int)

        self.sign = 'X'

        self.btns = []
        for i, j in [(i, j) for i in range(3) for j in range(3)]:
            btn = QPushButton('', self)
            btn.clicked.connect(lambda checked, btn=btn: self.clicked(btn))
            grid.addWidget(btn, i, j)
            btn.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
            self.btns.append(btn)

        label_spin_box = QLabel('Кол-во шагов, просматриваемых наперед противником', self)
        grid.addWidget(label_spin_box, 3, 0)
        
        self.spin_box = QSpinBox()
        self.spin_box.setMinimum(1)
        self.spin_box.setMaximum(9)
        self.spin_box.setValue(1)
        grid.addWidget(self.spin_box, 3, 1)

        label_spin_box2 = QLabel('Кол-во шагов, просматриваемых наперед минмакс ходом', self)
        grid.addWidget(label_spin_box2, 4, 0)
        
        self.spin_box2 = QSpinBox()
        self.spin_box2.setMinimum(1)
        self.spin_box2.setMaximum(5)
        self.spin_box2.setValue(2)
        grid.addWidget(self.spin_box2, 4, 1)

        self.smart_step = QPushButton('Сделать минмакс ход', self)
        self.smart_step.clicked.connect(self.perform_full_step)
        grid.addWidget(self.smart_step, 3, 2, 2, 1)
        self.smart_step.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)

        self.msg_box = QMessageBox()
        self.msg_box.setWindowTitle("Игра окончена")
        self.msg_box.setText("Результат неизвестен")
        self.msg_box.setStandardButtons(QMessageBox.Ok)

        ok_button = self.msg_box.button(QMessageBox.Ok)
        ok_button.clicked.connect(self.new_game)

        self.setWindowTitle('tic tac toe')
        self.show()
        self.start_actions()

    def update_state(self):
        for idx, btn in enumerate(self.btns):
            i, j = idx // 3, idx % 3
            if btn.text() == '':
                self.state[i, j] = 0
            elif btn.text() == 'X':
                self.state[i, j] = 1
            elif btn.text() == 'O':
                self.state[i, j] = 10
    
    def set_state(self):
        for idx, btn in enumerate(self.btns):
            i, j = idx // 3, idx % 3
            if self.state[i, j] == 0:
                btn.setText('')
            elif self.state[i, j] == 1:
                btn.setText('X')
            elif self.state[i, j] == 10:
                btn.setText('O')

    def choose_figure(self):
        figure, ok_pressed = QInputDialog.getItem(self, "Выберите фигуру", "Выберите фигуру:", ["X", "O"], 0, False)
        self.sign = figure

    def start_actions(self):
        self.choose_figure()
        if self.sign == 'O':
            self.perform_opponent_step()
    
    def new_game(self):
        self.state = np.zeros((3, 3), dtype=int)
        self.set_state()
        self.start_actions()
    
    def clicked(self, btn):
        if btn.text() == '':
            btn.setText(self.sign)
            self.update_state()
            if self.check_if_ended():
                return
            if self.perform_opponent_step():
                return
    
    def check_if_ended(self):
        res = is_game_ended(self.state)
        if is_game_ended(self.state):
            winner_X, winner_O = whoiswinner(self.state)
            if winner_X:
                self.msg_box.setText("Победил X")
            elif winner_O:
                self.msg_box.setText("Победил O")
            else:
                self.msg_box.setText("Ничья")
            self.msg_box.exec()
        return res
    
    def perform_full_step(self):
        if self.perform_my_step():
            return
        if self.perform_opponent_step():
            return
    
    def perform_my_step(self):
        self.state = minmax_step(self.state, self.spin_box2.value())
        self.set_state()
        return self.check_if_ended()
    
    def perform_opponent_step(self):
        self.state = minmax_step(self.state, self.spin_box.value())
        self.set_state()
        return self.check_if_ended()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = Example()
    sys.exit(app.exec_())
