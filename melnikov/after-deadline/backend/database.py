# -*- coding: utf-8 -*-

import time
import pymongo
import random

import tables_pb2 as protocol


class DB:
    __client__ = None
    __DBNAME__ = 'tables'

    def __init__(self):
        __HOST__ = 'localhost'
        __PORT__ = 27017

        if self.__client__ is None:
            DB.__client__ = pymongo.MongoClient(__HOST__, __PORT__)

    def __enter__(self):
        return self.__client__[DB.__DBNAME__]

    def __exit__(self, type, value, traceback):
        pass

def getSizes():
    sSizes = protocol.SSizes()

    with DB() as db:
        rows = []
        columns = []
        for row in db.rows.find():
            rows.append({
                'id': int(row['id']),
                'height': row['height']
            })
        columns.sort(key = lambda x: x['id'])
        for column in db.columns.find():
            columns.append({
                'id': int(column['id']),
                'width': column['width']
            })
        columns.sort(key = lambda x: x['id'])

    for dbRow in rows:
        row = sSizes.rows.add()
        row.height = dbRow['height']
    for dbColumn in columns:
        column = sSizes.columns.add()
        column.width = dbColumn['width']

    return sSizes

def getCells():
    sCells = protocol.SCells()
    with DB() as db:
        cells = []
        for cell in db.cells.find():
            cells.append({
                'x': int(cell['x']) - 1,
                'y': int(cell['y']) - 1,
                'text': cell['text']
            })
    for dbCell in cells:
        cell = sCells.cells.RandomCellGroup.cells.add()
        cell.x = dbCell['x']
        cell.y = dbCell['y']
        cell.cell.text = dbCell['text']

    return sCells

def updateCells(cells):
    with DB() as db:
        for cell in cells.cells.RandomCellGroup.cells:
            if cell.cell.text == '':
                db.cells.delete_one(
                    { "x": cell.x + 1, "y": cell.y + 1 }
                )
            else:
                db.cells.update_one(
                    { "x": cell.x + 1, "y": cell.y + 1 },
                    { "$set": {"text": cell.cell.text} },
                    upsert = True
                )
