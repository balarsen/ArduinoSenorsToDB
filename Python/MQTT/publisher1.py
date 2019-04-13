#!/usr/bin/env python3
# https://www.ev3dev.org/docs/tutorials/sending-and-receiving-messages-with-mqtt/

import datetime
import json

import paho.mqtt.client as mqtt

# This is the Publisher

message = {"Message": "Hello World!", "datetime": datetime.datetime.now().isoformat()}

client = mqtt.Client()
client.connect("localhost", 1883, 60)
# QOS
# At most once(0)
# At least once(1)
# Exactly once(2).
client.publish("topic/test", payload=json.dumps(message), qos=1)
client.disconnect()
