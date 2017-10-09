import * as log from 'loglevel'
import * as protobuf from 'protobufjs'
import * as jsonDescr from './assets/proto.json'

import Vue from 'vue'
import ElementUI from 'element-ui'
import App from './App'

import 'element-ui/lib/theme-default/index.css'

import { state, bus } from './globs'

window.log = log
log.setLevel('debug')
log.info('Running version ' + process.env.VERSION + ', commit ' + process.env.COMMIT)

Vue.use(ElementUI)

Vue.config.productionTip = false
Vue.config.debug = true
Vue.config.devtools = true

window._state = state
window._bus = bus

/* eslint-disable no-new */
new Vue({
  el: '#app',
  template: '<App/>',
  components: { App }
})

const proto = protobuf.Root.fromJSON(jsonDescr)

const SMessage = proto.lookup('tables.SMessage')
const CMessage = proto.lookup('tables.CMessage')

let wsUrl = window.location.protocol === 'https:' ? 'wss' : 'ws'
wsUrl += '://'
if (process.env.NODE_ENV === 'development') {
  wsUrl += 'localhost:9000'
} else {
  wsUrl += window.location.host
}
wsUrl += '/ws'

let conn = new WebSocket(wsUrl)
conn.binaryType = 'arraybuffer'

bus.$on('send', function (type, msg) {
  let tmp = {}
  tmp[type] = msg
  let buffer = CMessage.encode(tmp).finish()
  conn.send(buffer)
})

conn.onopen = function (evt) {
  log.debug('WS connected')
  if (state.sessId) {
    bus.$emit('send', 'Session', {
      sessId: state.sessId,
      userId: state.userId
    })
  } else {
    state.loading = false
  }
}

conn.onclose = function (evt) {
  log.debug('WS closed')
  bus.$emit('snack-msg', 'Disconnected from server')
}

conn.onmessage = function (evt) {
  let raw = new Uint8Array(evt.data)
  if (!raw.length) return // ping

  let message = SMessage.decode(raw)
  log.debug('message', message)

  if (message.Status !== null) { // Status of our request
    log.debug(message.Status)

    if (!message.Status.ok) {
      log.error('Server said:', message.Status.error)
    }
  } else if (message.Cells !== null) {
    bus.$emit('cells', message.Cells.cells)
  } else if (message.Sizes !== null) {
    bus.$emit('sizes', message.Sizes)
  } else {
    log.error('Unknown message type', message)
  }
}
