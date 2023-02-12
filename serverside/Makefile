SRC_PATH=src
BIN_PATH=bin


### Build ###

build:
	mkdir -p ${BIN_PATH}
	gcc -O3 -o ${BIN_PATH}/server ${SRC_PATH}/server.c -lcrypto -Wno-deprecated-declarations

clear_bin:
	rm -rf ${BIN_PATH}


### Run ###

server_zero_key:
	${BIN_PATH}/server 0.0.0.0 < /dev/zero

new_private_key:
	mkdir -p private
	dd if=/dev/urandom of=private/key bs=16 count=1
	echo "Secret key:"
	hexdump private/key

server_private_key:
	${BIN_PATH}/server 0.0.0.0 < private/key