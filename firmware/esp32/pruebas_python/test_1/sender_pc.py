from influxdb import InfluxDBClient
import time
import random

RPI_IP = "192.168.1.139"

client = InfluxDBClient(
    host=RPI_IP,
    port=8086,
    database="metrics"
)

while True:
    value = random.uniform(20.0, 30.0)

    json_body = [
        {
            "measurement": "temperature_pc",
            "tags": {
                "host": "my_pc"
            },
            "fields": {
                "value": value
            }
        }
    ]

    ok = client.write_points(json_body)
    print("Enviado:", value, "OK?", ok)
    time.sleep(5)
