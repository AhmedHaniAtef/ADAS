import tkinter as tk
import customtkinter as ctk
import math
import serial
import serial.tools.list_ports
from threading import Thread, Lock
import time
import os
from tkinter import simpledialog

class SteeringWheelApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Vehicle Control Simulator")
        self.root.geometry("1500x900")
        
        # Serial connection
        self.ser = None
        self.com_port = "COM5"
        self.baud_rate = 115200
        self.connected = False
        self.serial_lock = Lock()
        
        # Appearance
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")
        
        # Control variables
        self.steering_angle = 0  # -90 to 90 degrees
        self.acceleration = 0    # 0 to 100%
        self.move_direction = 0  # -1=backward, 0=stop, 1=forward
        self.last_angle = 0      # For smooth wheel rotation
        self.wheel_dragging = False
        self.steering_mode = "steering"  # "steering" or "rotation"
        
        # State tracking
        self.last_steering_angle = 0
        self.last_move_direction = 0
        self.last_steering_mode = "steering"
        self.last_sent_command = None
        
        # Custom buttons
        self.button_names = [f"Button {i+1}" for i in range(8)]
        self.button_states = [0] * 8  # 0=OFF, 1=ON
        self.button_types = ["toggle"] * 8  # "toggle" or "push"
        self.button_keys = [""] * 8  # Keyboard bindings
        self.load_button_config()
        self.custom_buttons = []
        
        # Create UI
        self.setup_ui()
        
        # Mouse wheel binding for acceleration
        self.root.bind("<MouseWheel>", self.on_mouse_wheel)
        
        # Start serial monitor thread
        self.serial_thread = Thread(target=self.serial_monitor, daemon=True)
        self.serial_thread.start()

    def setup_ui(self):
        """Initialize all UI components"""
        # Connection frame
        self.connection_frame = ctk.CTkFrame(self.root)
        self.connection_frame.pack(fill=tk.X, padx=10, pady=5)
        
        # Port selection
        self.port_label = ctk.CTkLabel(self.connection_frame, text="COM Port:")
        self.port_label.grid(row=0, column=0, padx=5)
        
        self.ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_var = tk.StringVar(value=self.com_port if self.com_port in self.ports else self.ports[0] if self.ports else "COM5")
        self.port_menu = ctk.CTkOptionMenu(self.connection_frame, variable=self.port_var, values=self.ports)
        self.port_menu.grid(row=0, column=1, padx=5)
        
        # Refresh button
        self.refresh_btn = ctk.CTkButton(self.connection_frame, text="↻", width=30, command=self.refresh_ports)
        self.refresh_btn.grid(row=0, column=2, padx=5)
        
        # Connect button
        self.connect_btn = ctk.CTkButton(self.connection_frame, text="Connect", command=self.toggle_connection, width=100)
        self.connect_btn.grid(row=0, column=3, padx=5)
        
        # Status label
        self.status_var = tk.StringVar(value="Disconnected")
        self.status_label = ctk.CTkLabel(self.connection_frame, textvariable=self.status_var, text_color="red")
        self.status_label.grid(row=0, column=4, padx=5)
        
        # Main frame
        self.main_frame = ctk.CTkFrame(self.root)
        self.main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # Left column (controls)
        self.left_column = ctk.CTkFrame(self.main_frame)
        self.left_column.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Right column (buttons)
        self.right_column = ctk.CTkFrame(self.main_frame)
        self.right_column.pack(side=tk.RIGHT, fill=tk.BOTH, padx=5, pady=5)
        
        # Title label
        self.title_label = ctk.CTkLabel(self.left_column, text="Vehicle Control Simulator", font=("Arial", 20, "bold"))
        self.title_label.pack(pady=(10, 20))
        
        # Steering wheel canvas
        self.canvas = tk.Canvas(self.left_column, width=300, height=300, bg="#2b2b2b", highlightthickness=0)
        self.canvas.pack(pady=10)
        self.draw_steering_wheel()
        
        # Mouse bindings
        self.canvas.bind("<Button-1>", self.start_wheel_drag)
        self.canvas.bind("<B1-Motion>", self.rotate_wheel_drag)
        self.canvas.bind("<ButtonRelease-1>", self.stop_wheel_drag)
        
        # Keyboard bindings
        self.root.bind("<KeyPress>", self.key_press)
        self.root.bind("<KeyRelease>", self.key_release)
        
        # Mode button
        self.mode_btn = ctk.CTkButton(
            self.left_column,
            text="Steering (Q)",
            command=self.toggle_steering_mode,
            width=120,
            height=40,
            fg_color="#9C27B0",
            hover_color="#7B1FA2"
        )
        self.mode_btn.pack(pady=10)
        
        # Movement controls
        self.movement_frame = ctk.CTkFrame(self.left_column)
        self.movement_frame.pack(pady=10)
        
        self.forward_btn = ctk.CTkButton(
            self.movement_frame,
            text="Forward (W)",
            command=lambda: self.set_movement(1),
            width=120,
            height=40,
            fg_color="#4CAF50",
            hover_color="#45a049"
        )
        self.forward_btn.pack(pady=5)
        
        self.stop_btn = ctk.CTkButton(
            self.movement_frame,
            text="Stop",
            command=lambda: self.set_movement(0),
            width=120,
            height=40,
            fg_color="#616161",
            hover_color="#424242"
        )
        self.stop_btn.pack(pady=5)
        
        self.backward_btn = ctk.CTkButton(
            self.movement_frame,
            text="Backward (S)",
            command=lambda: self.set_movement(-1),
            width=120,
            height=40,
            fg_color="#F44336",
            hover_color="#d32f2f"
        )
        self.backward_btn.pack(pady=5)
        
        # Acceleration control
        self.accel_frame = ctk.CTkFrame(self.left_column)
        self.accel_frame.pack(pady=20, fill=tk.X, padx=50)
        
        self.accel_label = ctk.CTkLabel(self.accel_frame, text="ACCELERATOR LEVEL:", font=("Arial", 14, "bold"))
        self.accel_label.pack(pady=(10, 5))
        
        self.accel_bar = ctk.CTkProgressBar(
            self.accel_frame,
            orientation="horizontal",
            width=400,
            height=30,
            corner_radius=15,
            progress_color="green",
            mode="determinate"
        )
        self.accel_bar.set(0)
        self.accel_bar.pack(pady=10)
        
        self.accel_slider = ctk.CTkSlider(
            self.accel_frame,
            from_=0,
            to=100,
            command=self.update_acceleration,
            width=400,
            height=15,
            button_color="#2196F3",
            button_hover_color="#1976D2"
        )
        self.accel_slider.set(0)
        self.accel_slider.pack(pady=(0, 10))
        
        # Status display
        self.status_frame = ctk.CTkFrame(self.left_column)
        self.status_frame.pack(pady=10, fill=tk.X, padx=50)
        
        self.steering_var = tk.StringVar(value="Steering Angle: 0°")
        self.steering_label = ctk.CTkLabel(self.status_frame, textvariable=self.steering_var, font=("Arial", 14), text_color="#2196F3")
        self.steering_label.pack(side=tk.LEFT, padx=20)
        
        self.accel_var = tk.StringVar(value="Acceleration: 0%")
        self.accel_label = ctk.CTkLabel(self.status_frame, textvariable=self.accel_var, font=("Arial", 14), text_color="#4CAF50")
        self.accel_label.pack(side=tk.LEFT, padx=20)
        
        self.movement_var = tk.StringVar(value="Movement: Stopped")
        self.movement_label = ctk.CTkLabel(self.status_frame, textvariable=self.movement_var, font=("Arial", 14), text_color="#FF9800")
        self.movement_label.pack(side=tk.LEFT, padx=20)
        
        self.mode_var = tk.StringVar(value="Mode: Steering")
        self.mode_status_label = ctk.CTkLabel(self.status_frame, textvariable=self.mode_var, font=("Arial", 14), text_color="#9C27B0")
        self.mode_status_label.pack(side=tk.LEFT, padx=20)
        
        # Custom buttons frame
        self.custom_buttons_frame = ctk.CTkFrame(self.right_column)
        self.custom_buttons_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        self.custom_title = ctk.CTkLabel(
            self.custom_buttons_frame, 
            text="Custom Buttons",
            font=("Arial", 16, "bold")
        )
        self.custom_title.pack(pady=(5, 10))
        
        # Add configure button
        self.configure_btn = ctk.CTkButton(
            self.custom_buttons_frame,
            text="Configure Buttons",
            command=self.configure_buttons,
            width=150,
            height=30,
            fg_color="#607D8B",
            hover_color="#455A64"
        )
        self.configure_btn.pack(pady=(0, 10))
        
        self.create_custom_buttons()

    def on_mouse_wheel(self, event):
        """Handle mouse wheel events for acceleration control"""
        if event.delta > 0:  # Scroll up
            new_accel = min(100, self.acceleration + 5)
        else:  # Scroll down
            new_accel = max(0, self.acceleration - 5)
        
        if new_accel != self.acceleration:
            self.acceleration = new_accel
            self.accel_slider.set(self.acceleration)
            self.update_acceleration(self.acceleration)

    def start_wheel_drag(self, event):
        """Handle mouse drag start on steering wheel"""
        self.wheel_dragging = True
        # Calculate initial angle
        center_x, center_y = 150, 150
        dx = event.x - center_x
        dy = event.y - center_y
        self.last_angle = math.degrees(math.atan2(dy, dx))

    def rotate_wheel_drag(self, event):
        """Handle steering wheel rotation with mouse"""
        if self.wheel_dragging:
            center_x, center_y = 150, 150
            dx = event.x - center_x
            dy = event.y - center_y
            
            # Calculate current angle
            current_angle = math.degrees(math.atan2(dy, dx))
            
            # Calculate angle difference from last position
            angle_diff = current_angle - self.last_angle
            
            # Handle wrap-around at 180/-180 degrees
            if angle_diff > 180:
                angle_diff -= 360
            elif angle_diff < -180:
                angle_diff += 360
            
            # Update steering angle (scale down the difference for better control)
            self.steering_angle = max(-90, min(90, self.steering_angle + angle_diff * 0.5))
            
            # Store current angle for next movement
            self.last_angle = current_angle
            
            # Draw wheel with visual rotation (exaggerated for visibility)
            visual_rotation = self.steering_angle * 2.5
            self.draw_steering_wheel(visual_rotation)
            
            # Update display
            if self.steering_mode == "steering":
                self.steering_var.set(f"Steering Angle: {int(self.steering_angle)}°")
            else:
                radius = (self.steering_angle / 90) * 100
                self.steering_var.set(f"Rotation Radius: {radius:.1f}%")
            
            # Send command if moving
            if self.move_direction != 0:
                self.send_movement_command()

    def stop_wheel_drag(self, event):
        """Handle mouse drag end on steering wheel"""
        self.wheel_dragging = False

    def configure_buttons(self):
        """Open configuration window for buttons"""
        config_window = ctk.CTkToplevel(self.root)
        config_window.title("Configure Buttons")
        config_window.geometry("600x400")
        
        # Create a frame for the configuration options
        config_frame = ctk.CTkFrame(config_window)
        config_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Create a notebook for each button
        for i in range(8):
            button_frame = ctk.CTkFrame(config_frame)
            button_frame.pack(fill=tk.X, padx=5, pady=5)
            
            # Button number label
            ctk.CTkLabel(button_frame, text=f"Button {i+1}:").pack(side=tk.LEFT, padx=5)
            
            # Name entry
            name_var = tk.StringVar(value=self.button_names[i])
            name_entry = ctk.CTkEntry(button_frame, textvariable=name_var, width=150)
            name_entry.pack(side=tk.LEFT, padx=5)
            
            # Type selection
            type_var = tk.StringVar(value=self.button_types[i])
            type_menu = ctk.CTkOptionMenu(button_frame, variable=type_var, values=["toggle", "push"])
            type_menu.pack(side=tk.LEFT, padx=5)
            
            # Key binding
            key_var = tk.StringVar(value=self.button_keys[i])
            key_entry = ctk.CTkEntry(button_frame, textvariable=key_var, width=50)
            key_entry.pack(side=tk.LEFT, padx=5)
            
            # Save function for this button
            def save_config(idx=i, n_var=name_var, t_var=type_var, k_var=key_var):
                self.button_names[idx] = n_var.get()
                self.button_types[idx] = t_var.get()
                self.button_keys[idx] = k_var.get().upper()
                self.save_button_config()
                self.create_custom_buttons()
                # Update key bindings
                self.update_key_bindings()
            
            # Save button
            save_btn = ctk.CTkButton(button_frame, text="Save", command=save_config, width=60)
            save_btn.pack(side=tk.LEFT, padx=5)

    def update_key_bindings(self):
        """Update keyboard bindings based on button configurations"""
        # Remove all existing button key bindings
        for key in self.button_keys:
            if key:
                self.root.unbind(f"<KeyPress-{key}>")
                self.root.unbind(f"<KeyRelease-{key}>")
        
        # Add new bindings
        for i, key in enumerate(self.button_keys):
            if key:
                if self.button_types[i] == "toggle":
                    self.root.bind(f"<KeyPress-{key}>", lambda e, idx=i: self.toggle_button(idx))
                else:  # push button
                    self.root.bind(f"<KeyPress-{key}>", lambda e, idx=i: self.push_button_down(idx))
                    self.root.bind(f"<KeyRelease-{key}>", lambda e, idx=i: self.push_button_up(idx))

    def create_custom_buttons(self):
        """Create 8 custom buttons in 2 columns"""
        for widget in self.custom_buttons_frame.winfo_children():
            if widget not in [self.custom_title, self.configure_btn]:
                widget.destroy()
        self.custom_buttons.clear()
        
        for row in range(4):
            frame = ctk.CTkFrame(self.custom_buttons_frame)
            frame.pack(fill=tk.X, padx=5, pady=5)
            
            for col in range(2):
                btn_index = row * 2 + col
                if btn_index >= 8:
                    break
                    
                btn_text = self.button_names[btn_index]
                if self.button_types[btn_index] == "toggle":
                    state = "ON" if self.button_states[btn_index] else "OFF"
                    btn_text += f" ({state})"
                    if self.button_keys[btn_index]:
                        btn_text += f" [{self.button_keys[btn_index]}]"
                
                btn = ctk.CTkButton(
                    frame,
                    text=btn_text,
                    command=lambda idx=btn_index: self.activate_button(idx),
                    width=150,
                    height=40,
                    fg_color="#4CAF50" if self.button_states[btn_index] else "#607D8B",
                    hover_color="#45a049" if self.button_states[btn_index] else "#455A64"
                )
                btn.pack(side=tk.LEFT, padx=5, expand=True)
                self.custom_buttons.append(btn)

    def activate_button(self, button_index):
        """Handle button activation based on its type"""
        if self.button_types[button_index] == "toggle":
            self.toggle_button(button_index)
        else:
            self.push_button_down(button_index)
            # For push buttons, we'll automatically release after a short delay
            self.root.after(200, lambda: self.push_button_up(button_index))

    def toggle_button(self, button_index):
        """Toggle button state and update appearance"""
        self.button_states[button_index] = 1 - self.button_states[button_index]
        self.update_button_appearance(button_index)
        self.send_button_states()

    def push_button_down(self, button_index):
        """Handle push button press"""
        self.button_states[button_index] = 1
        self.update_button_appearance(button_index)
        self.send_button_states()

    def push_button_up(self, button_index):
        """Handle push button release"""
        self.button_states[button_index] = 0
        self.update_button_appearance(button_index)
        self.send_button_states()

    def update_button_appearance(self, button_index):
        """Update button appearance based on its state and type"""
        btn_text = self.button_names[button_index]
        if self.button_types[button_index] == "toggle":
            state = "ON" if self.button_states[button_index] else "OFF"
            btn_text += f" ({state})"
            if self.button_keys[button_index]:
                btn_text += f" [{self.button_keys[button_index]}]"
        
        self.custom_buttons[button_index].configure(
            text=btn_text,
            fg_color="#4CAF50" if self.button_states[button_index] else "#607D8B",
            hover_color="#45a049" if self.button_states[button_index] else "#455A64"
        )

    def send_button_states(self):
        """Send current state of all 8 buttons"""
        if not self.connected:
            return
            
        # Create command string "BUTTONS:1,0,1,0,0,0,1,0"
        states_str = ",".join(map(str, self.button_states))
        command = f"BUTTONS:{states_str}"
        
        try:
            with self.serial_lock:
                if self.ser and self.ser.is_open:
                    self.ser.write(f"{command}\n".encode('utf-8'))
                    print(f"Sent button states: {command}")
        except Exception as e:
            print(f"Serial write error: {str(e)}")
            self.disconnect_serial()

    def load_button_config(self):
        """Load button configuration from file"""
        if os.path.exists("button_config.txt"):
            try:
                with open("button_config.txt", "r") as f:
                    for i, line in enumerate(f.readlines()):
                        if i >= 8:
                            break
                        parts = line.strip().split("|")
                        if len(parts) >= 1:
                            self.button_names[i] = parts[0]
                        if len(parts) >= 2:
                            self.button_types[i] = parts[1]
                        if len(parts) >= 3:
                            self.button_keys[i] = parts[2]
            except Exception as e:
                print(f"Error loading button config: {e}")

    def save_button_config(self):
        """Save button configuration to file"""
        try:
            with open("button_config.txt", "w") as f:
                for i in range(8):
                    line = f"{self.button_names[i]}|{self.button_types[i]}|{self.button_keys[i]}"
                    f.write(line + "\n")
        except Exception as e:
            print(f"Error saving button config: {e}")

    def refresh_ports(self):
        """Refresh available COM ports"""
        self.ports = [port.device for port in serial.tools.list_ports.comports()]
        self.port_menu.configure(values=self.ports)
        if self.ports:
            self.port_var.set(self.ports[0])

    def toggle_connection(self):
        """Toggle serial connection"""
        if not self.connected:
            self.connect_serial()
        else:
            self.disconnect_serial()

    def connect_serial(self):
        """Establish serial connection"""
        try:
            with self.serial_lock:
                self.ser = serial.Serial(
                    port=self.port_var.get(),
                    baudrate=self.baud_rate,
                    timeout=1
                )
                self.connected = True
                self.status_var.set("Connected")
                self.status_label.configure(text_color="green")
                self.connect_btn.configure(text="Disconnect")
                print(f"Connected to {self.port_var.get()}")
        except Exception as e:
            print(f"Connection failed: {str(e)}")
            self.status_var.set("Connection Failed")

    def disconnect_serial(self):
        """Close serial connection"""
        with self.serial_lock:
            if self.ser and self.ser.is_open:
                self.ser.close()
            self.connected = False
        self.status_var.set("Disconnected")
        self.status_label.configure(text_color="red")
        self.connect_btn.configure(text="Connect")
        print("Disconnected from serial port")

    def serial_monitor(self):
        """Monitor incoming serial data"""
        while True:
            if self.connected:
                try:
                    with self.serial_lock:
                        if self.ser and self.ser.in_waiting:
                            data = self.ser.readline().decode('utf-8').strip()
                            print(f"Received: {data}")
                except Exception as e:
                    print(f"Serial read error: {str(e)}")
                    self.disconnect_serial()
            time.sleep(0.1)

    def send_serial_command(self, command):
        """Send command over serial"""
        if self.connected and command != self.last_sent_command:
            try:
                with self.serial_lock:
                    if self.ser and self.ser.is_open:
                        self.ser.write(f"{command}\n".encode('utf-8'))
                        print(f"Sent: {command}")
                        self.last_sent_command = command
            except Exception as e:
                print(f"Serial write error: {str(e)}")
                self.disconnect_serial()

    def draw_steering_wheel(self, visual_angle=None):
        """Draw steering wheel with current angle"""
        self.canvas.delete("all")
        rotation = math.radians(visual_angle if visual_angle is not None else self.steering_angle * 2.5)
        
        # Draw wheel components
        self.canvas.create_oval(25, 25, 275, 275, outline="#2196F3", width=15, fill="#333333")
        self.canvas.create_oval(75, 75, 225, 225, outline="#2196F3", width=5, fill="#444444")
        
        # Draw spokes
        for angle in range(0, 360, 45):
            rad = math.radians(angle) + rotation
            x1, y1 = 150 + 70 * math.cos(rad), 150 + 70 * math.sin(rad)
            x2, y2 = 150 + 125 * math.cos(rad), 150 + 125 * math.sin(rad)
            self.canvas.create_line(x1, y1, x2, y2, fill="#2196F3", width=3)
        
        # Draw center and indicator
        self.canvas.create_oval(145, 145, 155, 155, fill="#2196F3", outline="")
        
        pointer_size = 15
        points = [
            150, 25,
            150 - pointer_size, 25 + pointer_size * 1.5,
            150 + pointer_size, 25 + pointer_size * 1.5
        ]
        rotated = []
        for i in range(0, len(points), 2):
            x, y = points[i] - 150, points[i+1] - 150
            new_x = x * math.cos(rotation) - y * math.sin(rotation) + 150
            new_y = x * math.sin(rotation) + y * math.cos(rotation) + 150
            rotated.extend([new_x, new_y])
        
        self.canvas.create_polygon(*rotated, fill="#FF9800", outline="#E65100", width=2)

    def toggle_steering_mode(self):
        """Switch between steering and rotation modes"""
        if self.steering_mode == "steering":
            new_mode = "rotation"
            if self.last_steering_mode != new_mode:
                self.steering_mode = new_mode
                self.mode_btn.configure(text="Rotation (Q)", fg_color="#FF9800", hover_color="#F57C00")
                self.mode_var.set("Mode: Rotation")
                self.send_serial_command("MODE:ROTATION")
                self.last_steering_mode = new_mode
        else:
            new_mode = "steering"
            if self.last_steering_mode != new_mode:
                self.steering_mode = new_mode
                self.mode_btn.configure(text="Steering (Q)", fg_color="#9C27B0", hover_color="#7B1FA2")
                self.mode_var.set("Mode: Steering")
                self.send_serial_command("MODE:STEERING")
                self.last_steering_mode = new_mode
        
        if self.move_direction != 0:
            self.send_movement_command()

    def set_movement(self, direction):
        """Set movement direction and send command if changed"""
        if direction != self.last_move_direction:
            self.move_direction = direction
            self.last_move_direction = direction
            
            if direction == 1:
                self.movement_var.set("Movement: Forward")
            elif direction == -1:
                self.movement_var.set("Movement: Backward")
            else:
                self.movement_var.set("Movement: Stopped")
            
            self.send_movement_command()

    def send_movement_command(self):
        """Send combined movement command based on current state"""
        if self.move_direction != 0:
            if self.steering_mode == "steering":
                command = f"STEER:{self.steering_angle}:{self.acceleration}:{self.move_direction}"
            else:
                radius = (self.steering_angle / 90) * 100
                command = f"ROTATE:{radius}:{self.acceleration}:{self.move_direction}"
        else:
            if self.steering_mode == "steering":
                command = "STEER:0:0:0"
            else:
                command = "ROTATE:0:0:0"
        
        self.send_serial_command(command)

    def update_acceleration(self, value):
        """Update acceleration value"""
        self.acceleration = float(value)
        self.accel_var.set(f"Acceleration: {int(self.acceleration)}%")
        self.accel_bar.set(self.acceleration / 100)
        
        # Update color
        color = "#4CAF50" if self.acceleration < 30 else "#FFC107" if self.acceleration < 70 else "#F44336"
        self.accel_bar.configure(progress_color=color)
        
        if self.move_direction != 0:
            self.send_movement_command()

    def key_press(self, event):
        """Handle keyboard input"""
        key = event.char.lower() if event.char else ""
        key_upper = event.keysym.upper()
        
        # Check if key matches any button binding
        for i, btn_key in enumerate(self.button_keys):
            if btn_key and key_upper == btn_key.upper():
                if self.button_types[i] == "toggle":
                    self.toggle_button(i)
                else:
                    self.push_button_down(i)
                return
        
        # Standard controls
        if key == 'q':
            self.toggle_steering_mode()
        elif key == 'a':
            # Rotate wheel left (visual only)
            self.steering_angle = max(-90, self.steering_angle - 5)
            self.update_steering_display()
        elif key == 'd':
            # Rotate wheel right (visual only)
            self.steering_angle = min(90, self.steering_angle + 5)
            self.update_steering_display()
        elif key == 'w':
            if self.move_direction != 1:  # Only if not already moving forward
                self.set_movement(1)
        elif key == 's':
            if self.move_direction != -1:  # Only if not already moving backward
                self.set_movement(-1)

    def key_release(self, event):
        """Handle key release"""
        key_upper = event.keysym.upper()
        
        # Check if released key matches any push button binding
        for i, btn_key in enumerate(self.button_keys):
            if btn_key and key_upper == btn_key.upper() and self.button_types[i] == "push":
                self.push_button_up(i)
                return
        
        # Standard controls
        key = event.char.lower() if event.char else ""
        if key in ('w', 's'):
            if (key == 'w' and self.move_direction == 1) or \
               (key == 's' and self.move_direction == -1):
                self.set_movement(0)

    def update_steering_display(self):
        """Update wheel display and send command if moving"""
        visual_rotation = self.steering_angle * 2.5
        self.draw_steering_wheel(visual_rotation)
        
        if self.steering_mode == "steering":
            self.steering_var.set(f"Steering Angle: {int(self.steering_angle)}°")
        else:
            radius = (self.steering_angle / 90) * 100
            self.steering_var.set(f"Rotation Radius: {radius:.1f}%")
        
        if self.move_direction != 0 and self.steering_angle != self.last_steering_angle:
            self.last_steering_angle = self.steering_angle
            self.send_movement_command()

if __name__ == "__main__":
    root = ctk.CTk()
    app = SteeringWheelApp(root)
    root.mainloop()