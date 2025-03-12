#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

// WiFi Credentials
const char* ssid = "ZTE Nubia Redmagic 9S Pro";
//ZTE Nubia Redmagic 9S Pro
const char* password = "rijan123";
//rijan123

// Web Server and WebSocket
ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

// HTML Content
const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>AgriBot Control</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
        }
       
        body {
            /* background: #1a1a1a; */
            background-image: linear-gradient(90deg, #020024 0%, #090979 35%, #00d4ff 100%);
           
            min-height: 100vh;
            padding: 20px;
        }
       
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
       
        .header {
            text-align: center;
            margin-bottom: 30px;
        }
       
        .header h1 {
            font-size: 2.5rem;
            color: #89da08;
        }
       
        .connection-panel {
           
            padding: 15px;
            border-radius: 10px;
            margin-bottom: 20px;
            display: flex;
            gap: 10px;
            align-items: center;
        }
       
        #esp-ip {
            flex: 1;
            padding: 8px;
            border-radius: 5px;
            border: 1px solid #444;
            background: #333;
            color: white;
        }
       
        button {
            padding: 8px 20px;
            border: none;
            border-radius: 5px;
            background: #3482a1;;
            color: white;
            cursor: pointer;
            transition: all 0.3s;
        }
       
        button:hover {
            background: #8edf98;;
        }
       
        .mode-selector {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
        }
       
        .mode-btn {
            flex: 1;
            padding: 12px;
           
            border: 2px solid #444;
            color: #888;
            font-weight: bold;
            transition: all 0.3s;
        }
       
        .mode-btn.active {
            border-color: #89da08;
            /* color: #; */
            background: #1a1a1a;
        }
       
       
       
        .control-panel {
           
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 20px;
            align-items: center;
        }
       
        .split-layout {
            display: grid;
            grid-template-columns: 1fr;
            gap: 20px;
            margin-bottom: 20px;
       
            @media (min-width: 768px) {
                grid-template-columns: repeat(2, 1fr);
               
            }
       
            @media (min-width: 1024px) {
                grid-template-columns: repeat(4, 1fr);
            }
       
           
        }
       
        .joystick-grid {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
        }
       
        .joystick-btn {
            aspect-ratio: 1;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 14px;
           
            border: 2px solid #444;
            color: #ccc;
            cursor: pointer;
            transition: all 0.3s;
            border-radius: 10px;
       
            @media (min-width: 768px) {
                font-size: 20px;
            }
            @media(min-width: 1024px) {
                font-size: 22px;
            }
        }
       
        .joystick-btn:hover {
            border-color: #89da08;
            /* color: #00cc66; */
        }
       
        #stop-btn {
            background: #ff4444;
        }
       
        #stop-btn:hover {
            background: #cc0000;
        }
       
        /* .video-feed {
            background: #000;
            border-radius: 10px;
            height: 300px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: #666;
        } */
       
        .tools-panel {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 10px;
            margin-top: 20px;
        }
       
        .tool-group {
            /* background: #333; */
            border-radius: 10px;
            padding: 15px;
            display: flex;
            flex-direction: column;
            gap: 10px;
        }
       
        .tool-group h3 {
            color: #89da08;
            margin-bottom: 10px;
            text-align: center;
        }
       
        .tool-group button {
            width: 100%;
            /* background: #444; */
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            padding: 12px;
            transition: all 0.3s;
        }
       
        .tool-group button:hover {
            background: #555;
        }
       
        .tool-group .stop-btn {
            background: #ff4444;
        }
       
        .tool-group .stop-btn:hover {
            background: #cc0000;
        }
       
        .status-indicators {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 10px;
            margin-top: 20px;
        }
       
        .status-card {
           
            padding: 15px;
            border-radius: 8px;
            text-align: center;
            /* background: #333; */
            color: #ccc;
        }
       
        .status-card h3 {
            color: #89da08;
            margin-bottom: 8px;
        }
       
        .small-btn {
            padding: 6px 12px;
            font-size: 0.9rem;
            width: auto;
            margin: 0 auto;
            background-color: #b8dabc;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: all 0.3s;
           
        }
        .measure-controls {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-bottom: 15px;
        }
        .measure-btn {
            background: #444;
            padding: 12px;
        }
        #length-value::before {
        content: "📏 ";
        }
       
        #breadth-value::before {
        content: "📐 ";
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>AgriBot Smart Control</h1>
        </div>


        <div class="connection-panel">
            <input type="text" id="esp-ip" placeholder="Enter ESP32 IP Address">
            <button onclick="connectWS()">Connect</button>
            <span id="connection-status" style="margin-left: auto; color: #ff4444">● Disconnected</span>
        </div>


        <div class="mode-selector">
            <button class="mode-btn active" onclick="switchMode(1)">Manual Control</button>
            <button class="mode-btn" onclick="switchMode(2)">Autonomous Mode</button>
        </div>


        <!-- Manual Control Panel -->
        <div id="mode1" class="control-panel">
            <div class="split-layout">
                <div>
                    <h2 style="margin-bottom: 15px; color: #00cc66">Manual Controls</h2>
                    <div class="joystick-grid">
                        <div></div>
                        <button class="joystick-btn" onclick="sendCommand('MOVE_FORWARD')">↑</button>
                        <div></div>
                        <button class="joystick-btn"
                                onmousedown="sendCommand('TURN_LEFT')"
                                onmouseup="sendCommand('STOP')"
                                ontouchstart="sendCommand('TURN_LEFT')"
                                ontouchend="sendCommand('STOP')">←</button>
                       
                        <button class="joystick-btn" id="stop-btn" onclick="sendCommand('STOP')">▶</button>
                        <button class="joystick-btn" onmousedown="sendCommand('TURN_RIGHT')"
                                onmouseup="sendCommand('STOP')"
                                ontouchstart="sendCommand('TURN_RIGHT')"
                                ontouchend="sendCommand('STOP')">→</button>
                        <div></div>
                        <button class="joystick-btn" onclick="sendCommand('MOVE_BACKWARD')">↓</button>
                        <div></div>
                    </div>
                </div>
               <div>
                    <div class="measure-controls">
                        <button class="measure-btn" onclick="sendCommand('LENGTH_MEASURE')">Start Length Measure</button>
                        <button class="measure-btn" onclick="sendCommand('STOP_LENGTH_MEASURE')">Stop Length Measure</button>
                        <button class="measure-btn" onclick="sendCommand('BREADTH_MEASURE')">Start Breadth Measure</button>
                        <button class="measure-btn" onclick="sendCommand('STOP_BREADTH_MEASURE')">Stop Breadth Measure</button>
                    </div>
                                  <div class="status-card">
                      <h3>Field Dimensions</h3>
                      <p id="length-value">Length: 0.00 m</p>
                      <p id="breadth-value">Breadth: 0.00 m</p>
                  </div>
                 
                </div>
           
            </div>


            <h3 style="margin: 20px 0 15px 0; color: #00cc66">Tools Control</h3>
            <div class="tools-panel">
                <div class="tool-group">
                    <h3>Flattening</h3>
                    <button onclick="sendCommand('FLATTENING_UP')">🔼 Flattening Up</button>
                    <button onclick="sendCommand('FLATTENING_DOWN')">🔽 Flattening Down</button>
                </div>
                <div class="tool-group">
                    <h3>Ploughing</h3>
                    <button onclick="sendCommand('PLOUGHING_UP')">🔼 Ploughing Up</button>
                    <button onclick="sendCommand('PLOUGHING_DOWN')">🔽 Ploughing Down</button>
                </div>
                <div class="tool-group">
                    <h3>Moisture </h3>
                    <button onclick="sendCommand('MOISTURE_UP')">🔼 Moisture Up</button>
                    <button onclick="sendCommand('MOISTURE_DOWN')">🔽 Moisture Down</button>
                </div>
                <div class="tool-group">
                    <h3>Pump Control</h3>
                    <button onclick="sendCommand('PUMP_ON')">💧 Start Pump</button>
                    <button class="stop-btn" onclick="sendCommand('PUMP_OFF')">🚫 Stop Pump</button>
                </div>
                <div class="tool-group">
        <h3>Seed Sowing</h3>
        <button onclick="sendCommand('SEED_SOWING_ON')">🌱 Start Seeding</button>
        <button class="stop-btn" onclick="sendCommand('SEED_SOWING_OFF')">🚫 Stop Seeding</button>
    </div>
            </div>
        </div>
 </div>
        <!-- Autonomous Mode Panel -->
        <div id="mode2" class="control-panel" style="display: none;">
    <h2 style="margin-bottom: 15px; color: #00cc66">Autonomous Operations</h2>
    <div class="split-layout">
     <div>
                   
        <div style="display: flex; flex-direction: column; gap: 10px;">
            <button class="small-btn" onclick="sendCommand('AUTO_START')">
                🤖 Start Program
            </button>
            <button class="small-btn stop-btn" id="stop-btn" onclick="sendCommand('EMERGENCY_STOP')">
                🛑 Emergency Stop
            </button>
        </div>
       </div>
    </div>
</div>






        <!-- Status Indicators -->
        <div class="status-indicators">
    <div class="status-card">
        <h3>Temperature and Humidity</h3>
        <p id="temp-value">24°C 🌡️</p>
        <p id="humidity-value">50% </p>
    </div>
    <div class="status-card">
        <h3>Soil Moisture Value</h3>
        <p id="moisture-value">65% </p>
    </div>
</div>
    </div>


    <script>
        let socket;
        let isMoving = false;


        function switchMode(mode) {
            document.querySelectorAll('.control-panel').forEach(panel => {
                panel.style.display = 'none';
            });
            document.querySelectorAll('.mode-btn').forEach(btn => {
                btn.classList.remove('active');
            });
            document.getElementById(`mode${mode}`).style.display = 'block';
            document.querySelectorAll('.mode-btn')[mode-1].classList.add('active');
        }


        function connectWS() {
            const ip = document.getElementById('esp-ip').value;
            socket = new WebSocket(`ws://${ip}:81`);


            socket.onopen = () => updateConnectionStatus(true);
            socket.onclose = () => updateConnectionStatus(false);




           socket.onmessage = (event) => {
    const data = JSON.parse(event.data);


    if(data.sensors) {
     
        document.getElementById('temp-value').textContent =
                `${data.sensors.temp.toFixed(1)}°C 🌡️`;
        document.getElementById('humidity-value').textContent =
            `${data.sensors.humidity.toFixed(1)}% `;
        document.getElementById('moisture-value').textContent =
            `${data.sensors.moisture}% `;    
    }
      if(data.length) {
        document.getElementById('length-value').textContent =
            `Length: ${data.length.toFixed(2)} m`;
    }
    if(data.breadth) {
        document.getElementById('breadth-value').textContent =
            `Breadth: ${data.breadth.toFixed(2)} m`;
    }
};
        }


        function sendCommand(command) {
            if(socket?.readyState === WebSocket.OPEN) {
                socket.send(command);
            }
        }


        function updateConnectionStatus(connected) {
            const status = document.getElementById('connection-status');
            status.textContent = connected ? '● Connected' : '● Disconnected';
            status.style.color = connected ? '#00cc66' : '#ff4444';
        }
       
    </script>
</body>
</html>
)rawliteral";

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT:
            if (length > 0) {
                String command = (char*)payload;
                Serial.println(command); // Forward the command to Arduino Mega
            }
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200); // Initialize Serial communication with Arduino Mega

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Start Web Server
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", htmlContent);
    });
    server.begin();

    // Start WebSocket
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    server.handleClient();
    webSocket.loop();

    // Read sensor data from Arduino Mega
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    
   if (data.startsWith("SENSOR_DATA:")) {
    data.remove(0, 12);
    int comma1 = data.indexOf(',');
    int comma2 = data.indexOf(',', comma1 + 1);

    if (comma1 == -1 || comma2 == -1) return;

    float temp = data.substring(0, comma1).toFloat();
    float humidity = data.substring(comma1 + 1, comma2).toFloat();
    int moisture = data.substring(comma2 + 1).toInt();

    String json = "{\"sensors\":{"
        "\"temp\":" + String(temp) + ","
        "\"humidity\":" + String(humidity) + ","
        "\"moisture\":" + String(moisture) + "}}";
    webSocket.broadcastTXT(json);
}
 else if (data.startsWith("LENGTH_DATA:")) {
            data.remove(0, 12);
            float length = data.toFloat();
            String json = "{\"length\":" + String(length) + "}";
            webSocket.broadcastTXT(json);
        }
        else if (data.startsWith("BREADTH_DATA:")) {
            data.remove(0, 13);
            float breadth = data.toFloat();
            String json = "{\"breadth\":" + String(breadth) + "}";
            webSocket.broadcastTXT(json);
        }

    }
    }

