const express = require('express');
const cors = require('cors');
const path = require('path');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const PORT = 3000;

let latestData = {
  temperature: null,
  humidity: null,
  distance: null,
};

// Configure le port série, adapte le COM et baudRate si besoin
const port = new SerialPort({
  path: 'COM3',
  baudRate: 115200,
});

// Parseur ligne par ligne
const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }));

// Lecture des données JSON envoyées par Arduino
parser.on('data', (line) => {
  try {
    const data = JSON.parse(line);
    latestData = data;
    console.log('Données reçues:', data);
  } catch (err) {
    console.error('Erreur JSON:', err.message, '| Ligne:', line);
  }
});

port.on('error', (err) => {
  console.error('Erreur port série:', err.message);
});

// Middleware CORS + JSON + statics
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public'))); // ton frontend HTML ici

// API pour récupérer les dernières données
app.get('/api/data', (req, res) => {
  res.json(latestData);
});

// Démarrage serveur
app.listen(PORT, () => {
  console.log(`Serveur lancé sur http://localhost:${PORT}`);
});
