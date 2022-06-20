all:
	#cd ../lib/picoruby ; $(MAKE) host_production_mrbc CFLAGS="-DMAX_REGS_SIZE=256 -DMAX_SYMBOLS_COUNT=500"
	$(CC) -O0 -g3 -Wall -Wpointer-arith -std=gnu99 \
	  -DHEAP_SIZE=1000000 \
	  -DMRBC_USE_HAL_POSIX \
	  -I../lib/picoruby/src/mrubyc/src \
	  -L../lib/picoruby/build/lib/host-production/alloc_mrbc \
	  -okeyboard_mock \
	  main.c \
	  -lmrubyc

clean:
	rm -f keyboard_mock
