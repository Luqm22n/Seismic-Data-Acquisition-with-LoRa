import tkinter as tk
import serial
import serial.tools.list_ports
import threading
import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt

class SerialTerminal(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Serial Terminal")
        self.geometry("1920x1080")

        # Daftar port serial
        self.available_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.selected_port = tk.StringVar(self)
        self.selected_port.set(self.available_ports[0] if self.available_ports else "")

        # Baud rate
        self.baud_rates = [9600, 19200, 38400, 57600, 115200]
        self.selected_baud = tk.IntVar(self)
        self.selected_baud.set(115200)

        # Koneksi serial
        self.serial_connection = None
        self.serial_thread = None
        self.running = False

        # Data plotter
        self.data = []
        self.figure = plt.Figure(figsize=(20, 7), dpi=100)
        self.plot = self.figure.add_subplot(111)

        # GUI
        self.create_widgets()

    def create_widgets(self):
        # Port selection
        port_label = tk.Label(self, text="Port Serial:")
        port_label.grid(row=0, column=0, padx=5, pady=5, sticky="w")
        port_dropdown = tk.OptionMenu(self, self.selected_port, *self.available_ports)
        port_dropdown.grid(row=0, column=1, padx=5, pady=5, sticky="w")

        # Baud rate selection
        baud_label = tk.Label(self, text="Baud Rate:")
        baud_label.grid(row=1, column=0, padx=5, pady=5, sticky="w")
        baud_dropdown = tk.OptionMenu(self, self.selected_baud, *self.baud_rates)
        baud_dropdown.grid(row=1, column=1, padx=5, pady=5, sticky="w")

        # Connect/Disconnect button
        self.connect_button = tk.Button(self, text="Connect", command=self.toggle_connection)
        self.connect_button.grid(row=0, column=2, padx=5, pady=5, sticky="w")

        # Text area for display
        self.text_area = tk.Text(self, wrap="word", height=10)
        self.text_area.grid(row=2, column=0, columnspan=3, padx=5, pady=5, sticky="nsew")

        # Input area
        self.input_area = tk.Entry(self)
        self.input_area.grid(row=3, column=0, columnspan=2, padx=5, pady=5, sticky="ew")
        self.input_area.bind("<Return>", self.send_data)

        # Send button
        send_button = tk.Button(self, text="Send", command=self.send_data)
        send_button.grid(row=3, column=2, padx=5, pady=5, sticky="w")

        # Plot area
        self.canvas = FigureCanvasTkAgg(self.figure, master=self)
        self.canvas.get_tk_widget().grid(row=5, column=0, columnspan=3, padx=20, pady=20, sticky="nsew")

    def toggle_connection(self):
        if self.serial_connection is None:
            try:
                self.serial_connection = serial.Serial(self.selected_port.get(), self.selected_baud.get())
                self.text_area.insert(tk.END, f"Connected to {self.selected_port.get()} at {self.selected_baud.get()} baud.\n")
                self.running = True
                self.serial_thread = threading.Thread(target=self.read_serial_data)
                self.serial_thread.start()
                self.connect_button.config(text="Disconnect")
            except serial.SerialException as e:
                self.text_area.insert(tk.END, f"Error: {e}\n")
        else:
            self.running = False
            self.serial_connection.close()
            self.serial_connection = None
            self.text_area.insert(tk.END, "Disconnected.\n")
            self.connect_button.config(text="Connect")

    def send_data(self, event=None):
        data = self.input_area.get()
        if self.serial_connection is not None:
            try:
                self.serial_connection.write(data.encode())
                self.text_area.insert(tk.END, f"Sent: {data}\n")
            except serial.SerialException as e:
                self.text_area.insert(tk.END, f"Error: {e}\n")
        self.input_area.delete(0, tk.END)

    def read_serial_data(self):
        while self.running:
            if self.serial_connection is not None:
                try:
                    data_bytes = self.serial_connection.readline().decode().strip()
                    if data_bytes:
                        self.update_gui(data_bytes)
                except (ValueError, serial.SerialException) as e:
                    self.text_area.insert(tk.END, f"Error: {e}\n")

    def update_gui(self, data_bytes):
        try:
            data_value = float(data_bytes)
            self.data.append(data_value)
            self.plot.cla()
            self.plot.plot(self.data)
            self.canvas.draw()
        except ValueError:
            pass  # Ignore non-numeric data

if __name__ == "__main__":
    app = SerialTerminal()
    app.mainloop()
