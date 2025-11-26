from flask import Flask, request, jsonify
from flask import render_template_string
import sqlite3
from datetime import datetime

DB_PATH = "data.db"

app = Flask(__name__)

def init_db():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("""
        CREATE TABLE IF NOT EXISTS measurements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts TEXT,
            lux REAL
        )
    """)
    conn.commit()
    conn.close()

init_db()

@app.route("/api/lux", methods=["POST"])
def api_lux():
    data = request.get_json(force=True)
    lux = data.get("lux")

    if lux is None:
        return jsonify({"status": "error", "msg": "lux requerido"}), 400

    ts = datetime.utcnow().isoformat()

    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("INSERT INTO measurements (ts, lux) VALUES (?, ?)", (ts, float(lux)))
    conn.commit()
    conn.close()

    return jsonify({"status": "ok"})

@app.route("/data")
def data():
    conn = sqlite3.connect(DB_PATH)
    c = conn.cursor()
    c.execute("SELECT ts, lux FROM measurements ORDER BY ts")
    rows = c.fetchall()
    conn.close()

    labels = [r[0] for r in rows]
    values = [r[1] for r in rows]

    return jsonify({"labels": labels, "values": values})

# Página principal con la gráfica
INDEX_HTML = """
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <title>Lux TSL2561</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <h2>Histórico de luz (lux)</h2>
  <canvas id="luxChart"></canvas>

  <script>
    async function loadData() {
      const resp = await fetch('/data');
      const json = await resp.json();

      const ctx = document.getElementById('luxChart').getContext('2d');

      const chart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: json.labels,
          datasets: [{
            label: 'Lux',
            data: json.values,
            borderWidth: 1
          }]
        },
        options: {
          scales: {
            x: {
              ticks: { maxRotation: 45, minRotation: 45 }
            },
            y: {
              beginAtZero: true
            }
          }
        }
      });
    }

    loadData();
    // Si quieres refresco automático cada 30s:
    // setInterval(loadData, 30000);
  </script>
</body>
</html>
"""

@app.route("/")
def index():
    return render_template_string(INDEX_HTML)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)

