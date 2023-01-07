import tkinter as tk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
from cabaret_shallow_water import cabaret_shallow_water


class Application(tk.Frame):
    def __init__(self, master=None, title="<application>", **kwargs):
        super().__init__(master, **kwargs)
        self.master.title(title)
        self.master.columnconfigure(0, weight=1)
        self.master.rowconfigure(0, weight=1)
        self.grid(sticky="NEWS")
        self.create_widgets()
        for column in range(self.grid_size()[0]):
            self.columnconfigure(column, weight=1)
        for row in range(self.grid_size()[1]):
            self.rowconfigure(row, weight=1)

    def create_widgets(self):
        pass


class App(Application):
    def create_widgets(self):
        super().create_widgets()

        self.fig = plt.figure(1, [16, 9])
        self.canvas = FigureCanvasTkAgg(self.fig)
        self.plot_widget = self.canvas.get_tk_widget()
        self.plot_widget.grid(row=0, column=0)

        self.left, self.right = 0, 10
        self.bottom, self.top = 0, 3

        n = 101

        self.x = np.linspace(self.left, self.right, n)
        self.h_x = self.x[2] - self.x[0]
        self.u = np.zeros(n)
        self.h = np.ones(n)

        self.CFL = 0.1

        self.speed = 5

        c = n // 2

        self.h[:c] = 2
        self.h[c:] = 1

        self.u[:c] = 0
        self.u[c:] = 0

        self.u_left = 0
        self.u_right = 0

        self.solve()

    def solve(self):
        i = 0
        while 1:
            self.h, self.u = cabaret_shallow_water(self.h, self.u, self.CFL, self.h_x, self.u_left, self.u_right)
            if not i % self.speed:
                self.draw()
            i += 1

    def draw(self):
        plt.clf()

        plt.plot(self.x, self.h, lw=5)

        plt.xlabel("$x$")
        plt.ylabel("$h$")
        plt.ylim(self.bottom, self.top)
        plt.xlim(self.left, self.right)
        plt.grid()

        self.fig.canvas.draw()

        self.update()


if __name__ == '__main__':
    app = App()
    app.mainloop()
