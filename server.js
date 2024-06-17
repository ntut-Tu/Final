
const express = require('express');
const fs = require('fs');
const path = require('path');
const { spawn ,exec} = require('child_process');

const app = express();
const port = 3000;

let sensorProcess = null;
let monitorProcess = null;
let monitorData = {
    light: 'No data',
    wet: 'No data',
    status: 'Null'
};

app.get('/', (req, res) => {
    fs.readFile('index.html', (err, data) => {
        if (err) {
            res.writeHead(500, {'Content-Type': 'text/plain'});
            res.end('Internal Server Error');
        } else {
            res.writeHead(200, {'Content-Type': 'text/html'});
            res.end(data);
        }
    });
});

app.get('/get_lightValue', (req, res) => {
    updateData();
    res.send(monitorData.light.toString());
});

app.get('/get_wetValue', (req, res) => {
    res.send(monitorData.wet.toString());
});

app.get('/get_hanging_clothes_mode', (req, res) => {
    res.send(monitorData.status);
});

app.get('/set_hanging_clothes_mode', (req, res) => {
    let mode = req.query.mode;
    switch (mode) {
        case 'on':
            startMonitorProcess();
            res.send('startMonitor');
            break;
        case 'off':
            stopMonitorProcess();
            res.send('stopMonitor');
            break;
        default:
            res.send('Invalid mode');
    }
});

app.use(express.static('public'));

app.listen(port, () => {
    console.log(`Server is running at http://localhost:${port}`);
    startSensorProcess();
});

function updateData(){
    fs.readFile('output.txt', 'utf8', (err, data) => {
        if (err) {
            console.error('Error reading the file:', err);
            return;
        }

        const lines = data.split('\n');
        const readMode = lines[0].trim();

        if (data.startsWith('R\n')) {
            const lines = data.split('\n');
            if (lines.length > 1) {
                const lightLine = lines[1].match(/Light: (\d+)/);
                const humidityLine = lines[1].match(/Humidity: (\d+)/);
                if (lightLine && humidityLine) {
                    const light = parseInt(lightLine[1], 10);
                    const humidity = parseInt(humidityLine[1], 10);
                    monitorData.wet = humidity;
                    monitorData.light = light;
                    console.log(`Light: ${light}, Humidity: ${humidity}`);
                    updateMonitorProcess()
                }
            } else {
            console.error('Invalid sensor data format.');
            }
        } else {
            console.log('Data did not update.');
        }
    });
}
function startSensorProcess() {
    sensorProcess = spawn('python3', ['sensor.py']);
    sensorProcess.on('close', (code) => {
        console.log(`Sensor process exited with code ${code}`);
        sensorProcess = null;
    });
    sensorProcess.on('error', (err) => {
        console.error('Failed to start sensor process:', err);
    });
}

function startMonitorProcess() {
    monitorProcess = spawn('python3', ['monitor.py','start']);

    monitorProcess.stdout.on('data', (data) => {
        console.log(`Monitor Data: ${data}`);
    });

    monitorProcess.on('close', (code) => {
        console.log(`Monitor process exited with code ${code}`);
        monitorProcess = null;
    });

    monitorProcess.on('error', (err) => {
        console.error('Failed to start monitor process:', err);
    });
}
function openAlarmLight() {
    const command = 'sudo ' + path.resolve(__dirname, 'test.o') + ' LED1 on';
    console.log(`alarmLight ON`);
    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return;
        }
        console.log(`stdout: ${stdout}`);
        console.error(`stderr: ${stderr}`);
    });
}

function closeAlarmLight() {
    const command = 'sudo ' + path.resolve(__dirname, 'test.o') + ' LED1 off';
    console.log(`alarmLight OFF`);
    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return;
        }
        console.log(`stdout: ${stdout}`);
        console.error(`stderr: ${stderr}`);
    });
}

function updateMonitorProcess() {
    monitorProcess = spawn('python3', ['monitor.py','update']);
    monitorProcess.stdout.on('data', (data) => {
        const parts = data.toString().trim().split(' ');
        const lastValue = parts[parts.length - 2];
        const status = parts[parts.length - 1];
        monitorData.status = `${lastValue} ${status}`;
        console.log(`Monitor Status: ${lastValue} ${status}`);
        console.log(`Data Status: ${status}`);
        if(status === "True") {
            openAlarmLight();
        }else{
            closeAlarmLight();
        }
    });
    monitorProcess.on('close', (code) => {
        console.log(`Monitor process exited with code ${code}`);
        monitorProcess = null;
    });

    monitorProcess.on('error', (err) => {
        console.error('Failed to start monitor process:', err);
    });

}


function stopMonitorProcess() {
    monitorProcess = spawn('python3', ['monitor.py','exit']);

    monitorProcess.stdout.on('data', (data) => {
        console.log(`Monitor Data: ${data}`);
    });

    monitorProcess.on('close', (code) => {
        console.log(`Monitor process exited with code ${code}`);
        monitorProcess = null;
    });

    monitorProcess.on('error', (err) => {
        console.error('Failed to start monitor process:', err);
    });
}
