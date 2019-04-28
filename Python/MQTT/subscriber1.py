#!/usr/bin/env python3
# https://www.tutorialspoint.com/sqlalchemy/sqlalchemy_core_creating_table.htm

import datetime
import json
from pprint import pprint

import paho.mqtt.client as mqtt
import pandas as pd
from sqlalchemy import create_engine

# from sqlalchemy import create_engine, MetaData, Table, Column
# from sqlalchemy.ext.declarative import declarative_base
# from sqlalchemy import Column, Integer, String, BigInteger, DateTime
# from sqlalchemy.orm import sessionmaker
#
# engine = create_engine('sqlite:///mqtt_test.sqlite', echo=True)
# meta = MetaData()
#
# # Base = declarative_base()
# # Session = sessionmaker(bind=engine)
# # session = Session()
# # Base.metadata.create_all(engine)
#
# message = Table('message', meta,
#                 Column('id', BigInteger, primary_key=True),
#                 Column('rectime', String),
#                 Column('sendtime', String),
#                 Column('message', String))
# meta.create_all(engine)
# class Message(Base):
#     __tablename__ = 'message'
#     id = Column(BigInteger, primary_key=True)
#     rectime = Column(String(26))
#     sendtime = Column(String(26))
#     message = Column(String)
#
#     def __repr__(self):
#         return "<Message(id={}, rectime={}, sendtime={}, message={}".format(self.id, self.rectime, self.sendtime,
#                                                                             self.message)
#

engine = create_engine('sqlite:///mqtt_test.sqlite', echo=True)


# This is the Subscriber

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("topic/test")


def _msg_to_db(msg, rectime):
    pprint((rectime, msg))
    df = pd.DataFrame({'rectime': rectime, 'message': msg['Message']}, index=pd.DatetimeIndex([msg['datetime']]))
    print(df)
    df.to_sql('message', con=engine, if_exists='append')
    # ins = message.insert().values(rectime=rectime.isoformat(), sendtime=msg['datetime'],
    #                               message=msg['Message'])
    # conn = engine.connect()
    # result = conn.execute(ins)
    # print("Result", result)
    # # row_Message = Message(rectime=rectime,
    #                       sendtime=dateutil.parser.parse(msg['datetime']),
    #                       message=msg['Message'])
    # session.add(row_Message)
    # session.commit()


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
