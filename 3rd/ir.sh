#!/bin/bash

echo "#include <stdint.h>" > 3rd/IRremote.h

cat 3rd/Arduino-IRremote/IRremote.h | \
	sed \
		-e 's/(decode_results/(struct decode_results/' \
		-e 's/class/struct/' \
		-e 's/public://' \
		-e 's/USECPERTICK 50/USECPERTICK 1/' \
	| \
	awk '/struct IRrecv/{a=1} {if(a==1){print "//" $0}else{print $0}} /^}/{a=0} ' | \
	awk '/struct IRsend/{a=1} {if(a==1){print "//" $0}else{print $0}} /^}/{a=0} ' | \
	sed \
		-e 's/\/\/  int getRC/  int getRC/' \
		-e 's/\/\/  int decode/  int decode/' \
		-e 's/\/\/  long decode/  long decode/' \
	>> 3rd/IRremote.h
cat 3rd/Arduino-IRremote/IRremoteInt.h | \
	grep -v "#include" | \
	sed \
		-e 's/int MATCH/static int MATCH/' \
	>> 3rd/IRremote.h

cat 3rd/Arduino-IRremote/IRremote.cpp | \
	grep -v "IRremoteInt.h" | \
	grep -v "#include <avr" | \
	grep -v "resume();" | \
	sed \
		-e 's/class/struct/' \
		-e 's/#define FNV.*/\0ul/' \
		-e 's/public://' \
		-e 's/decode_results/struct decode_results/' \
		-e 's/for (int i/int i; for (i/' \
		-e 's/IRsend:://' \
		-e 's/if (\!MATCH_MARK(results->rawbuf\[offset\], NEC_HDR_MARK)) {/\0 if (!MATCH_MARK(results->rawbuf[offset], 4500))/' \
		-e 's/IRrecv:://' | \
	awk '/^void enable/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^void blink/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^void resume/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^ISR/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^IRrecv/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^void send/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^void mark/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' | \
	awk '/^void space/{a=1} {if(a!=1){print $0}} /^}/{a=0} ' \
	> 3rd/IRremote.c
