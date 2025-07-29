import tkinter as tk
import customtkinter as ctk
import math
import serial
import serial.tools.list_ports
from threading import Thread, Lock, Event
import time
import os
from tkinter import simpledialog
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from collections import deque
import cv2
from PIL import Image, ImageTk
import torch
from ultralytics import YOLO
import random
import winsound
import pygame
from pygame import mixer
import threading
import smtplib
import requests
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

class ConfigurationWindow:
    def __init__(self):
        self.root = ctk.CTk()
        self.root.title("Configuration")

        # Get screen dimensions
        self.screen_width = self.root.winfo_screenwidth()
        self.screen_height = self.root.winfo_screenheight()
        
        # Calculate window dimensions (90% of screen)
        self.window_width = int(self.screen_width * 0.3)
        self.window_height = int(self.screen_height * 0.9)
        
        # Set window size and minimum size
        self.root.geometry(f"{self.window_width}x{self.window_height}")
        self.root.minsize(int(self.screen_width * 0.3), int(self.screen_height * 0.9))
        
        # Calculate scaling factors for fonts and widgets
        self.scale_factor = min(self.window_width / 400, self.window_height / 970)
        self.title_font = ("Arial", int(20 * self.scale_factor), "bold")
        self.normal_font = ("Arial", int(14 * self.scale_factor))
        self.small_font = ("Arial", int(12 * self.scale_factor))

        # Make window modal
        self.root.transient()
        self.root.grab_set()
        
        # Initialize configuration variables
        self.config = {
            'model_confidence': ctk.DoubleVar(value=0.85),
            'model_overlap': ctk.DoubleVar(value=0.5),
            'model_name': ctk.StringVar(value="best.pt"),
            'image_size': ctk.StringVar(value=1024),
            'camera_index': ctk.IntVar(value=0),
            'camera_resolution': ctk.StringVar(value="1920x1080"),
            'theme_mode': ctk.StringVar(value="dark"),
            'theme_color': ctk.StringVar(value="blue"),
            # Drowsiness configuration
            'drowsy_threshold': ctk.StringVar(value=3.0),
            'emergency_threshold': ctk.StringVar(value=8.0),
            'awake_reset_time': ctk.StringVar(value=3.0),
            'blink_threshold': ctk.StringVar(value=0.3),
            'blink_window': ctk.StringVar(value=2.0),
        }
        
        # Create main scrollable frame
        self.main_frame = ctk.CTkScrollableFrame(self.root)
        self.main_frame.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Create value labels
        self.confidence_label = ctk.CTkLabel(self.main_frame, text="Confidence: 0.85")
        self.overlap_label = ctk.CTkLabel(self.main_frame, text="Overlap: 0.5")
        self.resolution_label = ctk.CTkLabel(self.main_frame, text="Resolution: 1920x1080")
        
        self.create_widgets()
        
    def create_widgets(self):
        # Model Configuration Frame
        model_frame = ctk.CTkFrame(self.main_frame)
        model_frame.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        ctk.CTkLabel(model_frame, text="Model Configuration", font=("Arial", int(16 * self.scale_factor), "bold")).pack(pady=int(10 * self.scale_factor))
        
        # Model Confidence
        ctk.CTkLabel(model_frame, text="Model Confidence:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        confidence_slider = ctk.CTkSlider(model_frame, from_=0, to=1, 
                                        variable=self.config['model_confidence'],
                                        command=self.update_confidence_label)
        confidence_slider.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        self.confidence_label.pack(pady=int(5 * self.scale_factor))
        
        # Model Overlap
        ctk.CTkLabel(model_frame, text="Model Overlap:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        overlap_slider = ctk.CTkSlider(model_frame, from_=0, to=1,
                                     variable=self.config['model_overlap'],
                                     command=self.update_overlap_label)
        overlap_slider.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        self.overlap_label.pack(pady=int(5 * self.scale_factor))
        
        # Model Name
        ctk.CTkLabel(model_frame, text="Model Name:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        model_entry = ctk.CTkEntry(model_frame, textvariable=self.config['model_name'])
        model_entry.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))

        # Image Size
        ctk.CTkLabel(model_frame, text="Image Size:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        model_image_size = ctk.CTkEntry(model_frame, textvariable=self.config['image_size'])
        model_image_size.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Camera Configuration Frame
        camera_frame = ctk.CTkFrame(self.main_frame)
        camera_frame.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        ctk.CTkLabel(camera_frame, text="Camera Configuration", font=("Arial", int(16 * self.scale_factor), "bold")).pack(pady=int(10 * self.scale_factor))
        
        # Get available cameras
        self.available_cameras = self.get_available_cameras()
        
        # Camera Selection
        ctk.CTkLabel(camera_frame, text="Select Camera:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        camera_menu = ctk.CTkOptionMenu(camera_frame, 
                                      values=[f"Camera {i}" for i in range(len(self.available_cameras))],
                                      variable=self.config['camera_index'])
        camera_menu.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Camera Resolution
        ctk.CTkLabel(camera_frame, text="Camera Resolution:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        resolution_menu = ctk.CTkOptionMenu(camera_frame,
                                          values=["640x480", "1280x720", "1920x1080"],
                                          variable=self.config['camera_resolution'],
                                          command=self.update_resolution_label)
        resolution_menu.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        self.resolution_label.pack(pady=int(5 * self.scale_factor))
        
        # Drowsiness Configuration Frame
        drowsiness_frame = ctk.CTkFrame(self.main_frame)
        drowsiness_frame.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        ctk.CTkLabel(drowsiness_frame, text="Drowsiness Configuration", font=("Arial", int(16 * self.scale_factor), "bold")).pack(pady=int(10 * self.scale_factor))
        
        # Drowsy Threshold
        ctk.CTkLabel(drowsiness_frame, text="Drowsy Alert Threshold (seconds):", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        drowsy_entry = ctk.CTkEntry(drowsiness_frame, textvariable=self.config['drowsy_threshold'])
        drowsy_entry.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Emergency Threshold
        ctk.CTkLabel(drowsiness_frame, text="Emergency Threshold (seconds):", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        emergency_entry = ctk.CTkEntry(drowsiness_frame, textvariable=self.config['emergency_threshold'])
        emergency_entry.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Awake Reset Time
        ctk.CTkLabel(drowsiness_frame, text="Awake Reset Time (seconds):", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        awake_entry = ctk.CTkEntry(drowsiness_frame, textvariable=self.config['awake_reset_time'])
        awake_entry.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Blink Threshold
        ctk.CTkLabel(drowsiness_frame, text="Blink Threshold (seconds):", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        blink_threshold_entry = ctk.CTkEntry(drowsiness_frame, textvariable=self.config['blink_threshold'])
        blink_threshold_entry.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Blink Window
        ctk.CTkLabel(drowsiness_frame, text="Blink Window (seconds):", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        blink_window_entry = ctk.CTkEntry(drowsiness_frame, textvariable=self.config['blink_window'])
        blink_window_entry.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Theme Configuration Frame
        theme_frame = ctk.CTkFrame(self.main_frame)
        theme_frame.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        ctk.CTkLabel(theme_frame, text="Theme Configuration", font=("Arial", int(16 * self.scale_factor), "bold")).pack(pady=int(10 * self.scale_factor))
        
        # Theme Mode
        ctk.CTkLabel(theme_frame, text="Theme Mode:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        theme_mode_menu = ctk.CTkOptionMenu(theme_frame,
                                          values=["light", "dark", "system"],
                                          variable=self.config['theme_mode'])
        theme_mode_menu.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Theme Color
        ctk.CTkLabel(theme_frame, text="Theme Color:", font=("Arial", int(16 * self.scale_factor))).pack(pady=int(5 * self.scale_factor))
        theme_color_menu = ctk.CTkOptionMenu(theme_frame,
                                           values=["blue", "green", "dark-blue"],
                                           variable=self.config['theme_color'])
        theme_color_menu.pack(fill="x", padx=int(20 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Start Button
        start_button = ctk.CTkButton(self.main_frame, text="Start Program", font=("Arial", int(16 * self.scale_factor)), command=self.start_program)
        start_button.pack(pady=int(20 * self.scale_factor))
    
    def update_confidence_label(self, value):
        self.confidence_label.configure(text=f"Confidence: {value:.2f}")
    
    def update_overlap_label(self, value):
        self.overlap_label.configure(text=f"Overlap: {value:.2f}")
    
    def update_resolution_label(self, value):
        self.resolution_label.configure(text=f"Resolution: {value}")
        
    def get_available_cameras(self):
        """Get list of available cameras"""
        available_cameras = []
        for i in range(10):  # Check first 10 indices
            cap = cv2.VideoCapture(i)
            if cap.isOpened():
                available_cameras.append(i)
                cap.release()
        return available_cameras if available_cameras else [0]  # Return at least one camera
    
    def start_program(self):
        """Start the main program with selected configurations"""
        # Apply theme settings
        ctk.set_appearance_mode(self.config['theme_mode'].get())
        ctk.set_default_color_theme(self.config['theme_color'].get())
        
        # Close configuration window
        self.root.destroy()
        
        # Start main program
        root = ctk.CTk()
        app = VehicleSystem(root, self.config)
        root.protocol("WM_DELETE_WINDOW", app.on_closing)
        root.mainloop()

class VehicleSystem:
    def __init__(self, root, config):
        self.root = root
        self.config = config
        self.root.title("Vehicle Control and Monitoring System")
        
        # Get screen dimensions
        self.screen_width = root.winfo_screenwidth()
        self.screen_height = root.winfo_screenheight()
        
        # Calculate window dimensions (90% of screen)
        self.window_width = int(self.screen_width * 0.9)
        self.window_height = int(self.screen_height * 0.9)
        
        # Set window size and minimum size
        self.root.geometry(f"{self.window_width}x{self.window_height}")
        self.root.minsize(int(self.screen_width * 0.7), int(self.screen_height * 0.7))
        
        # Calculate scaling factors for fonts and widgets
        self.scale_factor = min(self.window_width / 1600, self.window_height / 900)
        self.title_font = ("Arial", int(20 * self.scale_factor), "bold")
        self.normal_font = ("Arial", int(14 * self.scale_factor))
        self.small_font = ("Arial", int(12 * self.scale_factor))
        
        # Initialize systems
        self.initialize_control_system()
        self.initialize_monitor_system()
        self.initialize_drowsiness_system()
        
        # Create notebook for tabs
        self.tabview = ctk.CTkTabview(self.root)
        self.tabview.pack(fill=tk.BOTH, expand=True, padx=int(10 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        # Add tabs
        self.control_tab = self.tabview.add("Control")
        self.monitor_tab = self.tabview.add("Monitor")
        
        # Setup UI for both systems
        self.setup_control_ui()
        self.setup_monitor_ui()
        
        # Start threads
        self.start_threads()
        
        # Bind keyboard events
        self.root.bind("<KeyPress>", self.key_press)
        self.root.bind("<KeyRelease>", self.key_release)
        self.root.bind("<MouseWheel>", self.on_mouse_wheel)
        
        # Bind window resize event
        self.root.bind("<Configure>", self.on_window_resize)
        
        # Set up theme change monitoring
        self.last_theme = ctk.get_appearance_mode()
        self.root.after(100, self.check_theme_change)
    
    def check_theme_change(self):
        """Check for theme changes and update UI accordingly"""
        current_theme = self.config['theme_mode'].get()
        if current_theme != self.last_theme:
            self.last_theme = current_theme
            self.update_theme_colors()
        self.root.after(100, self.check_theme_change)
    
    def update_theme_colors(self):
        """Update all theme-dependent colors"""
        is_dark = self.config['theme_mode'].get() == "dark"
        
        # Update steering wheel canvas background
        if hasattr(self, 'steering_wheel_canvas'):
            self.steering_wheel_canvas.configure(bg="#2b2b2b" if is_dark else "#d3d3d3")
            self.draw_steering_wheel()
        
        # Update monitoring graph colors
        if hasattr(self, 'fig'):
            self.update_plot_colors()
    
    def update_plot_colors(self):
        """Update plot colors based on current theme"""
        if hasattr(self, 'fig'):
            # Get current theme colors
            is_dark = self.config['theme_mode'].get() == "dark"
            bg_color = "#2b2b2b" if is_dark else "#d3d3d3"
            text_color = "white" if is_dark else "#000000"
            grid_color = "#cccccc" if is_dark else "#404040"
            
            # Update figure colors
            self.fig.patch.set_facecolor(bg_color)
            
            # Update all axes
            for ax in self.fig.axes:
                ax.set_facecolor(bg_color)
                ax.tick_params(colors=text_color)
                ax.xaxis.label.set_color(text_color)
                ax.yaxis.label.set_color(text_color)
                ax.grid(True, color=grid_color, linestyle='--', alpha=0.3)
                
                # Update title color
                if ax.get_title():
                    ax.set_title(ax.get_title(), color=text_color)
                
                # Update spine colors
                for spine in ax.spines.values():
                    spine.set_color(text_color)
            
            # Update canvas
            self.canvas.draw()
    
    def draw_steering_wheel(self, visual_angle=None):
        """Draw the steering wheel with theme-appropriate colors"""
        if not hasattr(self, 'steering_wheel_canvas'):
            return
            
        # Clear canvas
        self.steering_wheel_canvas.delete("all")
        
        # Get current theme colors
        is_dark = self.config['theme_mode'].get() == "dark"
        wheel_color = "#cccccc" if is_dark else "#404040"
        rim_color = "#2b2b2b" if is_dark else "#d3d3d3"
        text_color = "white" if is_dark else "#000000"
        
        # Get canvas dimensions
        width = self.steering_wheel_canvas.winfo_width()
        height = self.steering_wheel_canvas.winfo_height()
        center_x = width // 2
        center_y = height // 2
        
        # Calculate wheel dimensions
        outer_radius = min(width, height) // 2 - 10
        inner_radius = outer_radius - 20
        
        # Draw outer rim
        self.steering_wheel_canvas.create_oval(
            center_x - outer_radius, center_y - outer_radius,
            center_x + outer_radius, center_y + outer_radius,
            outline=wheel_color, width=5, fill=rim_color
        )
        
        # Draw inner rim
        self.steering_wheel_canvas.create_oval(
            center_x - inner_radius, center_y - inner_radius,
            center_x + inner_radius, center_y + inner_radius,
            outline=wheel_color, width=3, fill=rim_color
        )
        
        # Draw spokes
        for angle in range(0, 360, 45):
            rad = math.radians(angle)
            x1 = center_x + inner_radius * math.cos(rad)
            y1 = center_y + inner_radius * math.sin(rad)
            x2 = center_x + outer_radius * math.cos(rad)
            y2 = center_y + outer_radius * math.sin(rad)
            self.steering_wheel_canvas.create_line(
                x1, y1, x2, y2,
                fill=wheel_color, width=3
            )
        
        # Draw center hub
        hub_radius = inner_radius // 2
        self.steering_wheel_canvas.create_oval(
            center_x - hub_radius, center_y - hub_radius,
            center_x + hub_radius, center_y + hub_radius,
            outline=wheel_color, width=2, fill=rim_color
        )
        
        # Draw visual angle indicator if provided
        if visual_angle is not None:
            rad = math.radians(visual_angle)
            indicator_length = outer_radius + 10
            x = center_x + indicator_length * math.cos(rad)
            y = center_y + indicator_length * math.sin(rad)
            self.steering_wheel_canvas.create_line(
                center_x, center_y, x, y,
                fill="red", width=3, arrow="last"
            )
        
        # Draw angle text
        angle_text = f"{visual_angle:.1f}°" if visual_angle is not None else "0.0°"
        self.steering_wheel_canvas.create_text(
            center_x, center_y,
            text=angle_text,
            fill=text_color,
            font=("Arial", int(14 * self.scale_factor), "bold")
        )

    def on_window_resize(self, event):
        """Handle window resize events"""
        if event.widget == self.root:
            # Update window dimensions
            self.window_width = event.width
            self.window_height = event.height
            
            # Recalculate scale factor
            self.scale_factor = min(self.window_width / 1600, self.window_height / 900)
            
            # Update fonts
            self.title_font = ("Arial", int(20 * self.scale_factor), "bold")
            self.normal_font = ("Arial", int(14 * self.scale_factor))
            self.small_font = ("Arial", int(12 * self.scale_factor))
            
            # Update widget sizes and positions
            self.update_widget_sizes()
            self.update_all_fonts()

    def update_all_fonts(self):
        """Update fonts for all widgets"""
        # Update all CTk widgets
        for widget in self.root.winfo_children():
            self._update_widget_fonts(widget)

    def _update_widget_fonts(self, widget):
        """Recursively update fonts for a widget and its children"""
        # Update current widget's font if it has one
        if hasattr(widget, 'cget') and 'font' in widget.keys():
            current_font = widget.cget('font')
            if isinstance(current_font, tuple):
                # If it's a tuple font (family, size, style)
                if len(current_font) == 3 and current_font[0] == "Arial":
                    # Scale the size based on the original font size
                    if current_font[1] == 20:  # Title font
                        widget.configure(font=self.title_font)
                    elif current_font[1] == 14:  # Normal font
                        widget.configure(font=self.normal_font)
                    elif current_font[1] == 12:  # Small font
                        widget.configure(font=self.small_font)
                else:
                    # For other fonts, scale the size
                    new_size = int(current_font[1] * self.scale_factor)
                    widget.configure(font=(current_font[0], new_size, *current_font[2:]))

        # Update all children
        for child in widget.winfo_children():
            self._update_widget_fonts(child)

    def update_widget_sizes(self):
        """Update widget sizes based on new window dimensions"""
        # Update steering wheel canvas
        if hasattr(self, 'steering_wheel_canvas'):
            canvas_size = int(min(self.window_width, self.window_height) * 0.3)
            self.steering_wheel_canvas.configure(width=canvas_size, height=canvas_size)
            self.draw_steering_wheel()
        
        # Update acceleration controls
        if hasattr(self, 'accel_bar'):
            accel_width = int(self.window_width * 0.25)
            self.accel_bar.configure(width=accel_width)
            self.accel_slider.configure(width=accel_width)
        
        # Update custom buttons
        if hasattr(self, 'custom_buttons'):
            button_width = int(self.window_width * 0.1)
            button_height = int(self.window_height * 0.05)
            for btn in self.custom_buttons:
                btn.configure(width=button_width, height=button_height)
        
        # Update camera display
        if hasattr(self, 'driver_cam_label'):
            cam_width = int(self.window_width * 0.45)
            cam_height = int(self.window_height * 0.3)
            self.driver_cam_label.configure(width=cam_width, height=cam_height)
        
        # Update graph size
        if hasattr(self, 'fig'):
            self.fig.set_size_inches(self.window_width/100, self.window_height/100)
            self.canvas.draw()

    def get_current_location(self):
        """Get approximate location based on public IP"""
        try:
            # Get public IP
            public_ip = requests.get('https://api.ipify.org').text
            
            # Get location data
            response = requests.get(f'http://ip-api.com/json/{public_ip}')
            location_data = response.json()
            
            if location_data['status'] == 'success':
                return (
                    f"Approximate Location: {location_data['city']}, {location_data['regionName']}, {location_data['country']}\n"
                    f"Coordinates: {location_data['lat']}, {location_data['lon']}\n"
                    f"Google Maps: https://maps.google.com/?q={location_data['lat']},{location_data['lon']}"
                )
            return "Location: Could not determine exact location (IP-based approximation failed)"
        except Exception as e:
            return f"Location: Could not determine location ({str(e)})"

    def send_emergency_email(self, location_info):
        """Send emergency email with location information"""
        try:
            # Email configuration
            sender_email = "ahmedmessi975@gmail.com"  # Your Gmail
            receiver_email = self.emergency_email_var.get()  # Emergency contact
            password = "hrbf hlso rqfc jbhf"  # Use App Password
            subject = "🚨 EMERGENCY ALERT 🚨"

            body = f"""
URGENT: This is an automated emergency alert.

Someone lost consciousness while driving.
{location_info}

Please take immediate action.
"""

            # Create and send the email
            message = MIMEMultipart()
            message["From"] = sender_email
            message["To"] = receiver_email
            message["Subject"] = subject
            message.attach(MIMEText(body, "plain"))

            with smtplib.SMTP("smtp.gmail.com", 587) as server:
                server.starttls()
                server.login(sender_email, password)
                server.sendmail(sender_email, receiver_email, message.as_string())
            print("🆘 Emergency email sent successfully!")
        except Exception as e:
            print(f"❌ Failed to send email: {e}")

    def initialize_drowsiness_system(self):
        """Initialize drowsiness detection system with threading"""
        self.start = 0
        self.fps = 0
        # Drowsiness detection variables
        self.drowsy_detected = False
        self.drowsy_start_time = 0
        self.alarm_playing = False
        self.emergency_triggered = False
        self.emergency_cooldown = False
        self.awake_time = 0
        self.last_drowsy_state = False
        self.latest_frame = None
        self.frame_ready = Event()
        self.stop_threads = False
        self.emergency_email_var = tk.StringVar(value="ahmedhaniatef@gmail.com")  # Default emergency email
        
        # Blink detection variables
        self.blink_start_time = None
        self.blink_count = 0
        self.last_blink_time = 0
        self.blink_window = 3.0  # Time window to count blinks (seconds)
        self.blink_threshold = 10  # Number of blinks in window to trigger drowsiness
        self.blink_duration_threshold = 1.2  # Duration threshold for drowsy blink (seconds)
        
        # Initialize pygame for alarm sound
        mixer.init()
        try:
            self.alarm_sound = mixer.Sound('alarm.wav')
        except:
            print("Warning: Could not load alarm.wav")
            self.alarm_sound = None
        
        # Initialize YOLO model with configuration settings
        try:
            model_name = self.config['model_name'].get()
            self.drowsy_model = YOLO(model_name, 'detect', True).to('cuda' if torch.cuda.is_available() else 'cpu')
            self.drowsy_model.fuse()
            
            # Set model parameters from configuration
            self.drowsy_model.conf = self.config['model_confidence'].get()
            self.drowsy_model.iou = self.config['model_overlap'].get()
            
            # Warmup model
            dummy = torch.zeros(1, 3, 1024, 1024).to('cuda' if torch.cuda.is_available() else 'cpu')
            for _ in range(3):
                _ = self.drowsy_model(dummy)
        except Exception as e:
            print(f"Failed to initialize YOLO model: {e}")
            self.drowsy_model = None
        
        # Initialize camera with configured settings
        try:
            camera_index = self.config['camera_index'].get()
            self.cap = cv2.VideoCapture(camera_index)
            
            # Parse resolution from configuration
            width, height = map(int, self.config['camera_resolution'].get().split('x'))
            self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
            self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
            
            # Get actual resolution
            self.camera_width = int(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
            self.camera_height = int(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        except Exception as e:
            print(f"Failed to initialize camera: {e}")
            self.cap = None
        
        # Start processing threads if we have a camera and model
        if self.cap is not None and self.drowsy_model is not None:
            self.camera_thread = threading.Thread(target=self.camera_capture_thread, daemon=True)
            self.detection_thread = threading.Thread(target=self.detection_thread, daemon=True)
            self.monitoring_thread = threading.Thread(target=self.monitoring_thread, daemon=True)
            
            self.camera_thread.start()
            self.detection_thread.start()
            self.monitoring_thread.start()
    
    def camera_capture_thread(self):
        """Thread to continuously capture camera frames"""
        while not self.stop_threads and self.cap is not None:
            ret, frame = self.cap.read()
            if ret:
                self.latest_frame = frame
                self.frame_ready.set()
            time.sleep(0.005)

    def detection_thread(self):
        """Thread to process frames with YOLO"""
        while not self.stop_threads and self.drowsy_model is not None:
            if self.frame_ready.wait(timeout=0.5) and self.latest_frame is not None:
                frame = self.latest_frame.copy()
                self.frame_ready.clear()
                
                try:
                    self.start = time.time()

                    # Process frame
                    results = self.drowsy_model(frame, imgsz=int(self.config['image_size'].get()), half=True, conf=self.config['model_confidence'].get(), iou=self.config['model_overlap'].get() , device='cuda' if torch.cuda.is_available() else 'cpu')
                    
                    self.fps = 1/(time.time()-self.start)
        
                    # Display current parameters
                    param_text = f"FPS: {self.fps:.1f}"
                    
                    # Process results
                    detections = results[0].boxes.data.cpu().numpy()
                    current_drowsy = False
                    current_time = time.time()
                    
                    # Draw boxes and check for drowsiness
                    for det in detections:
                        x1, y1, x2, y2 = map(int, det[:4])
                        conf = float(det[4])
                        cls = int(det[5])
                        
                        class_name = results[0].names.get(cls, str(cls))
                        
                        # Draw bounding box and label
                        color = (0, 255, 0) if class_name.lower() != "drowsy" else (0, 0, 255)
                        cv2.rectangle(frame, (x1, y1), (x2, y2), color, 2)
                        
                        label = f"{class_name} {conf:.2f}"
                        (text_width, text_height), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
                        cv2.rectangle(frame, (x1, y1 - text_height - 10), (x1 + text_width, y1), color, -1)
                        cv2.putText(frame, label, (x1, y1 - 10), 
                                   cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1)
                        
                        # Check for drowsy class and handle blink detection
                        if class_name.lower() == "drowsy" and conf > 0.7:
                            if self.blink_start_time is None:
                                self.blink_start_time = current_time
                            else:
                                blink_duration = current_time - self.blink_start_time
                                # If blink duration exceeds threshold, consider it drowsiness
                                if blink_duration > float(self.config['blink_threshold'].get()):
                                    current_drowsy = True
                        else:
                            # If eyes are open and we were tracking a blink
                            if self.blink_start_time is not None:
                                blink_duration = current_time - self.blink_start_time
                                # If it was a normal blink (short duration)
                                if blink_duration <= float(self.config['blink_threshold'].get()):
                                    self.blink_count += 1
                                    self.last_blink_time = current_time
                                    
                                    # Check if we have too many blinks in the window
                                    if current_time - self.last_blink_time > float(self.config['blink_window'].get()):
                                        self.blink_count = 1
                                    elif self.blink_count >= 3:  # Fixed threshold for number of blinks
                                        current_drowsy = True
                                
                                self.blink_start_time = None  # Reset blink start time

                    # Reset blink count if no blinks in window
                    if current_time - self.last_blink_time > float(self.config['blink_window'].get()):
                        self.blink_count = 0
                    
                    cv2.putText(frame, param_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.55, (0, 0, 255), 2)

                    # Update drowsiness state
                    self.drowsy_detected = current_drowsy
                    
                    # Convert to PIL Image for display
                    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                    img = Image.fromarray(frame)
                    img_tk = ImageTk.PhotoImage(image=img)
                    
                    # Update GUI in main thread
                    self.root.after(0, self.update_camera_display, frame)
                except Exception as e:
                    print(f"Error in detection thread: {e}")

    def monitoring_thread(self):
        """Thread to monitor drowsiness state and trigger alerts"""
        while not self.stop_threads:
            if self.drowsy_detected:
                if not self.drowsy_start_time:
                    self.drowsy_start_time = time.time()
                else:
                    drowsy_duration = time.time() - self.drowsy_start_time
                    
                    # Update status text
                    status_text = f"Drowsy: {drowsy_duration:.1f}s"
                    if drowsy_duration >= float(self.config['drowsy_threshold'].get()):
                        status_text += " (ALARM)"
                    if drowsy_duration >= float(self.config['emergency_threshold'].get()):
                        status_text += " (EMERGENCY)"
                    
                    self.root.after(0, self.update_drowsy_status, status_text, "red")
                    
                    # Check drowsy threshold for alarm
                    if drowsy_duration >= float(self.config['drowsy_threshold'].get()) and not self.alarm_playing and not self.emergency_cooldown:
                        self.play_alarm()
                    
                    # Check emergency threshold
                    if drowsy_duration >= float(self.config['emergency_threshold'].get()) and not self.emergency_triggered and not self.emergency_cooldown:
                        self.trigger_emergency()
                        self.emergency_cooldown = True
            else:
                if self.drowsy_start_time:
                    # Only reset if awake for configured time
                    if not self.last_drowsy_state:
                        self.awake_time = time.time()
                        self.last_drowsy_state = True
                    
                    awake_duration = time.time() - self.awake_time
                    if awake_duration >= float(self.config['awake_reset_time'].get()):
                        self.drowsy_start_time = None
                        self.stop_alarm()
                        self.reset_emergency()
                        self.emergency_cooldown = False
                        self.last_drowsy_state = False
                        self.root.after(0, self.update_drowsy_status, "Status: Awake", "green")
                    else:
                        status_text = f"Awake: {awake_duration:.1f}s (need {float(self.config['awake_reset_time'].get())}s to reset)"
                        self.root.after(0, self.update_drowsy_status, status_text, "orange")
                else:
                    self.last_drowsy_state = False
            
            time.sleep(0.01)

    def update_camera_display(self, frame):
        """Update camera display in main thread"""
        if hasattr(self, 'driver_cam_label'):
            # Calculate display size based on window size
            display_width = int(self.window_width * 0.575)  # 30% of window width
            display_height = int(self.window_height * 0.425)  # 40% of window height
            
            # Resize image for display while maintaining aspect ratio
            img = Image.fromarray(frame)
            img.thumbnail((display_width, display_height), Image.Resampling.LANCZOS)
            img_tk = ImageTk.PhotoImage(image=img)
            
            # Update FPS counter
            fps_text = f"FPS: {self.fps:.1f}"
            self.driver_cam_label.configure(image=img_tk, text=fps_text, compound=tk.TOP)
            self.driver_cam_label.image = img_tk

    def update_drowsy_status(self, text, color):
        """Update drowsiness status label in main thread"""
        if hasattr(self, 'drowsy_status_var'):
            self.drowsy_status_var.set(text)
            self.drowsy_status_label.configure(text_color=color)

    def play_alarm(self):
        """Play alarm sound"""
        self.alarm_playing = True
        if self.alarm_sound:
            self.alarm_sound.play(-1)  # Loop indefinitely
        winsound.Beep(1000, 1000)  # Fallback beep

    def stop_alarm(self):
        """Stop alarm sound"""
        self.alarm_playing = False
        if self.alarm_sound:
            self.alarm_sound.stop()

    def trigger_emergency(self):
        """Trigger emergency protocol"""
        self.emergency_triggered = True
        # Press button 8 (index 7) - emergency button
        self.button_states[7] = 1
        self.update_button_appearance(7)
        self.send_button_states()

    def reset_emergency(self):
        """Reset emergency state"""
        if self.emergency_triggered:
            self.emergency_triggered = False
            # Release button 8
            self.button_states[7] = 0
            self.update_button_appearance(7)
            self.send_button_states()

    def call_emergency(self):
        """Handle emergency parking signal"""
        print("EMERGENCY PARKING ACTIVATED!")
        # Trigger alarm
        winsound.Beep(2000, 1000)  # High frequency beep for emergency
        
        # Get location and send email
        location_info = self.get_current_location()
        self.send_emergency_email(location_info)
        
        # Show emergency message
        emergency_window = ctk.CTkToplevel(self.root)
        emergency_window.title("EMERGENCY")
        emergency_window.geometry("400x200")
        emergency_window.attributes("-topmost", True)
        
        ctk.CTkLabel(
            emergency_window,
            text="EMERGENCY PARKING ACTIVATED!",
            font=("Arial", 20, "bold"),
            text_color="red"
        ).pack(pady=20)
        
        ctk.CTkButton(
            emergency_window,
            text="Acknowledge",
            command=emergency_window.destroy,
            fg_color="red",
            hover_color="#d32f2f"
        ).pack(pady=10)

    def create_driver_camera_panel(self):
        """Create panel for driver camera and drowsiness detection"""
        camera_frame = ctk.CTkFrame(self.control_right_column)
        camera_frame.pack(fill=tk.BOTH, expand=True, padx=int(10 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        # Title
        ctk.CTkLabel(
            camera_frame, 
            text="Driver Monitoring",
            font=self.title_font
        ).pack(pady=(int(5 * self.scale_factor), int(10 * self.scale_factor)))
        
        # Email configuration
        email_frame = ctk.CTkFrame(camera_frame)
        email_frame.pack(fill=tk.X, pady=int(5 * self.scale_factor))
        
        ctk.CTkLabel(email_frame, text="Emergency Email:", font=self.small_font).pack(side=tk.LEFT, padx=int(5 * self.scale_factor))
        self.emergency_email_entry = ctk.CTkEntry(email_frame, textvariable=self.emergency_email_var, 
                                                width=int(200 * self.scale_factor))
        self.emergency_email_entry.pack(side=tk.LEFT, padx=int(5 * self.scale_factor), fill=tk.X, expand=True)
        
        # Test email button
        test_email_btn = ctk.CTkButton(
            email_frame, 
            text="Test", 
            width=int(60 * self.scale_factor),
            command=lambda: self.Test_emergency()
        )
        test_email_btn.pack(side=tk.LEFT, padx=int(5 * self.scale_factor))
        
        # Camera display with FPS counter
        self.driver_cam_label = ctk.CTkLabel(
            camera_frame, 
            text="Camera not available",
            font=self.normal_font
        )
        self.driver_cam_label.pack(pady=int(5 * self.scale_factor), fill=tk.BOTH, expand=True)
        
        # Drowsiness status
        self.drowsy_status_var = tk.StringVar(value="Status: Awake")
        self.drowsy_status_label = ctk.CTkLabel(
            camera_frame, 
            textvariable=self.drowsy_status_var,
            font=self.normal_font,
            text_color="green"
        )
        self.drowsy_status_label.pack(pady=int(5 * self.scale_factor))
    
    def Test_emergency(self):
        location_info = self.get_current_location()
        self.send_emergency_email(location_info)

    def initialize_control_system(self):
        """Initialize control system variables"""
        # Serial connection for control
        self.control_ser = None
        self.control_port = "COM5"  # Default control port
        self.control_baud = 115200
        self.control_connected = False
        self.control_lock = Lock()
        
        # Control variables
        self.steering_angle = 0
        self.acceleration = 0
        self.move_direction = 0
        self.last_angle = 0
        self.wheel_dragging = False
        self.steering_mode = "steering"
        
        # State tracking
        self.last_steering_angle = 0
        self.last_move_direction = 0
        self.last_steering_mode = "steering"
        self.last_sent_command = None
        
        # Custom buttons
        self.button_names = [f"Button {i+1}" for i in range(8)]
        self.button_states = [0] * 8
        self.button_types = ["toggle"] * 8
        self.button_keys = [""] * 8
        self.load_button_config()
        self.custom_buttons = []
        
        # Drowsiness detection
        self.drowsy_detected = False
        self.drowsy_start_time = 0
        self.alarm_playing = False
        self.drowsy_model = None
        self.cap = None
        self.driver_cam_label = None
        self.drowsy_status_label = None

    def initialize_monitor_system(self):
        """Initialize monitor system variables"""
        # Serial connection for monitoring
        self.monitor_ser = None
        self.monitor_port = "COM3"  # Default monitor port
        self.monitor_baud = 9600
        self.monitor_connected = False
        self.monitor_lock = Lock()
        
        # Performance optimization flags
        self.running = True
        self.last_plot_time = 0
        self.plot_refresh_rate = 10  # Hz
        self.redundant_frames = 1
        
        # Data variables
        self.data_buffer = []
        self.raw_data_buffer = []
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
        self.display_mode = "graph"
        self.value_boxes = []
        self.value_labels = []
        self.SIGNALS_FILE = "signals.txt"
        
        # Emergency parking signal
        self.emergency_park_signal = None
        self.emergency_park_value = 0
        
        # Colors
        self.graph_bg = "#1e1e1e"
        self.graph_text = "white"
        self.graph_grid = "#3a3a3a"

    def setup_control_ui(self):
        """Setup control system UI"""
        # Connection frame
        self.control_connection_frame = ctk.CTkFrame(self.control_tab)
        self.control_connection_frame.pack(fill=tk.X, padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Control port selection
        ctk.CTkLabel(self.control_connection_frame, text="Control Port:", font=self.small_font).grid(row=0, column=0, padx=int(5 * self.scale_factor))
        
        self.control_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.control_port_var = tk.StringVar(value=self.control_port if self.control_port in self.control_ports else self.control_ports[0] if self.control_ports else "COM5")
        self.control_port_menu = ctk.CTkOptionMenu(self.control_connection_frame, variable=self.control_port_var, values=self.control_ports)
        self.control_port_menu.grid(row=0, column=1, padx=int(5 * self.scale_factor))
        
        # Refresh button
        ctk.CTkButton(self.control_connection_frame, text="↻", width=int(30 * self.scale_factor), 
                     command=self.refresh_control_ports).grid(row=0, column=2, padx=int(5 * self.scale_factor))
        
        # Connect button
        self.control_connect_btn = ctk.CTkButton(self.control_connection_frame, text="Connect", 
                                               command=self.toggle_control_connection, 
                                               width=int(100 * self.scale_factor))
        self.control_connect_btn.grid(row=0, column=3, padx=int(5 * self.scale_factor))
        
        # Status label
        self.control_status_var = tk.StringVar(value="Disconnected")
        ctk.CTkLabel(self.control_connection_frame, textvariable=self.control_status_var, 
                    text_color="red", font=self.small_font).grid(row=0, column=4, padx=int(5 * self.scale_factor))
        
        # Main control frame
        self.control_main_frame = ctk.CTkFrame(self.control_tab)
        self.control_main_frame.pack(fill=tk.BOTH, expand=True, padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Left column (controls)
        self.control_left_column = ctk.CTkFrame(self.control_main_frame)
        self.control_left_column.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Right column (buttons and camera)
        self.control_right_column = ctk.CTkFrame(self.control_main_frame)
        self.control_right_column.pack(side=tk.RIGHT, fill=tk.BOTH, padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Title label
        ctk.CTkLabel(self.control_left_column, text="Vehicle Control", font=self.title_font).pack(pady=(int(10 * self.scale_factor), int(20 * self.scale_factor)))
        
        # Steering wheel canvas
        self.steering_wheel_canvas = tk.Canvas(self.control_left_column, width=300, height=300, bg="#2b2b2b" if self.config['theme_mode'].get() == "dark" else "#d3d3d3", highlightthickness=0)
        self.steering_wheel_canvas.pack(pady=int(10 * self.scale_factor))
        self.draw_steering_wheel()
        
        # Mouse bindings
        self.steering_wheel_canvas.bind("<Button-1>", self.start_wheel_drag)
        self.steering_wheel_canvas.bind("<B1-Motion>", self.rotate_wheel_drag)
        self.steering_wheel_canvas.bind("<ButtonRelease-1>", self.stop_wheel_drag)
        
        # Mode button
        self.mode_btn = ctk.CTkButton(
            self.control_left_column,
            text="Steering (Q)",
            command=self.toggle_steering_mode,
            width=int(120 * self.scale_factor),
            height=int(40 * self.scale_factor),
            fg_color="#9C27B0",
            hover_color="#7B1FA2"
        )
        self.mode_btn.pack(pady=int(10 * self.scale_factor))
        
        # Movement controls
        self.create_movement_controls()
        
        # Acceleration control
        self.create_acceleration_controls()
        
        # Status display
        self.create_control_status_display()
        
        # Custom buttons
        self.create_custom_buttons_panel()
        
        # Driver camera and drowsiness detection
        self.create_driver_camera_panel()

    def create_movement_controls(self):
        """Create movement control buttons"""
        movement_frame = ctk.CTkFrame(self.control_left_column)
        movement_frame.pack(pady=int(10 * self.scale_factor))
        
        self.forward_btn = ctk.CTkButton(
            movement_frame,
            text="Forward (W)",
            command=lambda: self.set_movement(1),
            width=int(120 * self.scale_factor),
            height=int(40 * self.scale_factor),
            fg_color="#4CAF50",
            hover_color="#45a049"
        )
        self.forward_btn.pack(pady=int(5 * self.scale_factor))
        
        self.stop_btn = ctk.CTkButton(
            movement_frame,
            text="Stop",
            command=lambda: self.set_movement(0),
            width=int(120 * self.scale_factor),
            height=int(40 * self.scale_factor),
            fg_color="#616161",
            hover_color="#424242"
        )
        self.stop_btn.pack(pady=int(5 * self.scale_factor))
        
        self.backward_btn = ctk.CTkButton(
            movement_frame,
            text="Backward (S)",
            command=lambda: self.set_movement(-1),
            width=int(120 * self.scale_factor),
            height=int(40 * self.scale_factor),
            fg_color="#F44336",
            hover_color="#d32f2f"
        )
        self.backward_btn.pack(pady=int(5 * self.scale_factor))

    def create_acceleration_controls(self):
        """Create acceleration controls"""
        accel_frame = ctk.CTkFrame(self.control_left_column)
        accel_frame.pack(pady=int(20 * self.scale_factor), fill=tk.X, padx=int(50 * self.scale_factor))
        
        ctk.CTkLabel(accel_frame, text="ACCELERATOR LEVEL:", font=self.normal_font).pack(pady=(int(10 * self.scale_factor), int(5 * self.scale_factor)))
        
        self.accel_bar = ctk.CTkProgressBar(
            accel_frame,
            orientation="horizontal",
            width=int(400 * self.scale_factor),
            height=int(30 * self.scale_factor),
            corner_radius=int(15 * self.scale_factor),
            progress_color="green",
            mode="determinate"
        )
        self.accel_bar.set(0)
        self.accel_bar.pack(pady=int(10 * self.scale_factor))
        
        self.accel_slider = ctk.CTkSlider(
            accel_frame,
            from_=0,
            to=100,
            command=self.update_acceleration,
            width=int(400 * self.scale_factor),
            height=int(15 * self.scale_factor),
            button_color="#2196F3",
            button_hover_color="#1976D2"
        )
        self.accel_slider.set(0)
        self.accel_slider.pack(pady=(0, int(10 * self.scale_factor)))

    def create_control_status_display(self):
        """Create status display for control system"""
        status_frame = ctk.CTkFrame(self.control_left_column)
        status_frame.pack(pady=int(10 * self.scale_factor), fill=tk.X, padx=int(50 * self.scale_factor))
        
        self.steering_var = tk.StringVar(value="Steering Angle: 0°")
        ctk.CTkLabel(status_frame, textvariable=self.steering_var, font=self.normal_font, text_color="#2196F3").pack(side=tk.LEFT, padx=int(20 * self.scale_factor))
        
        self.accel_var = tk.StringVar(value="Acceleration: 0%")
        ctk.CTkLabel(status_frame, textvariable=self.accel_var, font=self.normal_font, text_color="#4CAF50").pack(side=tk.LEFT, padx=int(20 * self.scale_factor))
        
        self.movement_var = tk.StringVar(value="Movement: Stopped")
        ctk.CTkLabel(status_frame, textvariable=self.movement_var, font=self.normal_font, text_color="#FF9800").pack(side=tk.LEFT, padx=int(20 * self.scale_factor))
        
        self.mode_var = tk.StringVar(value="Mode: Steering")
        ctk.CTkLabel(status_frame, textvariable=self.mode_var, font=self.normal_font, text_color="#9C27B0").pack(side=tk.LEFT, padx=int(20 * self.scale_factor))

    def create_custom_buttons_panel(self):
        """Create panel for custom buttons"""
        custom_buttons_frame = ctk.CTkFrame(self.control_right_column)
        custom_buttons_frame.pack(fill=tk.BOTH, expand=True, padx=int(10 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        ctk.CTkLabel(
            custom_buttons_frame, 
            text="Custom Buttons",
            font=self.title_font
        ).pack(pady=(int(5 * self.scale_factor), int(10 * self.scale_factor)))
        
        # Configure button
        self.configure_btn = ctk.CTkButton(
            custom_buttons_frame,
            text="Configure Buttons",
            command=self.configure_buttons,
            width=int(150 * self.scale_factor),
            height=int(30 * self.scale_factor),
            fg_color="#607D8B",
            hover_color="#455A64"
        )
        self.configure_btn.pack(pady=(0, int(10 * self.scale_factor)))
        
        self.create_custom_buttons()

    def setup_monitor_ui(self):
        """Setup monitor system UI"""
        # Connection frame
        self.monitor_connection_frame = ctk.CTkFrame(self.monitor_tab)
        self.monitor_connection_frame.pack(fill=tk.X, padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Monitor port selection
        ctk.CTkLabel(self.monitor_connection_frame, text="Monitor Port:", 
                    font=self.small_font).grid(row=0, column=0, padx=int(5 * self.scale_factor))
        
        self.monitor_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.monitor_port_var = tk.StringVar(value=self.monitor_port if self.monitor_port in self.monitor_ports else self.monitor_ports[0] if self.monitor_ports else "COM3")
        self.monitor_port_menu = ctk.CTkOptionMenu(self.monitor_connection_frame, variable=self.monitor_port_var, values=self.monitor_ports)
        self.monitor_port_menu.grid(row=0, column=1, padx=int(5 * self.scale_factor))
        
        # Refresh button
        ctk.CTkButton(self.monitor_connection_frame, text="↻", width=int(30 * self.scale_factor), 
                     command=self.refresh_monitor_ports).grid(row=0, column=2, padx=int(5 * self.scale_factor))
        
        # Connect button
        self.monitor_connect_btn = ctk.CTkButton(self.monitor_connection_frame, text="Connect", 
                                               command=self.toggle_monitor_connection, 
                                               width=int(100 * self.scale_factor))
        self.monitor_connect_btn.grid(row=0, column=3, padx=int(5 * self.scale_factor))
        
        # Status label
        self.monitor_status_var = tk.StringVar(value="Disconnected")
        ctk.CTkLabel(self.monitor_connection_frame, textvariable=self.monitor_status_var, 
                    text_color="red", font=self.small_font).grid(row=0, column=4, padx=int(5 * self.scale_factor))
        
        # Display mode toggle
        self.display_mode_var = tk.IntVar(value=1)
        ctk.CTkRadioButton(self.monitor_connection_frame, text="Graph", variable=self.display_mode_var, value=1, command=self.toggle_display_mode).grid(row=0, column=5, padx=(int(20 * self.scale_factor),int(5 * self.scale_factor)), pady=int(5 * self.scale_factor))
        ctk.CTkRadioButton(self.monitor_connection_frame, text="Text", variable=self.display_mode_var, value=0, command=self.toggle_display_mode).grid(row=0, column=6, padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Main monitor frame
        self.monitor_main_frame = ctk.CTkFrame(self.monitor_tab)
        self.monitor_main_frame.pack(fill=tk.BOTH, expand=True, padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        self.monitor_main_frame.grid_rowconfigure(0, weight=1)
        self.monitor_main_frame.grid_columnconfigure(0, weight=1)
        self.monitor_main_frame.grid_columnconfigure(1, weight=0)
        self.monitor_main_frame.grid_columnconfigure(2, weight=0)
        
        # Graph frame
        self.create_monitor_graph_frame()
        
        # Control panel
        self.create_monitor_control_panel()
        
        # Values frame
        self.create_monitor_values_frame()
        
        # Load saved signals
        self.load_signals()

    def create_monitor_graph_frame(self):
        """Create the graph display frame"""
        self.graph_frame = ctk.CTkFrame(self.monitor_main_frame)
        self.graph_frame.grid(row=0, column=0, sticky="nsew", padx=(int(10 * self.scale_factor),0), pady=int(10 * self.scale_factor))
        self.graph_frame.grid_rowconfigure(0, weight=1)
        self.graph_frame.grid_columnconfigure(0, weight=1)
        
        # Create a container frame specifically for the canvas and toolbar
        self.plot_container = ctk.CTkFrame(self.graph_frame)
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

    def create_monitor_control_panel(self):
        """Create the right-side control panel"""
        control_panel = ctk.CTkFrame(self.monitor_main_frame, width=int(280 * self.scale_factor))
        control_panel.grid(row=0, column=1, sticky="ns", padx=(0,int(10 * self.scale_factor)), pady=int(10 * self.scale_factor))
        control_panel.grid_propagate(False)
        
        # Title
        ctk.CTkLabel(control_panel, text="Control Panel", 
                    font=self.normal_font).pack(pady=(int(10 * self.scale_factor), int(5 * self.scale_factor)))
        
        # Separator
        ctk.CTkFrame(control_panel, height=int(2 * self.scale_factor), fg_color="#555555").pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Signal controls frame
        signal_controls = ctk.CTkFrame(control_panel, fg_color="transparent")
        signal_controls.pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Manage signals button
        ctk.CTkButton(signal_controls, text="Manage Signals", 
                     command=self.list_signals).pack(fill="x", pady=int(5 * self.scale_factor))
        
        # Active signals label
        ctk.CTkLabel(signal_controls, text="Active Signals:", font=self.normal_font).pack(anchor="w", pady=(int(10 * self.scale_factor),int(5 * self.scale_factor)))
        
        # Signals frame with scrollbar
        self.signals_frame = ctk.CTkScrollableFrame(signal_controls, height=int(200 * self.scale_factor))
        self.signals_frame.pack(fill="both", expand=True, pady=int(5 * self.scale_factor))
        
        # Separator
        ctk.CTkFrame(control_panel, height=int(2 * self.scale_factor), fg_color="#555555").pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Time step controls
        time_frame = ctk.CTkFrame(control_panel, fg_color="transparent")
        time_frame.pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        ctk.CTkLabel(time_frame, text="Time Step (s):", font=self.small_font).pack(side="left", padx=int(5 * self.scale_factor))
        
        self.time_step_entry = ctk.CTkEntry(time_frame, width=int(80 * self.scale_factor))
        self.time_step_entry.insert(0, str(self.update_interval))
        self.time_step_entry.pack(side="left", padx=int(5 * self.scale_factor))
        
        ctk.CTkButton(time_frame, text="Set", width=int(50 * self.scale_factor),
                     command=self.set_time_step).pack(side="left", padx=int(5 * self.scale_factor))
        
        # Separator
        ctk.CTkFrame(control_panel, height=int(2 * self.scale_factor), fg_color="#555555").pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Navigation controls
        nav_frame = ctk.CTkFrame(control_panel, fg_color="transparent")
        nav_frame.pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        self.navigation_button = ctk.CTkButton(nav_frame, text="Enable Navigation", 
                                             command=self.toggle_navigation)
        self.navigation_button.pack(fill="x", pady=int(5 * self.scale_factor))

    def create_monitor_values_frame(self):
        """Create frame to display values at paused time"""
        self.values_frame = ctk.CTkFrame(self.monitor_main_frame, width=int(200 * self.scale_factor))
        self.values_frame.grid(row=0, column=2, sticky="ns", padx=(0,int(10 * self.scale_factor)), pady=int(10 * self.scale_factor))
        self.values_frame.grid_propagate(False)
        self.values_frame.grid_remove()  # Start hidden
        
        # Title
        ctk.CTkLabel(self.values_frame, text="Paused Values", 
                    font=self.normal_font).pack(pady=(int(10 * self.scale_factor), int(5 * self.scale_factor)))
        
        # Separator
        ctk.CTkFrame(self.values_frame, height=int(2 * self.scale_factor), fg_color="#555555").pack(fill="x", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Container for values
        self.values_container = ctk.CTkScrollableFrame(self.values_frame, fg_color="transparent")
        self.values_container.pack(fill="both", expand=True, padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))
        
        # Time display
        self.paused_time_label = ctk.CTkLabel(self.values_frame, text="Time: 0.00s", 
                                            font=self.normal_font, text_color="#3a7ebf")
        self.paused_time_label.pack(side="bottom", pady=int(5 * self.scale_factor))

    def refresh_control_ports(self):
        """Refresh available control ports"""
        self.control_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.control_port_menu.configure(values=self.control_ports)
        if self.control_ports:
            self.control_port_var.set(self.control_ports[0])

    def refresh_monitor_ports(self):
        """Refresh available monitor ports"""
        self.monitor_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.monitor_port_menu.configure(values=self.monitor_ports)
        if self.monitor_ports:
            self.monitor_port_var.set(self.monitor_ports[0])

    def toggle_control_connection(self):
        """Toggle control serial connection"""
        if not self.control_connected:
            self.connect_control_serial()
        else:
            self.disconnect_control_serial()

    def toggle_monitor_connection(self):
        """Toggle monitor serial connection"""
        if not self.monitor_connected:
            self.connect_monitor_serial()
        else:
            self.disconnect_monitor_serial()

    def connect_control_serial(self):
        """Establish control serial connection"""
        try:
            with self.control_lock:
                self.control_ser = serial.Serial(
                    port=self.control_port_var.get(),
                    baudrate=self.control_baud,
                    timeout=1
                )
                self.control_connected = True
                self.control_status_var.set("Connected")
                self.control_connect_btn.configure(text="Disconnect")
                print(f"Control connected to {self.control_port_var.get()}")
        except Exception as e:
            print(f"Control connection failed: {str(e)}")
            self.control_status_var.set("Connection Failed")

    def connect_monitor_serial(self):
        """Establish monitor serial connection"""
        try:
            with self.monitor_lock:
                self.monitor_ser = serial.Serial(
                    port=self.monitor_port_var.get(),
                    baudrate=self.monitor_baud,
                    timeout=1
                )
                self.monitor_connected = True
                self.monitor_status_var.set("Connected")
                self.monitor_connect_btn.configure(text="Disconnect")
                print(f"Monitor connected to {self.monitor_port_var.get()}")
        except Exception as e:
            print(f"Monitor connection failed: {str(e)}")
            self.monitor_status_var.set("Connection Failed")

    def disconnect_control_serial(self):
        """Close control serial connection"""
        with self.control_lock:
            if self.control_ser and self.control_ser.is_open:
                self.control_ser.close()
            self.control_connected = False
        self.control_status_var.set("Disconnected")
        self.control_connect_btn.configure(text="Connect")
        print("Control disconnected")

    def disconnect_monitor_serial(self):
        """Close monitor serial connection"""
        with self.monitor_lock:
            if self.monitor_ser and self.monitor_ser.is_open:
                self.monitor_ser.close()
            self.monitor_connected = False
        self.monitor_status_var.set("Disconnected")
        self.monitor_connect_btn.configure(text="Connect")
        print("Monitor disconnected")

    def control_serial_monitor(self):
        """Monitor incoming control serial data"""
        while True:
            if self.control_connected:
                try:
                    with self.control_lock:
                        if self.control_ser and self.control_ser.in_waiting:
                            data = self.control_ser.readline().decode('utf-8').strip()
                            print(f"Control received: {data}")
                except Exception as e:
                    print(f"Control serial read error: {str(e)}")
                    self.disconnect_control_serial()
            time.sleep(0.1)

    def monitor_serial_monitor(self):
        """Monitor incoming monitor serial data"""
        sync_bytes = b'\xF0\xF0\xF0\xF0'
        frame_size = 4 * self.num_signals + 4  # 4 bytes per float + 4 bytes CRC

        while self.running and self.monitor_connected:
            try:
                with self.monitor_lock:
                    if not (self.monitor_ser and self.monitor_ser.is_open):
                        continue
                        
                    # Wait for sync bytes
                    sync_received = False
                    while not sync_received and self.running:
                        if self.monitor_ser.in_waiting >= 4:
                            received = self.monitor_ser.read(4)
                            if received == sync_bytes:
                                sync_received = True
                            else:
                                continue

                    # Wait for full frame
                    while self.monitor_ser.in_waiting < frame_size and self.running:
                        time.sleep(0.0001)

                    # Read frame
                    raw_data = self.monitor_ser.read(frame_size)
                    
                    # Process frame
                    self.process_monitor_data(raw_data)
                    
            except (serial.SerialException, OSError) as e:
                if self.running:
                    print(f"Monitor serial error: {e}")
                break
            except Exception as e:
                if self.running:
                    print(f"Monitor unexpected error: {e}")
                break

    def process_monitor_data(self, raw_data):
        """Process received monitor data"""
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
            timestamp = time.time() - self.start_time
            
            # Check for emergency park signal (0x0A)
            for i, signal_name in enumerate(self.signals):
                if "emergency_park" in signal_name.lower() and values[i] == 0x0A:
                    self.call_emergency()
            
            # Update text display
            if self.display_mode == "text":
                for i, value in enumerate(values):
                    if i < len(self.value_boxes):
                        self.value_boxes[i].configure(text=f"{value:.5f}")

            # Update graph data if not paused
            if self.plotting_enabled and self.display_mode == "graph":
                for i, value in enumerate(values):
                    if f"Signal {i + 1}" in self.signals and self.signals[f"Signal {i + 1}"].get():
                        self.data_buffer[i].append((timestamp, value))

    def start_threads(self):
        """Start all necessary threads"""
        # Control system thread
        self.control_thread = Thread(target=self.control_serial_monitor, daemon=True)
        self.control_thread.start()
        
        # Monitor system thread
        self.monitor_thread = Thread(target=self.monitor_serial_monitor, daemon=True)
        self.monitor_thread.start()
        
        # Plot update thread
        self.schedule_plot_update()

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
            
            # Set y-axis limits with padding if we have visible data
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
            if hasattr(self, 'text_display_frame') and self.text_display_frame:
                self.text_display_frame.grid_remove()
            self.graph_frame.grid()
            self.update_plot()
        else:
            self.display_mode = "text"
            self.graph_frame.grid_remove()
            self.create_text_display()
            self.text_display_frame.grid()

    def create_text_display(self):
        """Create the text display frame"""
        if hasattr(self, 'text_display_frame') and self.text_display_frame.winfo_exists():
            self.text_display_frame.destroy()
            
        self.text_display_frame = ctk.CTkFrame(self.monitor_main_frame)
        self.text_display_frame.grid(row=0, column=0, sticky="nsew", padx=(int(10 * self.scale_factor),0), pady=int(10 * self.scale_factor))
        
        # Create scrollable container
        scroll_container = ctk.CTkScrollableFrame(self.text_display_frame)
        scroll_container.pack(fill="both", expand=True, padx=int(10 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        # Title
        ctk.CTkLabel(scroll_container, text="Signal Values", 
                    font=self.title_font).pack(pady=(0, int(10 * self.scale_factor)))
        
        # Create value displays
        self.value_boxes = []
        for i, signal_name in enumerate(self.signals):
            frame = ctk.CTkFrame(scroll_container, border_width=1, border_color="#555555")
            frame.pack(fill="x", padx=int(5 * self.scale_factor), pady=int(2 * self.scale_factor))
            
            label = ctk.CTkLabel(frame, text=self.signals[signal_name].cget("text"), 
                                width=int(180 * self.scale_factor), anchor="w", font=self.normal_font)
            label.pack(side="left", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
            
            value_box = ctk.CTkLabel(frame, text="0.00", width=int(80 * self.scale_factor), 
                                   fg_color="#1f1f1f", corner_radius=3,
                                   text_color="white", font=self.normal_font)
            value_box.pack(side="right", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
            
            self.value_boxes.append(value_box)

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
            self.toolbar_frame = ctk.CTkFrame(self.plot_container, height=int(40 * self.scale_factor))
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
                
                frame = ctk.CTkFrame(self.values_container, border_width=1, border_color="#555555")
                frame.pack(fill="x", padx=int(5 * self.scale_factor), pady=int(2 * self.scale_factor))
                
                label = ctk.CTkLabel(frame, text=self.signals[signal_name].cget("text"), 
                                    anchor="w", font=self.normal_font)
                label.pack(side="left", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
                
                value_label = ctk.CTkLabel(frame, text="0.00", width=int(80 * self.scale_factor), 
                                       fg_color="#1f1f1f", corner_radius=3,
                                       text_color="white", font=self.normal_font)
                value_label.pack(side="right", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))
                
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
            add_frame.grid(row=0, column=0, sticky="ew", padx=int(10 * self.scale_factor), pady=int(10 * self.scale_factor))
            
            self.signal_entry = ctk.CTkEntry(add_frame, placeholder_text="New signal name")
            self.signal_entry.pack(side="left", padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor), expand=True, fill="x")
            
            add_btn = ctk.CTkButton(add_frame, text="Add", width=int(80 * self.scale_factor),
                                   command=self.add_signal_from_entry)
            add_btn.pack(side="right", padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))

            # Signals list with scrollbar
            scroll_frame = ctk.CTkScrollableFrame(self.list_window)
            scroll_frame.grid(row=1, column=0, sticky="nsew", padx=int(10 * self.scale_factor), pady=(0,int(10 * self.scale_factor)))
            
            # Populate with current signals
            for signal_name in self.signals:
                frame = ctk.CTkFrame(scroll_frame, border_width=1, border_color="#555555")
                frame.pack(fill="x", padx=int(5 * self.scale_factor), pady=int(2 * self.scale_factor))

                label = ctk.CTkLabel(frame, text=self.signals[signal_name].cget("text"),
                                    width=int(250 * self.scale_factor), anchor="w", font=self.normal_font)
                label.pack(side="left", padx=int(10 * self.scale_factor), pady=int(5 * self.scale_factor))

                remove_btn = ctk.CTkButton(frame, text="Remove", width=int(80 * self.scale_factor),
                                         command=lambda name=signal_name: self.remove_signal(name),
                                         fg_color="#d9534f", hover_color="#c9302c")
                remove_btn.pack(side="right", padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))

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
            self.signals[signal_key].pack(side="top", padx=int(5 * self.scale_factor), pady=int(2 * self.scale_factor), anchor="w")
            self.signals[signal_key].select()  # Default to checked
            self.data_buffer.append(deque(maxlen=1000))
            self.raw_data_buffer.append(deque(maxlen=1000))
            
            # Check if this is the emergency park signal
            if "emergency_park" in signal_name.lower():
                self.emergency_park_signal = signal_key
            
            # Update text display if it exists
            if hasattr(self, 'text_display_frame') and self.text_display_frame.winfo_exists():
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
            if hasattr(self, 'text_display_frame') and self.text_display_frame.winfo_exists():
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
                        self.signals[signal_key].pack(side="top", padx=int(5 * self.scale_factor), pady=int(2 * self.scale_factor), anchor="w")
                        self.signals[signal_key].select()
                        self.data_buffer.append(deque(maxlen=1000))
                        self.raw_data_buffer.append(deque(maxlen=1000))
                        
                        # Check if this is the emergency park signal
                        if "emergency_park" in signal_name.lower():
                            self.emergency_park_signal = signal_key

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

    # Control System Methods
    def draw_steering_wheel(self, visual_angle=None):
        """Draw steering wheel with current angle"""
        self.steering_wheel_canvas.delete("all")
        rotation = math.radians(visual_angle if visual_angle is not None else self.steering_angle * 2.5)
        
        # Calculate wheel size based on canvas size
        canvas_size = min(self.steering_wheel_canvas.winfo_width(), self.steering_wheel_canvas.winfo_height())
        center = canvas_size / 2
        outer_radius = (canvas_size * 0.4)  # 80% of half canvas size
        inner_radius = outer_radius * 0.5    # 50% of outer radius
        spoke_length = outer_radius * 0.25    # 25% of outer radius
        
        # Draw wheel components with original proportions
        self.steering_wheel_canvas.create_oval(
            center - outer_radius, center - outer_radius,
            center + outer_radius, center + outer_radius,
            outline="#2196F3", width=int(12 * self.scale_factor), fill="#333333"
        )
        self.steering_wheel_canvas.create_oval(
            center - inner_radius, center - inner_radius,
            center + inner_radius, center + inner_radius,
            outline="#2196F3", width=int(4 * self.scale_factor), fill="#444444"
        )
        
        # Draw spokes with original proportions
        for angle in range(0, 360, 45):
            rad = math.radians(angle) + rotation
            x1, y1 = center + (inner_radius + spoke_length) * math.cos(rad), center + (inner_radius + spoke_length) * math.sin(rad)
            x2, y2 = center + outer_radius * math.cos(rad), center + outer_radius * math.sin(rad)
            self.steering_wheel_canvas.create_line(x1, y1, x2, y2, fill="#2196F3", width=int(2 * self.scale_factor))
        
        # Draw center with original size
        center_size = int(8 * self.scale_factor)
        self.steering_wheel_canvas.create_oval(
            center - center_size, center - center_size,
            center + center_size, center + center_size,
            fill="#2196F3", outline=""
        )
        
        # Draw pointer with original proportions
        pointer_size = int(12 * self.scale_factor)
        points = [
            center, center - outer_radius,
            center - pointer_size, center - outer_radius + pointer_size * 1.2,
            center + pointer_size, center - outer_radius + pointer_size * 1.2
        ]
        rotated = []
        for i in range(0, len(points), 2):
            x, y = points[i] - center, points[i+1] - center
            new_x = x * math.cos(rotation) - y * math.sin(rotation) + center
            new_y = x * math.sin(rotation) + y * math.cos(rotation) + center
            rotated.extend([new_x, new_y])
        
        self.steering_wheel_canvas.create_polygon(*rotated, fill="#FF9800", outline="#E65100", width=int(2 * self.scale_factor))

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
            visual_rotation = self.steering_angle
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

    def send_serial_command(self, command):
        """Send command over control serial"""
        if self.control_connected and command != self.last_sent_command:
            try:
                with self.control_lock:
                    if self.control_ser and self.control_ser.is_open:
                        self.control_ser.write(f"{command}\n".encode('utf-8'))
                        print(f"Control sent: {command}")
                        self.last_sent_command = command
            except Exception as e:
                print(f"Control serial write error: {str(e)}")
                self.disconnect_control_serial()

    def create_custom_buttons(self):
        """Create 8 custom buttons in 2 columns"""
        for widget in self.control_right_column.winfo_children():
            if widget not in [self.configure_btn]:
                widget.destroy()
        self.custom_buttons.clear()
        
        # Calculate button sizes based on window size
        button_width = int(self.window_width * 0.12)  # 12% of window width
        button_height = int(self.window_height * 0.06)  # 6% of window height
        button_padding = int(5 * self.scale_factor)
        
        for row in range(4):
            frame = ctk.CTkFrame(self.control_right_column)
            frame.pack(fill=tk.X, padx=button_padding, pady=button_padding)
            
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
                    width=button_width,
                    height=button_height,
                    font=self.normal_font,
                    fg_color="#4CAF50" if self.button_states[btn_index] else "#607D8B",
                    hover_color="#45a049" if self.button_states[btn_index] else "#455A64"
                )
                btn.pack(side=tk.LEFT, padx=button_padding, expand=True)
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
        if not self.control_connected:
            return
            
        # Create command string "BUTTONS:1,0,1,0,0,0,1,0"
        states_str = ",".join(map(str, self.button_states))
        command = f"BUTTONS:{states_str}"
        
        self.send_serial_command(command)

    def configure_buttons(self):
        """Open configuration window for buttons"""
        config_window = ctk.CTkToplevel(self.root)
        config_window.title("Configure Buttons")
        config_window.geometry("600x400")
        
        # Create a frame for the configuration options
        config_frame = ctk.CTkFrame(config_window)
        config_frame.pack(fill=tk.BOTH, expand=True, padx=int(10 * self.scale_factor), pady=int(10 * self.scale_factor))
        
        # Create a notebook for each button
        for i in range(8):
            button_frame = ctk.CTkFrame(config_frame)
            button_frame.pack(fill=tk.X, padx=int(5 * self.scale_factor), pady=int(5 * self.scale_factor))
            
            # Button number label
            ctk.CTkLabel(button_frame, text=f"Button {i+1}:", font=self.small_font).pack(side=tk.LEFT, padx=int(5 * self.scale_factor))
            
            # Name entry
            name_var = tk.StringVar(value=self.button_names[i])
            name_entry = ctk.CTkEntry(button_frame, textvariable=name_var, width=int(150 * self.scale_factor))
            name_entry.pack(side=tk.LEFT, padx=int(5 * self.scale_factor))
            
            # Type selection
            type_var = tk.StringVar(value=self.button_types[i])
            type_menu = ctk.CTkOptionMenu(button_frame, variable=type_var, values=["toggle", "push"])
            type_menu.pack(side=tk.LEFT, padx=int(5 * self.scale_factor))
            
            # Key binding
            key_var = tk.StringVar(value=self.button_keys[i])
            key_entry = ctk.CTkEntry(button_frame, textvariable=key_var, width=int(50 * self.scale_factor))
            key_entry.pack(side=tk.LEFT, padx=int(5 * self.scale_factor))
            
            # Save function for this button
            def save_config(idx=i, n_var=name_var, t_var=type_var, k_var=key_var):
                self.button_names[idx] = n_var.get()
                self.button_types[idx] = t_var.get()
                self.button_keys[idx] = k_var.get().upper()
                self.save_button_config()
                self.create_custom_buttons()
                self.update_key_bindings()
            
            # Save button
            save_btn = ctk.CTkButton(button_frame, text="Save", width=int(60 * self.scale_factor), command=save_config)
            save_btn.pack(side=tk.LEFT, padx=int(5 * self.scale_factor))

    def update_key_bindings(self):
        """Update keyboard bindings based on button configurations"""
        # Remove all existing button key bindings
        for key in self.button_keys:
            if key:
                self.root.unbind(f"<KeyPress-{key}>")
                self.root.unbind(f"<KeyRelease-{key}>")
        
        # Add new bindings
        for i, btn_key in enumerate(self.button_keys):
            if btn_key:
                if self.button_types[i] == "toggle":
                    self.root.bind(f"<KeyPress-{btn_key}>", lambda e, idx=i: self.toggle_button(idx))
                else:  # push button
                    self.root.bind(f"<KeyPress-{btn_key}>", lambda e, idx=i: self.push_button_down(idx))
                    self.root.bind(f"<KeyRelease-{btn_key}>", lambda e, idx=i: self.push_button_up(idx))

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
        visual_rotation = self.steering_angle
        self.draw_steering_wheel(visual_rotation)
        
        if self.steering_mode == "steering":
            self.steering_var.set(f"Steering Angle: {int(self.steering_angle)}°")
        else:
            radius = (self.steering_angle / 90) * 100
            self.steering_var.set(f"Rotation Radius: {radius:.1f}%")
        
        if self.move_direction != 0 and self.steering_angle != self.last_steering_angle:
            self.last_steering_angle = self.steering_angle
            self.send_movement_command()

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

    def on_closing(self):
        """Handle application closing"""
        try:
            # Stop all threads
            self.stop_threads = True
            
            # Stop camera if it's running
            if hasattr(self, 'cap') and self.cap is not None:
                self.cap.release()
            
            # Stop serial connections if they're active
            if hasattr(self, 'control_serial') and self.control_serial is not None:
                self.disconnect_control_serial()
            if hasattr(self, 'monitor_serial') and self.monitor_serial is not None:
                self.disconnect_monitor_serial()
            
            # Stop pygame mixer
            if hasattr(self, 'alarm_sound'):
                mixer.quit()
            
            # Close matplotlib figure
            if hasattr(self, 'fig'):
                plt.close(self.fig)
            
            # Destroy the root window
            self.root.destroy()
            
            # Force exit after a short delay to ensure cleanup
            self.root.after(100, lambda: os._exit(0))
            
        except Exception as e:
            print(f"Error during cleanup: {e}")
            os._exit(1)

if __name__ == "__main__":
    # Show configuration window first
    config_window = ConfigurationWindow()
    config_window.root.protocol("WM_DELETE_WINDOW", lambda: os._exit(0))  # Handle closing of config window
    config_window.root.mainloop()