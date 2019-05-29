vpath % $(jgit)/jsrc

%.o: %.c
	$(CC) -c $(CFLAGS) $(CFLAGS_SIMD) $(CFLAGS_FMA) -o$@ $^

CFLAGS=$(COMPILE)
$(OBJS_FMA) : CFLAGS_FMA := -mfma 
$(TARGET) : $(OBJS) $(OBJS_FMA)
	$(CC)  $(OBJS) $(OBJS_FMA) $(LINK)
