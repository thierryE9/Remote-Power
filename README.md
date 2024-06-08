# ESP8266 Remote PC Power Switch

This project allows you to remotely turn on your PC using an ESP8266 module. By integrating with Google Sheets, the system can monitor specific cell values and trigger actions based on their state.

## Components

- ESP8266
- IRFZ44N MOSFET
- Two 1kΩ resistors

## How It Works

1. **Hardware Setup:**
   - Connect the IRFZ44N MOSFET and resistors to the ESP8266 and the PC power switch.
   
2. **Google Sheets Setup:**
   - Create a new Google Sheet.
   - Name two cells as `powerBtn` and `forceSD`.

3. **Functionality:**
   - When the cell next to `powerBtn` contains the value `1`, the ESP8266 will trigger the power switch to turn on the PC.
   - Similarly, when the cell next to `forceSD` contains the value `1`, the ESP8266 will trigger a forced shutdown.
   - After performing the action, the ESP8266 sends a POST request to set the respective cell value back to `0`.
   - The ESP8266 checks the values of these cells every 5 seconds by sending GET requests.

## Setup Instructions

### 1. Hardware Connections

- Connect the ESP8266, IRFZ44N MOSFET, and resistors according to the following schematic:

  ```
  [Schematic Diagram Placeholder]
  ```

### 2. Google Sheets Configuration

- Create a Google Sheets document.
- Name two cells as follows:
  - `powerBtn`
  - `forceSD`
  
### 3. ESP8266 Code

- Update the ESP8266 Arduino code with your WiFi SSID and password:

  ```cpp
  const char* ssid = "YOUR_SSID";
  const char* password = "YOUR_PASSWORD";
  ```

### 4. Google Sheets Script

- Add the provided script to your Google Sheets to allow interaction with the ESP8266:

- Replace `sheet-id` and `sheet-name` in the script with your actual Google Sheets ID and sheet name.

### 5. Upload Code to ESP8266

- Upload the updated Arduino code to your ESP8266 module.

## Usage

1. Open your Google Sheets document.
2. Set the value next to `powerBtn` to `1` to turn on the PC.
3. Set the value next to `forceSD` to `1` to force shutdown the PC.
4. The ESP8266 will automatically reset these values back to `0` after executing the commands.
