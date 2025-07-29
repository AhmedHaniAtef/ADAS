import spidev
import time

class MCP2515:
    # MCP2515 SPI Instruction Set
    RESET = 0xC0
    READ = 0x03
    READ_RXB0SIDH = 0x90
    READ_RXB0D0 = 0x92
    READ_RXB1SIDH = 0x94
    READ_RXB1D0 = 0x96
    WRITE = 0x02
    LOAD_TXB0SIDH = 0x40
    LOAD_TXB0D0 = 0x41
    LOAD_TXB1SIDH = 0x42
    LOAD_TXB1D0 = 0x43
    LOAD_TXB2SIDH = 0x44
    LOAD_TXB2D0 = 0x45
    RTS_TX0 = 0x81
    RTS_TX1 = 0x82
    RTS_TX2 = 0x84
    RTS_ALL = 0x87
    READ_STATUS = 0xA0
    RX_STATUS = 0xB0
    BIT_MOD = 0x05

    # MCP2515 Register Addresses
    CANSTAT = 0x0E
    CANCTRL = 0x0F
    CANINTE = 0x2B
    CANINTF = 0x2C
    EFLG = 0x2D
    TXB0CTRL = 0x30
    TXB1CTRL = 0x40
    TXB2CTRL = 0x50
    RXB0CTRL = 0x60
    RXB1CTRL = 0x70
    CNF1 = 0x2A
    CNF2 = 0x29
    CNF3 = 0x28

    # CAN speeds for 8MHz clock
    CAN_SPEEDS = {
        '1000kBPS': [0x00, 0xC0, 0x80],
        '500kBPS': [0x00, 0xD1, 0x81],
        '250kBPS': [0x80, 0xE5, 0x83],
        '200kBPS': [0x80, 0xF6, 0x84],
        '125kBPS': [0x81, 0xE5, 0x83],
        '100kBPS': [0x81, 0xF6, 0x84]
    }

    def __init__(self, bus=0, device=0, speed_hz=10000000, mode=0b00):
        self.spi = spidev.SpiDev()
        self.spi.open(bus, device)
        self.spi.max_speed_hz = speed_hz
        self.spi.mode = mode
        self.initialized = False

    def close(self):
        self.spi.close()

    def _spi_transfer(self, data):
        """Helper function for SPI transfer"""
        return self.spi.xfer2(data)

    def reset(self):
        """Reset MCP2515"""
        self._spi_transfer([self.RESET])
        time.sleep(0.1)  # Wait for reset to complete

    def read_byte(self, address):
        """Read single byte from specified address"""
        tx_data = [self.READ, address, 0x00]  # Dummy byte for reading
        rx_data = self._spi_transfer(tx_data)
        return rx_data[2]  # Return the received data byte

    def write_byte(self, address, data):
        """Write single byte to specified address"""
        tx_data = [self.WRITE, address, data]
        self._spi_transfer(tx_data)

    def bit_modify(self, address, mask, data):
        """Modify specific bits in a register"""
        tx_data = [self.BIT_MOD, address, mask, data]
        self._spi_transfer(tx_data)

    def set_config_mode(self):
        """Set MCP2515 to configuration mode"""
        self.write_byte(self.CANCTRL, 0x80)
        
        # Verify mode change
        for _ in range(10):
            canstat = self.read_byte(self.CANSTAT)
            if (canstat & 0xE0) == 0x80:
                return True
            time.sleep(0.01)
        return False

    def set_normal_mode(self):
        """Set MCP2515 to normal mode"""
        self.write_byte(self.CANCTRL, 0x00)
        
        # Verify mode change
        for _ in range(10):
            canstat = self.read_byte(self.CANSTAT)
            if (canstat & 0xE0) == 0x00:
                return True
            time.sleep(0.01)
        return False

    def set_sleep_mode(self):
        """Set MCP2515 to sleep mode"""
        self.write_byte(self.CANCTRL, 0x20)
        
        # Verify mode change
        for _ in range(10):
            canstat = self.read_byte(self.CANSTAT)
            if (canstat & 0xE0) == 0x20:
                return True
            time.sleep(0.01)
        return False

    def initialize(self, speed='500kBPS'):
        """Initialize MCP2515 with specified CAN speed"""
        if speed not in self.CAN_SPEEDS:
            raise ValueError("Invalid CAN speed specified")
        
        # Reset the controller
        self.reset()
        
        # Set configuration mode
        if not self.set_config_mode():
            return False
        
        # Set CAN speed
        cnf1, cnf2, cnf3 = self.CAN_SPEEDS[speed]
        self.write_byte(self.CNF1, cnf1)
        self.write_byte(self.CNF2, cnf2)
        self.write_byte(self.CNF3, cnf3)
        
        # Enable interrupts
        self.write_byte(self.CANINTE, 0x03)  # Enable RX0 and RX1 interrupts
        
        # Set filters/masks to accept all messages
        # RXB0CTRL: Enable rollover, accept all
        self.write_byte(self.RXB0CTRL, 0x04)
        # RXB1CTRL: Accept all
        self.write_byte(self.RXB1CTRL, 0x01)
        
        # Set normal mode
        if not self.set_normal_mode():
            return False
        
        self.initialized = True
        return True

    def read_status(self):
        """Read status register"""
        tx_data = [self.READ_STATUS, 0x00]  # Dummy byte for reading
        rx_data = self._spi_transfer(tx_data)
        return rx_data[1]  # Return the received status byte

    def get_rx_status(self):
        """Get RX status"""
        tx_data = [self.RX_STATUS, 0x00]  # Dummy byte for reading
        rx_data = self._spi_transfer(tx_data)
        return rx_data[1]  # Return the received status byte

    def load_tx_buffer(self, buffer_num, id_reg, dlc, data):
        """Load TX buffer with message data"""
        if buffer_num == 0:
            instruction = self.LOAD_TXB0SIDH
        elif buffer_num == 1:
            instruction = self.LOAD_TXB1SIDH
        elif buffer_num == 2:
            instruction = self.LOAD_TXB2SIDH
        else:
            raise ValueError("Invalid TX buffer number")
        
        # Create the data packet
        tx_data = [instruction] + id_reg + [dlc] + data[:dlc]
        self._spi_transfer(tx_data)

    def request_to_send(self, buffer_num):
        """Send RTS for specified buffer"""
        if buffer_num == 0:
            instruction = self.RTS_TX0
        elif buffer_num == 1:
            instruction = self.RTS_TX1
        elif buffer_num == 2:
            instruction = self.RTS_TX2
        else:
            raise ValueError("Invalid TX buffer number")
        
        self._spi_transfer([instruction])

    def send_message(self, can_id, data, extended=False):
        """Send CAN message"""
        # Convert CAN ID to register format
        id_reg = self._convert_can_id_to_reg(can_id, extended)
        
        # Find available TX buffer
        status = self.read_status()
        tx_priority = [0, 1, 2]  # Priority order for TX buffers
        
        for buf_num in tx_priority:
            if not (status & (1 << (2 + buf_num))):  # Check TXBnREQ bit
                self.load_tx_buffer(buf_num, id_reg, len(data), data)
                self.request_to_send(buf_num)
                return True
        
        return False  # No buffers available

    def receive_message(self):
        """Receive CAN message if available"""
        rx_status = self.get_rx_status()
        rx_buffer = rx_status & 0x03
        
        if rx_buffer == 0:
            return None  # No message available
        
        # Determine which buffer to read from
        if rx_buffer & 0x01:  # RXB0 has message
            instruction = self.READ_RXB0SIDH
        else:  # RXB1 has message
            instruction = self.READ_RXB1SIDH
        
        # Read the message (13 bytes: SIDH, SIDL, EID8, EID0, DLC, D0-D7)
        tx_data = [instruction] + [0x00] * 13  # Dummy bytes for reading
        rx_data = self._spi_transfer(tx_data)[1:]  # Skip first byte
        
        # Parse the message
        msg = {
            'id': 0,
            'extended': bool(rx_status & 0x08),  # Check if extended frame
            'dlc': rx_data[4] & 0x0F,
            'data': rx_data[5:13]
        }
        
        if msg['extended']:
            msg['id'] = self._convert_reg_to_extended_id(rx_data[1], rx_data[2], rx_data[0], rx_data[1])
        else:
            msg['id'] = self._convert_reg_to_standard_id(rx_data[0], rx_data[1])
        
        # Clear interrupt flag
        if rx_buffer & 0x01:
            self.bit_modify(self.CANINTF, 0x01, 0x00)  # Clear RX0IF
        else:
            self.bit_modify(self.CANINTF, 0x02, 0x00)  # Clear RX1IF
        
        return msg

    def _convert_reg_to_standard_id(self, sidh, sidl):
        """Convert register values to standard CAN ID"""
        return (sidh << 3) | (sidl >> 5)

    def _convert_reg_to_extended_id(self, eidh, eidl, sidh, sidl):
        """Convert register values to extended CAN ID"""
        sidl_lo2bits = sidl & 0x03
        sidl_hi3bits = sidl >> 5
        can_id = (sidh << 3) | sidl_hi3bits
        can_id = (can_id << 2) | sidl_lo2bits
        can_id = (can_id << 8) | eidh
        can_id = (can_id << 8) | eidl
        return can_id

    def _convert_can_id_to_reg(self, can_id, extended):
        """Convert CAN ID to register format"""
        id_reg = [0, 0, 0, 0]  # SIDH, SIDL, EID8, EID0
        
        if extended:
            # EID0
            id_reg[3] = can_id & 0xFF
            can_id >>= 8
            
            # EID8
            id_reg[2] = can_id & 0xFF
            can_id >>= 8
            
            # SIDL
            sidl_lo2bits = can_id & 0x03
            can_id <<= 3
            id_reg[1] = (can_id & 0xE0) | sidl_lo2bits | 0x08
            
            # SIDH
            can_id >>= 8
            id_reg[0] = can_id & 0xFF
        else:
            # Standard ID
            id_reg[1] = (can_id << 5) & 0xFF
            can_id >>= 3
            id_reg[0] = can_id & 0xFF
        
        return id_reg


# Example usage
if __name__ == "__main__":
    try:
        # Initialize MCP2515
        can = MCP2515(bus=0, device=0, speed_hz=10000000)
        
        if not can.initialize(speed='1000kBPS'):
            print("Failed to initialize MCP2515")
            exit(1)
        
        print("MCP2515 initialized successfully")
        
        # Send a CAN message
        can_id = 0x123
        data = [0x01, 0x02, 0x03, 0x04]
        if can.send_message(can_id, data):
            print(f"Sent CAN message: ID=0x{can_id:03X}, Data={[hex(x) for x in data]}")
        else:
            print("Failed to send CAN message (no buffers available)")
        
        # Check for received messages
        msg = can.receive_message()
        if msg:
            print(f"Received CAN message: ID=0x{msg['id']:03X}, Extended={msg['extended']}, "
                  f"DLC={msg['dlc']}, Data={[hex(x) for x in msg['data'][:msg['dlc']]]}")
        else:
            print("No messages received")
        
    finally:
        can.close()
