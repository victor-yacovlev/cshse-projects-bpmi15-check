all: build

deps:
	npm install

proto:
	node build/proto.js

build: deps proto
	npm run build

dev: proto
	npm run dev
