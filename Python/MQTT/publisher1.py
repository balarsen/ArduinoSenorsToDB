#!/usr/bin/env python3
# https://www.ev3dev.org/docs/tutorials/sending-and-receiving-messages-with-mqtt/

import datetime
import json
import numpy as np

import paho.mqtt.client as mqtt

import NumpyEncoder

# This is the Publisher

data = np.random.random_sample(1)
message = {"Message": "Hello World!",
           "datetime": datetime.datetime.now().isoformat(),
           "data":data}

client = mqtt.Client()
client.connect("localhost", 1883, 60)
# QOS
# At most once(0)
# At least once(1)
# Exactly once(2).
message_json = json.dumps(message, cls=NumpyEncoder.NumpyEncoder)

client.publish("topic/test",
               payload=json.dumps(message, cls=NumpyEncoder.NumpyEncoder),
               qos=1)
client.disconnect()

