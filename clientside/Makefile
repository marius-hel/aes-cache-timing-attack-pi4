SRC_PATH=src
BIN_PATH=bin
RES_PATH=results
TOOLS_PATH=tools

DEFAULT_PROBABILITY=1.0

# The environment variable TARGET_IP has to be set by the user before !

### Build ###

build:
	mkdir -p ${BIN_PATH}
	gcc -O3 -o ${BIN_PATH}/study ${SRC_PATH}/study.c -lm
	gcc -O3 -o ${BIN_PATH}/ciphertext ${SRC_PATH}/ciphertext.c
	gcc -O3 -o ${BIN_PATH}/correlate ${SRC_PATH}/correlate.c -lm
	gcc -O3 -o ${BIN_PATH}/search ${SRC_PATH}/search.c -lcrypto -Wno-deprecated-declarations -Wno-unused-result

clear_bin:
	rm -rf ${BIN_PATH}


### Run ###

study:
	mkdir -p ${RES_PATH}
ifeq (${TARGET_IP},)
	@echo "Error: Target's IP is not set ! Please type:\n"
	@echo "export TARGET_IP=<Target's IP>\n"
	@echo "Advice: check connection before launching the attack:"
	@echo "ping <Target's IP>\n"
	@exit 1
else
	@echo "Target IP is ${TARGET_IP}"
endif
	${BIN_PATH}/study ${TARGET_IP} 800 > ${RES_PATH}/study.800

attack:
	mkdir -p ${RES_PATH}
	${BIN_PATH}/study ${TARGET_IP} 800 > ${RES_PATH}/attack.800

cyphertext:
	mkdir -p ${RES_PATH}
	${BIN_PATH}/ciphertext ${TARGET_IP}  > ${RES_PATH}/attack
	cat ${RES_PATH}/attack

correlate:
	(tail -4096 ${RES_PATH}/study.800; tail -4096 ${RES_PATH}/attack.800) \
	| ${BIN_PATH}/correlate >> ${RES_PATH}/attack

search:
	${BIN_PATH}/search < ${RES_PATH}/attack


### Analyze ###

# Plots the average cycle count with regard to each possible value (0 to 255) 
# for each byte of the input block (plaintext[0] to plaintext[15]).
# Each subplot corresponds to one of the 16 bytes of the input plaintext block.
# The input data are the most recent values obtained with the study phase.
overview_study:
	tail -4096 ${RES_PATH}/study.800 > ${RES_PATH}/study_tmp
	python3 ${TOOLS_PATH}/overview.py ${RES_PATH}/study_tmp
	rm ${RES_PATH}/study_tmp

# Plots the average cycle count with regard to each possible value (0 to 255) 
# for each byte of the input block (plaintext[0] to plaintext[15]).
# Each subplot corresponds to one of the 16 bytes of the input plaintext block.
# The input data are the most recent values obtained with the attack phase.
overview_attack:
	tail -4096 ${RES_PATH}/attack.800 > ${RES_PATH}/attack_tmp
	python3 ${TOOLS_PATH}/overview.py ${RES_PATH}/attack_tmp
	rm ${RES_PATH}/attack_tmp

# Shows a representation of the 'supposed known bits' of the private key.
# If a bit is known, his value ('1' or '0') is printed. Else, '_' is printed.
# The bits are grouped in bytes. k[0] ... k[15] ar represented from left to right
# with MSB first.
# User can manually execute the script with custom probability as threshold.
show_known_bits:
	tail -16 ${RES_PATH}/attack > ${RES_PATH}/attack_tmp
	python3 ${TOOLS_PATH}/known_bits.py ${RES_PATH}/attack_tmp ${DEFAULT_PROBABILITY}
	rm ${RES_PATH}/attack_tmp
