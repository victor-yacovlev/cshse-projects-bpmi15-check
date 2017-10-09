# -*- coding: utf-8 -*-

import random
import string

import tables_pb2 as protocol


def getSizes():
    sSizes = protocol.SSizes()
    for i in range(10000):
        row = sSizes.rows.add()
        row.height = 20
    for i in range(2000):
        row = sSizes.columns.add()
        row.width = random.randrange(80, 250)

    return sSizes

def getCells():
    sCells = protocol.SCells()
    for i in random.sample(range(100), 20):
        for j in random.sample(range(100), 20):
            cell = sCells.cells.RandomCellGroup.cells.add()
            cell.x = i
            cell.y = j
            cell.cell.text = 'some text'

    return sCells

def updateCells(cells):
    return 'ok'
