all: deps proto

deps:
	pip install --user -r requirements.txt

proto:
	protoc --python_out=. --proto_path=.. ../tables.proto
