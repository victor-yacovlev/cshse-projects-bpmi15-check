<template>
  <div id="app" class="container">
    <div id="formula" width=100% height=30px contenteditable="true" placeholder="empty"></div>
    <div id="canvas" width=100% height=100%></div>
  </div>
</template>

<script>
import { state, bus } from '@/globs'
import * as $ from 'jquery'

export default {
  data () {
    return {
      state: state,
      canvas: undefined,
      context: undefined,
      table: { cells: { }, rows: [], columns: [] },
      selected: { row: 0, col: 0 },
      offset: { x: 0, y: 0 },
      edges: { right: 0, bottom: 0 },
      version: process.env.VERSION + ' (' + process.env.COMMIT + ')'
    }
  },
  mounted () {
    this.initCanvas()
  },
  methods: {
    drawBoard () {
      function toAZ (number) {
        let res = ''
        if (number === 0) {
          return 'A'
        }
        while (number > 0) {
          res = String.fromCharCode(65 + (number - 1) % 26) + res
          number = Math.floor((number - 1) / 26)
        }
        return res
      }

      // grid width and height
      let bw = $(document).width()
      let bh = $(document).height()
      // size of canvas
      let cw = bw
      let ch = bh

      let hColumnWidth = 45
      let hRowHeight = 25

      this.context.clearRect(0, 0, this.canvas.width, this.canvas.height)
      let width
      let height

      // draw headers
      width = hColumnWidth
      for (let col = this.offset.x; width < cw && col < this.table.columns.length; col++) {
        this.context.beginPath()
        this.context.lineWidth = 0.07
        if (col === this.selected.col) {
          this.context.fillStyle = '#DEDEDE'
        } else {
          this.context.fillStyle = '#F1F1F1'
        }
        this.context.rect(width + 0.5, 0.5, this.table.columns[col].width, hRowHeight)
        this.context.fill()
        this.context.textAlign = 'center'
        this.context.font = '12px Roboto'
        this.context.fillStyle = '#000000'
        this.context.fillText(toAZ(col + 1), width + this.table.columns[col].width / 2, hRowHeight / 1.5)
        this.context.stroke()

        width += this.table.columns[col].width
      }

      height = hRowHeight
      for (let row = this.offset.y; height < ch && row < this.table.rows.length; row++) {
        this.context.beginPath()
        this.context.lineWidth = 0.07
        if (row === this.selected.row) {
          this.context.fillStyle = '#DEDEDE'
        } else {
          this.context.fillStyle = '#F1F1F1'
        }
        this.context.rect(0.5, height + 0.5, hColumnWidth, this.table.rows[row].height)
        this.context.fill()
        this.context.textAlign = 'center'
        this.context.font = '12px Roboto'
        this.context.fillStyle = '#000000'
        this.context.fillText(row + 1, hColumnWidth / 2, height + this.table.rows[row].height / 1.5)
        this.context.stroke()

        height += this.table.rows[row].height
      }

      width = hColumnWidth
      for (let col = this.offset.x; width < cw && col < this.table.columns.length; col++) {
        height = hRowHeight
        for (let row = this.offset.y; height < ch && row < this.table.rows.length; row++) {
          this.context.beginPath()
          this.context.rect(width + 0.5, height + 0.5, this.table.columns[col].width, this.table.rows[row].height)
          this.context.textAlign = 'center'
          this.context.font = '14px Roboto'
          if (row in this.table.cells && col in this.table.cells[row] && 'text' in this.table.cells[row][col]) {
            let text = this.table.cells[row][col]['text']
            this.context.fillText(text, width + this.table.columns[col].width / 2, height + this.table.rows[row].height - 4)
          }
          if (row === this.selected.row && col === this.selected.col) {
            this.context.lineWidth = 2.0
            this.edges.right = width + this.table.columns[col].width
            this.edges.bottom = height + this.table.rows[row].height
          } else {
            this.context.lineWidth = 0.05
          }
          this.context.stroke()

          // this.table.cells[row][col].currentX = width
          // this.table.cells[row][col].currentY = height

          height += this.table.rows[row].height
        }

        width += this.table.columns[col].width
      }
    },
    initCanvas () {
      let that = this

      // grid width and height
      let bw = $(document).width()
      let bh = $(document).height() - 30 // bad
      // padding around grid
      // let p = 10
      // size of canvas
      let cw = bw
      let ch = bh

      log.debug(cw, ch)

      let canvas = $('<canvas/>').attr({width: cw, height: ch}).appendTo('#canvas')
      this.canvas = canvas.get(0)
      // console.log(canvas)
      this.context = this.canvas.getContext('2d')

      this.canvas.addEventListener('onresize', function (event) { that.initCanvas() })

      this.canvas.addEventListener('click', function (event) {
        let x = event.pageX - that.canvas.offsetLeft
        let y = event.pageY - that.canvas.offsetTop

        let row = 0
        let col = 0

        let hColumnWidth = 45
        let hRowHeight = 25

        let foundRow = false
        let foundCol = false

        for (let width = hColumnWidth; width < cw && col < that.table.columns.length; col++) {
          if (x >= width && x <= width + that.table.columns[col].width) {
            foundCol = true
            break
          }
          width += that.table.columns[col].width
        }
        for (let height = hRowHeight; height < ch && row < that.table.rows.length; row++) {
          if (y >= height && y <= height + that.table.rows[row].height) {
            foundRow = true
            break
          }
          height += that.table.rows[row].height
        }
        log.debug(x, y)
        if (foundRow && foundCol) {
          that.selected.row = row
          that.selected.col = col
          that.writeContent()
          that.drawBoard()
        }
      }, false)

      window.addEventListener('keydown', function (e) {
        // space, page up, page down, arrow, delete, enter keys:
        log.debug(e.keyCode)
        if ([32, 33, 34, 37, 38, 39, 40, 46, 13].indexOf(e.keyCode) > -1) {
          e.preventDefault()
        }
        switch (e.keyCode) {
          case 37:
            if (that.selected.col > 0) {
              that.parseContent()
              that.selected.col -= 1
              if (that.selected.col < that.offset.x) {
                that.offset.x -= 1
              }
              that.drawBoard()
              that.writeContent()
            }
            break
          case 38:
            if (that.selected.row > 0) {
              that.parseContent()
              that.selected.row -= 1
              if (that.selected.row < that.offset.y) {
                that.offset.y -= 1
              }
              that.drawBoard()
              that.writeContent()
            }
            break
          case 39:
            if (that.selected.col < that.table.columns.length - 1) {
              that.parseContent()
              that.selected.col += 1
              while (that.edges.right + that.table.columns[that.selected.col].width > that.canvas.width) {
                that.edges.right -= that.table.columns[that.offset.x].width
                that.offset.x += 1
              }
              that.drawBoard()
              that.writeContent()
            }
            break
          case 13:
          case 40:
            if (that.selected.row < that.table.rows.length - 1) {
              that.parseContent()
              that.selected.row += 1
              while (that.edges.bottom + that.table.rows[that.selected.row].height > that.canvas.height) {
                that.edges.bottom -= that.table.rows[that.offset.y].height
                that.offset.y += 1
              }
              that.drawBoard()
              that.writeContent()
            }
            break
          case 46:
            that.setFormula('')
            break
        }
      }, false)

      $('#formula').keydown(function (e) {
        if (e.keyCode === 13) {
          that.parseContent()
          return false
        }
      })

      bus.$on('sizes', function (sizes) {
        that.table['rows'] = sizes.rows
        that.table['columns'] = sizes.columns
        that.drawBoard()
      })

      bus.$on('cells', function (cells) {
        for (let i in cells.RandomCellGroup.cells) {
          let cell = cells.RandomCellGroup.cells[i]
          if (!(cell.x in that.table.cells)) {
            that.table.cells[cell.x] = { }
          }
          that.table.cells[cell.x][cell.y] = {'text': cell.cell.text}
          log.debug(cell)
        }
        that.writeContent()
        that.drawBoard()
      })
    },
    writeContent () {
      if (this.selected.row in this.table.cells &&
          this.selected.col in this.table.cells[this.selected.row] &&
          'text' in this.table.cells[this.selected.row][this.selected.col]
      ) {
        this.setFormula(this.table.cells[this.selected.row][this.selected.col]['text'])
      } else {
        this.setFormula('')
      }
    },
    parseContent () {
      if (!(this.selected.row in this.table.cells)) {
        this.table.cells[this.selected.row] = { }
      }
      if (!(this.selected.col in this.table.cells[this.selected.row])) {
        this.table.cells[this.selected.row][this.selected.col] = { }
      }
      if (this.table.cells[this.selected.row][this.selected.col]['text'] !== this.getFormula() &&
          !(typeof this.table.cells[this.selected.row][this.selected.col]['text'] === 'undefined' &&
            this.getFormula() === '')) {
        bus.$emit('send', 'CellsUpdate', {
          cells: {
            RandomCellGroup: {
              cells: [{
                x: this.selected.row,
                y: this.selected.col,
                cell: {
                  text: this.getFormula()
                }
              }]
            }
          }
        })
      }
      this.table.cells[this.selected.row][this.selected.col]['text'] = this.getFormula()
      this.drawBoard()
    },
    setFormula (text) {
      $('#formula').text(text)
      this.parseContent()
    },
    getFormula () {
      return $('#formula').text()
    }
  }
}
</script>

<style>
  html, body, .rview {
    height: 100%;
    overflow: hidden;
  }

  body {
    margin: 0;
    display: flex;
  }

  canvas {
    display: block;
    position: absolute;
    top: 25px;
    left: 0;
    right: 0;
    bottom: 0;
    width: 100%;
    height: 100%;
  }

  #formula {
    margin-left: 10px;
    margin-top: 2px;
  }

  [contenteditable]:focus {
    outline: 0px solid transparent;
  }

  [placeholder]:empty:before {
      content: attr(placeholder);
      color: #555;
  }

  [placeholder]:empty:focus:before {
      content: "";
  }
</style>
