#!/usr/bin/env python3
# https://www.tutorialspoint.com/sqlalchemy/sqlalchemy_core_creating_table.htm

import datetime
import json
from pprint import pprint

import paho.mqtt.client as mqtt
import pandas as pd
from sqlalchemy import create_engine

engine = create_engine('sqlite:///mqtt_test.sqlite', echo=True)


# This is the Subscriber

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("topic/test")


def _msg_to_db(msg, rectime):
    pprint((rectime, msg))
    df = pd.DataFrame.from_dict(msg).set_index('datetime', drop=True)
    df['rectime'] = rectime
    print(df)
    df.to_sql('message', con=engine, if_exists='append')


def on_message(client, userdata, msg):
    rectime = datetime.datetime.now()
    message = msg.payload.decode()
    message_val = json.loads(message)
    _msg_to_db(message_val, rectime)


client = mqtt.Client()
client.connect("localhost", 1883, 60)

client.on_connect = on_connect
client.on_message = on_message

client.loop_forever()
