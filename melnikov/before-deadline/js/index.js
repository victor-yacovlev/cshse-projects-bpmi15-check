function loadTable() {
  var table = {
    'columns': [
      {'width': 50},
      {'width': 80},
      {'width': 80},
      {'width': 80},
      {'width': 80},
      {'width': 80},
      {'width': 80},
      {'width': 80},
      {'width': 80}
    ],
    'rows': [
      {'height': 25},
      {'height': 20},
      {'height': 20},
      {'height': 20},
      {'height': 20},
      {'height': 20},
      {'height': 20},
      {'height': 20},
      {'height': 20},
      {'height': 20}
    ],
    'cells':[
      [{'value':'','borderTop':'1','borderLeft':'1'},{'value':'A','borderTop':'1','borderLeft':'1'},{'value':'B','borderTop':'1','borderLeft':'1'},{'value':'C','borderTop':'1','borderLeft':'1'},{'value':'D','borderTop':'1','borderLeft':'1'},{'value':'E','borderTop':'1','borderLeft':'1'},{'value':'F','borderTop':'1','borderLeft':'1'},{'value':'G','borderTop':'1','borderLeft':'1'},{'value':'H','borderTop':'1','borderLeft':'1'}],[{'value':'1','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'},{'value':'123','borderTop':'1','borderLeft':'1'}],[{'value':'2','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'},{'value':'124','borderTop':'1','borderLeft':'1'}],[{'value':'3','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'},{'value':'125','borderTop':'1','borderLeft':'1'}],[{'value':'4','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'}],[{'value':'5','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'}],[{'value':'6','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'}],[{'value':'7','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'}],[{'value':'8','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'}],[{'value':'9','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'},{'value':'126','borderTop':'1','borderLeft':'1'}]
    ]
  }

  var selected = {'row': 1, 'col': 1}

  //grid width and height
  var bw = $(document).width();
  var bh = $(document).height();
  //padding around grid
  var p = 10;
  //size of canvas
  var cw = bw;
  var ch = bh;

  var canvas = $('<canvas/>').attr({width: cw, height: ch}).appendTo('#table-container');
  canvas = canvas.get(0);
  var context = canvas.getContext("2d");

  canvas.addEventListener('click', function(event) {
    var x = event.pageX - canvas.offsetLeft,
        y = event.pageY - canvas.offsetTop;

    for (var col = 0; col < table.columns.length; col++) {
      for (var row = 0; row < table.rows.length; row++) {
        if (x >= table.cells[row][col].currentX &&
            x <  table.cells[row][col].currentX + table.columns[col].width &&
            y >= table.cells[row][col].currentY &&
            y <  table.cells[row][col].currentY + table.rows[row].height) {
          // alert(table.cells[row][col].value);
          if (row > 0 && col > 0) {
            selected.row = row;
            selected.col = col;
            drawBoard();
          }

          return;
        }
      }
    }

  }, false);

  function drawBoard() {
    context.clearRect(0, 0, canvas.width, canvas.height);
    var width = 0;
    for (var col = 0; width < cw && col < table.columns.length; col++) {
      var height = 0;
      for (var row = 0; height < ch && row < table.rows.length; row++) {
        context.beginPath();
        context.rect(width, height, table.columns[col].width, table.rows[row].height);
        context.textAlign="center";
        context.font="16px Arial";
        context.fillText(table.cells[row][col].value, width + table.columns[col].width/2, height + table.rows[row].height - 2);
        // context.fillStyle = 'yellow';
        // context.fill();
        if (row == selected.row && col == selected.col) {
          context.lineWidth = 3;
        } else {
          context.lineWidth = 2;
        }
        context.strokeStyle = 'black';
        context.stroke();

        table.cells[row][col].currentX = width;
        table.cells[row][col].currentY = height;

        height += table.rows[row].height;
      }

      width += table.columns[col].width;
    }

    context.strokeStyle = "black";
    context.stroke();
  }

  window.addEventListener("keydown", function(e) {
    // space, page up, page down and arrow keys:
    if ([32, 33, 34, 37, 38, 39, 40].indexOf(e.keyCode) > -1) {
        e.preventDefault();
    }
    switch(e.keyCode) {
      case 37:
        if (selected.col > 1) {
          selected.col -= 1;
          drawBoard();
        }
        break;
      case 38:
        if (selected.row > 1) {
          selected.row -= 1;
          drawBoard();
        }
        break;
      case 39:
        if (selected.col < table.columns.length - 1) {
          selected.col += 1;
          drawBoard();
        }
        break;
      case 40:
        if (selected.row < table.rows.length - 1) {
          selected.row += 1;
          drawBoard();
        }
        break;
    }
  }, false);

  drawBoard();
}