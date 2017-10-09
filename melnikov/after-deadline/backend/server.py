#!/usr/bin/python3

import random
import string
import sys

from autobahn.asyncio.websocket import WebSocketServerFactory, \
    WebSocketServerProtocol

import tables_pb2 as protocol
import fake_database as db

class TablesServerProtocol(WebSocketServerProtocol):
    def onOpen(self):
        print('opening socket: {}'.format(self.peer))
        self.sendSizes()
        self.sendCells()

    def onClose(self, wasClean, code, reason):
        print('closing socket: {} {} {} {}'
              .format(self.peer, wasClean, code, reason))

    def onMessage(self, payload, isBinary):
        if isBinary:
            self.processMessage(payload)

    def connectionLost(self, reason):
        WebSocketServerProtocol.connectionLost(self, reason)
        self.factory.unregister(self)

    ### Support functions

    def sendProtobufData(self, data):
        print(data)
        self.sendMessage(data.SerializeToString(), isBinary = True)

    def sendOk(self, reply):
        sMessage = protocol.SMessage()
        sMessage.Status.ok = True
        sMessage.Status.replyTo = reply
        self.sendProtobufData(sMessage)

    def sendError(self, reply, error):
        sMessage = protocol.SMessage()
        sMessage.Status.ok = False
        sMessage.Status.replyTo = reply
        sMessage.Status.error = error
        self.sendProtobufData(sMessage)

    def sendSizes(self):
        sMessage = protocol.SMessage()
        sMessage.Sizes.CopyFrom(db.getSizes())
        self.sendProtobufData(sMessage)

    def sendCells(self):
        sMessage = protocol.SMessage()
        sMessage.Cells.CopyFrom(db.getCells())
        self.sendProtobufData(sMessage)

    ### Main logic

    def processMessage(self, msg):
        cMessage = protocol.CMessage()
        cMessage.ParseFromString(msg)

        print(cMessage)

        if cMessage.HasField('CellsUpdate'):
            db.updateCells(cMessage.CellsUpdate)
            self.sendOk(reply='CellsUpdate')
        elif cMessage.HasField('Sizes'):
            self.sendSizes()
        else:
            self.sendError(reply='', error='Unknown message')
            print('unknown message')


class TablesServerFactory(WebSocketServerFactory):
    def __init__(self, url):
        WebSocketServerFactory.__init__(self, url)

if __name__ == '__main__':
    import asyncio

    factory = TablesServerFactory(u'ws://127.0.0.1:9000')
    factory.protocol = TablesServerProtocol

    loop = asyncio.get_event_loop()
    coro = loop.create_server(factory, '0.0.0.0', 9000)
    server = loop.run_until_complete(coro)

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.close()
        loop.close()
