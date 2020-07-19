vpath % $(jgit)/jsrc $(jgit)/jsrc/openssl/sha $(jgit)/jsrc/openssl/sha/asm

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_SIMD) $(CFLAGS_FMA) $(CFLAGS_AESNI) $(CFLAGS_AESARM) -o$@ $^

%.o: %.S
	$(CC) $(GASM_FLAGS) -c -o $@ $<

CFLAGS=$(COMPILE)
$(OBJS_FMA) : CFLAGS_FMA := -mfma
$(OBJS_AESNI) : CFLAGS_AESNI := -maes
$(OBJS_AESARM) : CFLAGS_AESARM := -march=armv8-a+crc+crypto
$(TARGET) : $(OBJS) $(OBJS_FMA) $(OBJS_AESNI) $(OBJS_AESARM) $(SRC_ASM)
	$(CC)  $(OBJS) $(OBJS_FMA) $(OBJS_AESNI) $(OBJS_AESARM) $(SRC_ASM) $(LINK)
