import serial
import struct
import threading
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
import customtkinter as ctk
from collections import deque
from tkinter import simpledialog
import os
import serial.tools.list_ports

# Configure appearance
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

class BluetoothMonitor:
    def __init__(self, root):
        self.root = root
        self.root.title("Bluetooth Data Monitor")
        self.root.geometry("1200x800")
        self.root.minsize(1000, 650)
        
        # Handle window closing properly
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        
        # Performance optimization flags
        self.running = True
        self.last_plot_time = 0
        self.plot_refresh_rate = 10  # Hz
        self.redundant_frames = 1  # Number of redundant frames for smoothing
        
        # Initialize variables
        self.initialize_variables()
        
        # Create GUI
        self.setup_gui()
        
        # Load saved signals
        self.load_signals()
        
        # Start plot updates
        self.schedule_plot_update()

    def on_closing(self):
        """Handle window closing event"""
        self.running = False
        
        # Close Bluetooth connection if open
        if hasattr(self, 'bluetooth') and self.bluetooth and self.bluetooth.is_open:
            self.bluetooth.close()
        
        # Destroy the root window
        self.root.quit()
        self.root.destroy()

    def initialize_variables(self):
        """Initialize all class variables"""
        self.data_buffer = []
        self.raw_data_buffer = []  # For storing raw data before smoothing
        self.signals = {}
        self.plotting_enabled = True
        self.update_interval = 0.01
        self.x_axis_range = 10
        self.num_signals = 0
        self.start_time = time.time()
        self.vertical_line = None
        self.navigation_toolbar = None
        self.toolbar_frame = None
        self.paused_time = None
        self.navigation_enabled = False
        self.bluetooth = None
        self.display_mode = "graph"
        self.value_boxes = []
        self.value_labels = []
        self.text_display_frame = None
        self.values_frame = None
        self.SIGNALS_FILE = "signals.txt"
        
        # Colors
        self.bg_color = "#2b2b2b"
        self.frame_color = "#3b3b3b"
        self.button_color = "#1f6aa5"
        self.hover_color = "#144870"
        self.text_color = "white"
        self.accent_color = "#3a7ebf"
        self.graph_bg = "#1e1e1e"
        self.graph_text = "white"
        self.graph_grid = "#3a3a3a"

    def setup_gui(self):
        """Setup all GUI components"""
        # Configure grid layout
        self.root.grid_rowconfigure(1, weight=1)
        self.root.grid_columnconfigure(0, weight=1)
        
        # Create connection frame
        self.create_connection_frame()
        
        # Create main content frame
        self.main_frame = ctk.CTkFrame(self.root, fg_color=self.bg_color)
        self.main_frame.grid(row=1, column=0, sticky="nsew", padx=10, pady=(0,10))
        self.main_frame.grid_rowconfigure(0, weight=1)
        self.main_frame.grid_columnconfigure(0, weight=1)
        self.main_frame.grid_columnconfigure(1, weight=0)
        self.main_frame.grid_columnconfigure(2, weight=0)  # For values frame
        
        # Create graph frame
        self.create_graph_frame()
        
        # Create control panel
        self.create_control_panel()
        
        # Create values frame
        self.create_values_frame()

    def create_connection_frame(self):
        """Create the connection controls frame"""
        connection_frame = ctk.CTkFrame(self.root, height=50, fg_color=self.frame_color)
        connection_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=10)
        
        # Port selection
        port_label = ctk.CTkLabel(connection_frame, text="COM Port:", text_color=self.text_color)
        port_label.grid(row=0, column=0, padx=(10,5), pady=5)
        
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_var = ctk.StringVar(value=ports[0] if ports else "COM3")
        port_menu = ctk.CTkOptionMenu(connection_frame, variable=self.port_var, 
                                    values=ports, fg_color=self.button_color,
                                    button_color=self.button_color,
                                    button_hover_color=self.hover_color)
        port_menu.grid(row=0, column=1, padx=5, pady=5)
        
        # Refresh button
        refresh_btn = ctk.CTkButton(connection_frame, text="↻", width=30,
                                  command=self.refresh_ports,
                                  fg_color=self.button_color,
                                  hover_color=self.hover_color)
        refresh_btn.grid(row=0, column=2, padx=5, pady=5)
        
        # Connect button
        self.connect_btn = ctk.CTkButton(connection_frame, text="Connect", width=100,
                                       command=self.toggle_bluetooth_connection,
                                       fg_color=self.button_color,
                                       hover_color=self.hover_color)
        self.connect_btn.grid(row=0, column=3, padx=5, pady=5)
        
        # Status label
        self.status_var = ctk.StringVar(value="Disconnected")
        self.status_label = ctk.CTkLabel(connection_frame, textvariable=self.status_var, 
                                       text_color="red")
        self.status_label.grid(row=0, column=4, padx=10, pady=5)
        
        # Display mode toggle
        self.display_mode_var = ctk.IntVar(value=1)
        
        graph_radio = ctk.CTkRadioButton(connection_frame, text="Graph", 
                                       variable=self.display_mode_var, 
                                       value=1, command=self.toggle_display_mode,
                                       fg_color=self.accent_color,
                                       hover_color=self.hover_color)
        graph_radio.grid(row=0, column=5, padx=(20,5), pady=5)
        
        text_radio = ctk.CTkRadioButton(connection_frame, text="Text", 
                                      variable=self.display_mode_var, 
                                      value=0, command=self.toggle_display_mode,
                                      fg_color=self.accent_color,
                                      hover_color=self.hover_color)
        text_radio.grid(row=0, column=6, padx=5, pady=5)

    def create_graph_frame(self):
        """Create the graph display frame"""
        self.graph_frame = ctk.CTkFrame(self.main_frame, fg_color=self.bg_color)
        self.graph_frame.grid(row=0, column=0, sticky="nsew", padx=(10,0), pady=10)
        self.graph_frame.grid_rowconfigure(0, weight=1)
        self.graph_frame.grid_columnconfigure(0, weight=1)
        
        # Create a container frame specifically for the canvas and toolbar
        self.plot_container = ctk.CTkFrame(self.graph_frame, fg_color=self.bg_color)
        self.plot_container.pack(fill="both", expand=True)
        
        # Create matplotlib figure with dark theme
        plt.style.use('dark_background')
        self.fig, self.ax = plt.subplots(figsize=(8, 5), tight_layout=True)
        self.fig.patch.set_facecolor(self.graph_bg)
        self.ax.set_facecolor(self.graph_bg)
        self.ax.tick_params(axis='both', colors=self.graph_text)
        self.ax.xaxis.label.set_color(self.graph_text)
        self.ax.yaxis.label.set_color(self.graph_text)
        self.ax.title.set_color(self.graph_text)
        self.ax.grid(True, color=self.graph_grid, linestyle='--', alpha=0.5)
        
        # Embed the matplotlib figure
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.plot_container)
        self.canvas.get_tk_widget().pack(side="top", fill="both", expand=True)
        self.canvas.mpl_connect("motion_notify_event", self.move_vertical_line)

    def create_values_frame(self):
        """Create frame to display values at paused time"""
        self.values_frame = ctk.CTkFrame(self.main_frame, width=200, fg_color=self.frame_color)
        self.values_frame.grid(row=0, column=2, sticky="ns", padx=(0,10), pady=10)
        self.values_frame.grid_propagate(False)
        self.values_frame.grid_remove()  # Start hidden
        
        # Title
        title = ctk.CTkLabel(self.values_frame, text="Paused Values", 
                            font=("Arial", 14, "bold"), text_color=self.text_color)
        title.pack(pady=(10, 5))
        
        # Separator
        separator = ctk.CTkFrame(self.values_frame, height=2, fg_color="#555555")
        separator.pack(fill="x", padx=10, pady=5)
        
        # Container for values
        self.values_container = ctk.CTkScrollableFrame(self.values_frame, fg_color="transparent")
        self.values_container.pack(fill="both", expand=True, padx=5, pady=5)
        
        # Time display
        self.paused_time_label = ctk.CTkLabel(self.values_frame, text="Time: 0.00s", 
                                            font=("Arial", 15), text_color=self.accent_color)
        self.paused_time_label.pack(side="bottom", pady=5)

    def create_control_panel(self):
        """Create the right-side control panel"""
        control_panel = ctk.CTkFrame(self.main_frame, width=280, fg_color=self.frame_color)
        control_panel.grid(row=0, column=1, sticky="ns", padx=(0,10), pady=10)
        control_panel.grid_propagate(False)
        
        # Title
        title = ctk.CTkLabel(control_panel, text="Control Panel", 
                            font=("Arial", 14, "bold"), text_color=self.text_color)
        title.pack(pady=(10, 5))
        
        # Separator
        separator = ctk.CTkFrame(control_panel, height=2, fg_color="#555555")
        separator.pack(fill="x", padx=10, pady=5)
        
        # Signal controls frame
        signal_controls = ctk.CTkFrame(control_panel, fg_color="transparent")
        signal_controls.pack(fill="x", padx=10, pady=5)
        
        # Manage signals button
        manage_btn = ctk.CTkButton(signal_controls, text="Manage Signals", 
                                  command=self.list_signals,
                                  fg_color=self.button_color,
                                  hover_color=self.hover_color)
        manage_btn.pack(fill="x", pady=5)
        
        # Active signals label
        active_label = ctk.CTkLabel(signal_controls, text="Active Signals:", 
                                  text_color=self.accent_color)
        active_label.pack(anchor="w", pady=(10,5))
        
        # Signals frame with scrollbar
        self.signals_frame = ctk.CTkScrollableFrame(signal_controls, height=200,
                                                   fg_color="transparent")
        self.signals_frame.pack(fill="both", expand=True, pady=5)
        
        # Separator
        separator = ctk.CTkFrame(control_panel, height=2, fg_color="#555555")
        separator.pack(fill="x", padx=10, pady=5)
        
        # Time step controls
        time_frame = ctk.CTkFrame(control_panel, fg_color="transparent")
        time_frame.pack(fill="x", padx=10, pady=5)
        
        time_label = ctk.CTkLabel(time_frame, text="Time Step (s):", text_color=self.text_color)
        time_label.pack(side="left", padx=5)
        
        self.time_step_entry = ctk.CTkEntry(time_frame, width=80)
        self.time_step_entry.insert(0, str(self.update_interval))
        self.time_step_entry.pack(side="left", padx=5)
        
        set_btn = ctk.CTkButton(time_frame, text="Set", width=50,
                              command=self.set_time_step,
                              fg_color=self.button_color,
                              hover_color=self.hover_color)
        set_btn.pack(side="left", padx=5)
        
        # Separator
        separator = ctk.CTkFrame(control_panel, height=2, fg_color="#555555")
        separator.pack(fill="x", padx=10, pady=5)
        
        # Navigation controls
        nav_frame = ctk.CTkFrame(control_panel, fg_color="transparent")
        nav_frame.pack(fill="x", padx=10, pady=5)
        
        self.navigation_button = ctk.CTkButton(nav_frame, text="Enable Navigation", 
                                             command=self.toggle_navigation,
                                             fg_color=self.button_color,
                                             hover_color=self.hover_color)
        self.navigation_button.pack(fill="x", pady=5)

    def create_text_display(self):
        """Create the text display frame"""
        if self.text_display_frame is None or not self.text_display_frame.winfo_exists():
            self.text_display_frame = ctk.CTkFrame(self.main_frame, fg_color=self.bg_color)
            self.text_display_frame.grid(row=0, column=0, sticky="nsew", padx=(10,0), pady=10)
            
            # Create scrollable container
            scroll_container = ctk.CTkScrollableFrame(self.text_display_frame, 
                                                     fg_color=self.bg_color)
            scroll_container.pack(fill="both", expand=True, padx=10, pady=10)
            
            # Title
            title = ctk.CTkLabel(scroll_container, text="Signal Values", 
                                font=("Arial", 18, "bold"), text_color=self.text_color)
            title.pack(pady=(0, 10))
            
            # Create value displays
            self.value_boxes = []
            for i, signal_name in enumerate(self.signals):
                frame = ctk.CTkFrame(scroll_container, fg_color=self.frame_color,
                                   border_width=1, border_color="#555555")
                frame.pack(fill="x", padx=5, pady=2)
                
                label = ctk.CTkLabel(frame, text=self.signals[signal_name].cget("text"), 
                                    width=180, anchor="w", text_color=self.text_color, font=("Arial", 15))
                label.pack(side="left", padx=10, pady=5)
                
                value_box = ctk.CTkLabel(frame, text="0.00", width=80, 
                                       fg_color="#1f1f1f", corner_radius=3,
                                       text_color="white", font=("Arial", 15))
                value_box.pack(side="right", padx=10, pady=5)
                
                self.value_boxes.append(value_box)

    def refresh_ports(self):
        """Refresh available COM ports"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_var.set(ports[0] if ports else "COM3")
        return ports

    def toggle_bluetooth_connection(self):
        """Toggle Bluetooth connection"""
        if self.bluetooth and self.bluetooth.is_open:
            self.bluetooth.close()
            self.status_var.set("Disconnected")
            self.status_label.configure(text_color="red")
            self.connect_btn.configure(text="Connect")
            print(f"Disconnected from {self.port_var.get()}")
        else:
            self.bluetooth = self.connect_bluetooth(self.port_var.get())
            if self.bluetooth and self.bluetooth.is_open:
                self.status_var.set("Connected")
                self.status_label.configure(text_color="green")
                self.connect_btn.configure(text="Disconnect")
                bluetooth_thread = threading.Thread(target=self.read_bluetooth_data, 
                                                  args=(self.bluetooth,), daemon=True)
                bluetooth_thread.start()
            else:
                self.status_var.set("Connection Failed")

    def connect_bluetooth(self, port_name, baud_rate=9600, timeout=5):
        """Establish Bluetooth serial connection"""
        try:
            ser = serial.Serial(port_name, baud_rate, timeout=timeout)
            print(f"Connected to {port_name} at {baud_rate} baud")
            return ser
        except serial.SerialException as e:
            print(f"Error connecting to {port_name}: {e}")
            return None

    def read_bluetooth_data(self, bluetooth):
        """Read data from Bluetooth connection with redundant frames for smoothing"""
        sync_bytes = b'\xF0\xF0\xF0\xF0'
        frame_size = 4 * self.num_signals + 4  # 4 bytes per float + 4 bytes CRC

        while self.running and bluetooth and bluetooth.is_open:
            try:
                # Buffer for storing multiple frames for smoothing
                frames_buffer = []
                
                # Read multiple frames for redundancy
                for _ in range(self.redundant_frames):
                    # Wait for sync bytes
                    sync_received = False
                    while not sync_received and self.running:
                        if bluetooth.in_waiting >= 4:
                            received = bluetooth.read(4)
                            if received == sync_bytes:
                                sync_received = True
                            else:
                                continue

                    # Wait for full frame
                    while bluetooth.in_waiting < frame_size and self.running:
                        time.sleep(0.0001)

                    # Read frame
                    raw_data = bluetooth.read(frame_size)
                    frames_buffer.append(raw_data)

                # Process all received frames
                valid_values = []
                for raw_data in frames_buffer:
                    # Extract data and CRC
                    data = raw_data[:4 * self.num_signals]
                    received_crc = raw_data[-4:]

                    # Calculate CRC
                    temp_data = bytearray()
                    for i in range(self.num_signals):
                        start = i * 4
                        end = start + 4
                        temp_data.extend(data[start:end][::-1])  # Reverse bytes within each signal

                    calculated_crc = 0xFFFFFFFF
                    for byte in temp_data:
                        calculated_crc ^= (byte << 24)
                        for _ in range(8):
                            if calculated_crc & 0x80000000:
                                calculated_crc = ((calculated_crc << 1) ^ 0x04C11DB7) & 0xFFFFFFFF
                            else:
                                calculated_crc = (calculated_crc << 1) & 0xFFFFFFFF
                    calculated_crc ^= 0x00000000

                    if calculated_crc == struct.unpack('<I', received_crc)[0]:
                        values = struct.unpack('f' * self.num_signals, data)
                        valid_values.append(values)

                if valid_values:
                    # Average the valid values for smoothing
                    avg_values = np.mean(valid_values, axis=0)
                    timestamp = time.time() - self.start_time
                    
                    # Update text display
                    if self.display_mode == "text":
                        for i, value in enumerate(avg_values):
                            if i < len(self.value_boxes):
                                self.value_boxes[i].configure(text=f"{value:.5f}")

                    # Update graph data if not paused
                    if self.plotting_enabled and self.display_mode == "graph":
                        for i, value in enumerate(avg_values):
                            if f"Signal {i + 1}" in self.signals and self.signals[f"Signal {i + 1}"].get():
                                self.data_buffer[i].append((timestamp, value))
                    else:
                        print("CRC Error")
            except (serial.SerialException, OSError) as e:
                if self.running:
                    print(f"Serial error: {e}")
                break
            except Exception as e:
                if self.running:
                    print(f"Unexpected error: {e}")
                break

    def schedule_plot_update(self):
        """Schedule the next plot update"""
        if not self.running:
            return
            
        current_time = time.time()
        if current_time - self.last_plot_time >= 1/self.plot_refresh_rate:
            self.update_plot()
            self.last_plot_time = current_time
        
        self.root.after(10, self.schedule_plot_update)

    def update_plot(self):
        """Update the plot with new data"""
        if not self.running or self.display_mode != "graph" or self.num_signals == 0 or self.navigation_enabled:
            return

        try:
            self.ax.clear()
            has_visible_signals = False
            
            # Get current time for x-axis limits
            current_time = time.time() - self.start_time
            x_min = current_time - self.x_axis_range
            x_max = current_time
            
            # Initialize variables for y-axis limits
            y_min = float('inf')
            y_max = -float('inf')
            
            # Plot each signal and find min/max values
            for i, signal_name in enumerate(self.signals):
                if self.signals[signal_name].get() and self.data_buffer[i]:
                    timestamps, values = zip(*self.data_buffer[i])
                    timestamps = np.array(timestamps)
                    values = np.array(values)
                    
                    # Only consider data within current x-axis range
                    mask = (timestamps >= x_min) & (timestamps <= x_max)
                    visible_timestamps = timestamps[mask]
                    visible_values = values[mask]
                    
                    if len(visible_values) > 0:
                        self.ax.plot(visible_timestamps, visible_values, 
                                label=self.signals[signal_name].cget("text"))
                        has_visible_signals = True
                        
                        # Update y-axis limits
                        current_min = np.min(visible_values)
                        current_max = np.max(visible_values)
                        y_min = min(y_min, current_min)
                        y_max = max(y_max, current_max)

            # Configure plot appearance
            self.ax.set_xlabel("Time (s)", color=self.graph_text)
            self.ax.set_ylabel("Value", color=self.graph_text)
            self.ax.set_title("Real-Time Signal Data", color=self.graph_text)
            self.ax.grid(True, color=self.graph_grid, linestyle='--', alpha=0.5)
            self.ax.set_facecolor(self.graph_bg)
            
            # Set x-axis limits
            self.ax.set_xlim(x_min, x_max)
            
            # Set y-axis limits with 10% padding if we have visible data
            if has_visible_signals and y_min != float('inf') and y_max != -float('inf'):
                padding = 0.1 * (y_max - y_min) if y_max != y_min else 0.1 * abs(y_max) if y_max != 0 else 1.0
                self.ax.set_ylim(y_min - padding, y_max + padding)
            
            # Only create legend if there are visible signals
            if has_visible_signals:
                self.ax.legend(loc="upper left", facecolor=self.graph_bg, labelcolor=self.graph_text)

            # Redraw vertical line if it exists
            if self.vertical_line:
                self.vertical_line = self.ax.axvline(x=self.vertical_line.get_xdata()[0], 
                                                color='red', linestyle='--')
            
            # Update status if paused
            if self.paused_time is not None:
                self.ax.set_xlabel("Time (s) [Paused]", color=self.graph_text)

            self.canvas.draw()
        except Exception as e:
            print(f"Error updating plot: {e}")

    def toggle_display_mode(self):
        """Switch between graph and text display modes"""
        if self.display_mode_var.get() == 1:
            self.display_mode = "graph"
            if self.text_display_frame:
                self.text_display_frame.grid_remove()
            self.graph_frame.grid()
            self.update_plot()
        else:
            self.display_mode = "text"
            self.graph_frame.grid_remove()
            self.create_text_display()
            self.text_display_frame.grid()

    def toggle_navigation(self):
        """Toggle navigation mode"""
        if self.navigation_enabled:
            self.disable_navigation()
        else:
            self.enable_navigation()

    def enable_navigation(self):
        """Enable plot navigation tools and pause the graph"""
        if not self.navigation_enabled:
            self.plotting_enabled = False  # Pause the graph updates
            self.paused_time = time.time() - self.start_time
            
            # Create a frame for the toolbar to avoid mixing geometry managers
            self.toolbar_frame = ctk.CTkFrame(self.plot_container, height=40)
            self.toolbar_frame.pack(side="bottom", fill="x")
            
            self.navigation_toolbar = NavigationToolbar2Tk(self.canvas, self.toolbar_frame)
            self.navigation_toolbar.update()
            self.navigation_button.configure(text="Disable Navigation")
            self.navigation_enabled = True
            
            # Show values frame and update with current data
            self.values_frame.grid()
            self.update_values_display(self.paused_time)
            
            # Add vertical line at current time
            self.vertical_line = self.ax.axvline(x=self.paused_time, color='red', linestyle='--')
            self.canvas.draw()

    def disable_navigation(self):
        """Disable plot navigation tools and resume the graph"""
        if self.navigation_enabled:
            if hasattr(self, 'navigation_toolbar') and self.navigation_toolbar:
                self.navigation_toolbar.destroy()
                self.navigation_toolbar = None
            if hasattr(self, 'toolbar_frame') and self.toolbar_frame:
                self.toolbar_frame.destroy()
                self.toolbar_frame = None
            self.navigation_button.configure(text="Enable Navigation")
            self.plotting_enabled = True  # Resume the graph updates
            self.paused_time = None
            self.navigation_enabled = False
            
            # Hide values frame
            self.values_frame.grid_remove()
            
            if hasattr(self, 'vertical_line') and self.vertical_line:
                self.vertical_line.remove()
                self.vertical_line = None
            self.update_plot()

    def update_values_display(self, time_value):
        """Update the values frame with data at specific time"""
        # Clear previous values
        for widget in self.values_container.winfo_children():
            widget.destroy()
        
        # Update time label
        self.paused_time_label.configure(text=f"Time: {time_value:.5f}s")
        
        # Add current values
        for i, signal_name in enumerate(self.signals):
            if self.signals[signal_name].get() and self.data_buffer[i]:
                timestamps, values = zip(*self.data_buffer[i])
                timestamps = np.array(timestamps)
                values = np.array(values)
                idx = np.abs(timestamps - time_value).argmin()
                
                frame = ctk.CTkFrame(self.values_container, fg_color=self.frame_color,
                                   border_width=1, border_color="#555555")
                frame.pack(fill="x", padx=5, pady=2)
                
                label = ctk.CTkLabel(frame, text=self.signals[signal_name].cget("text"), 
                                    anchor="w", text_color=self.text_color, font=("Arial", 14))
                label.pack(side="left", padx=10, pady=5)
                
                value_label = ctk.CTkLabel(frame, text="0.00", width=80, 
                                       fg_color="#1f1f1f", corner_radius=3,
                                       text_color="white", font=("Arial", 14))
                value_label.pack(side="right", padx=10, pady=5)
                
                value_label.configure(text=f"{values[idx]:.4f}")

    def move_vertical_line(self, event):
        """Move vertical line on mouse motion"""
        if self.navigation_enabled and event.inaxes:
            x = event.xdata
            if self.vertical_line:
                self.vertical_line.set_xdata([x, x])
            else:
                self.vertical_line = self.ax.axvline(x=x, color='red', linestyle='--')
            self.update_values_display(x)
            self.canvas.draw()

    def list_signals(self):
        """Create window to manage signals"""
        if not hasattr(self, "list_window") or not self.list_window.winfo_exists():
            self.list_window = ctk.CTkToplevel(self.root)
            self.list_window.title("Manage Signals")
            self.list_window.geometry("400x400")
            self.list_window.attributes("-topmost", True)
            self.list_window.grid_rowconfigure(1, weight=1)
            self.list_window.grid_columnconfigure(0, weight=1)

            # Add signal controls
            add_frame = ctk.CTkFrame(self.list_window)
            add_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=10)
            
            self.signal_entry = ctk.CTkEntry(add_frame, placeholder_text="New signal name")
            self.signal_entry.pack(side="left", padx=5, pady=5, expand=True, fill="x")
            
            add_btn = ctk.CTkButton(add_frame, text="Add", width=80,
                                   command=self.add_signal_from_entry,
                                   fg_color=self.button_color,
                                   hover_color=self.hover_color)
            add_btn.pack(side="right", padx=5, pady=5)

            # Signals list with scrollbar
            scroll_frame = ctk.CTkScrollableFrame(self.list_window)
            scroll_frame.grid(row=1, column=0, sticky="nsew", padx=10, pady=(0,10))
            
            # Populate with current signals
            for signal_name in self.signals:
                frame = ctk.CTkFrame(scroll_frame, border_width=1, border_color="#555555")
                frame.pack(fill="x", padx=5, pady=2)

                label = ctk.CTkLabel(frame, text=self.signals[signal_name].cget("text"),
                                    width=250, anchor="w")
                label.pack(side="left", padx=10, pady=5)

                remove_btn = ctk.CTkButton(frame, text="Remove", width=80,
                                         command=lambda name=signal_name: self.remove_signal(name),
                                         fg_color="#d9534f", hover_color="#c9302c")
                remove_btn.pack(side="right", padx=5, pady=5)

    def add_signal_from_entry(self):
        """Add signal from entry box"""
        signal_name = self.signal_entry.get().strip()
        if signal_name:
            self.add_signal(signal_name)
            self.signal_entry.delete(0, "end")
            self.list_window.destroy()
            self.list_signals()  # Refresh the list

    def add_signal(self, signal_name=None):
        """Add a new signal"""
        if signal_name is None:
            signal_name = simpledialog.askstring("Add Signal", "Enter signal name:")
        if signal_name:
            self.num_signals += 1
            signal_key = f"Signal {self.num_signals}"
            self.signals[signal_key] = ctk.CTkCheckBox(self.signals_frame, text=signal_name,
                                                     command=lambda i=self.num_signals-1: self.toggle_signal(i))
            self.signals[signal_key].pack(side="top", padx=5, pady=2, anchor="w")
            self.signals[signal_key].select()  # Default to checked
            self.data_buffer.append(deque(maxlen=1000))  # Increased buffer size
            self.raw_data_buffer.append(deque(maxlen=1000))  # For raw data storage
            
            # Update text display if it exists
            if self.text_display_frame and self.text_display_frame.winfo_exists():
                self.create_text_display()
            
            self.save_signals()
            self.update_plot()

    def toggle_signal(self, signal_index):
        """Toggle signal display and update plot"""
        if not self.signals[f"Signal {signal_index + 1}"].get():
            self.data_buffer[signal_index].clear()
        self.update_plot()

    def remove_signal(self, signal_name):
        """Remove a signal"""
        if signal_name in self.signals:
            signal_index = list(self.signals.keys()).index(signal_name)
            self.signals[signal_name].destroy()
            del self.signals[signal_name]
            self.data_buffer.pop(signal_index)
            self.raw_data_buffer.pop(signal_index)
            if signal_index < len(self.value_boxes):
                self.value_boxes.pop(signal_index)
            self.renumber_signals()
            self.num_signals -= 1
            self.save_signals()
            
            # Update displays
            if self.text_display_frame and self.text_display_frame.winfo_exists():
                self.create_text_display()
            
            # Refresh list window if open
            if hasattr(self, "list_window") and self.list_window.winfo_exists():
                self.list_window.destroy()
                self.list_signals()
            
            self.update_plot()

    def renumber_signals(self):
        """Renumber signals after removal"""
        new_signals = {}
        new_value_boxes = []
        for i, signal_name in enumerate(self.signals.keys()):
            new_signals[f"Signal {i + 1}"] = self.signals[signal_name]
            if i < len(self.value_boxes):
                new_value_boxes.append(self.value_boxes[i])
        self.signals = new_signals
        self.value_boxes = new_value_boxes

    def save_signals(self):
        """Save signals to file"""
        with open(self.SIGNALS_FILE, "w") as file:
            for signal in self.signals:
                file.write(self.signals[signal].cget("text") + "\n")

    def load_signals(self):
        """Load signals from file"""
        if os.path.exists(self.SIGNALS_FILE):
            with open(self.SIGNALS_FILE, "r") as file:
                for line in file:
                    signal_name = line.strip()
                    if signal_name:
                        self.num_signals += 1
                        signal_key = f"Signal {self.num_signals}"
                        self.signals[signal_key] = ctk.CTkCheckBox(self.signals_frame, 
                                                                 text=signal_name,
                                                                 command=lambda i=self.num_signals-1: self.toggle_signal(i))
                        self.signals[signal_key].pack(side="top", padx=5, pady=2, anchor="w")
                        self.signals[signal_key].deselect()  # Default to checked
                        self.data_buffer.append(deque(maxlen=1000))  # Increased buffer size
                        self.raw_data_buffer.append(deque(maxlen=1000))  # For raw data storage

    def set_time_step(self):
        """Set the plot update interval"""
        try:
            new_interval = float(self.time_step_entry.get())
            if new_interval > 0:
                self.update_interval = new_interval
                print(f"Time step updated to {self.update_interval} seconds")
            else:
                print("Time step must be positive")
        except ValueError:
            print("Invalid time step value")

if __name__ == "__main__":
    root = ctk.CTk()
    app = BluetoothMonitor(root)
    root.mainloop()