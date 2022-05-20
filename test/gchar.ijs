prolog './gchar.ijs'
NB. char signed vs unsigned
1[0 : 0
signed char   - default char on windows/linux/darwin compilers
unsigned char - default char on ARM (except iOS)
previous tests ran clean with this compiler difference
tests below catch the difference

jtype.h has: typedef char C;

perhaps it should be: typedef signed char C;

but the above change causes problems on raspbian
so I am not willing to take that leap right now

a compiler option should be used to change the
compiler default from unsigned char to be signed char

raspbian build is done with compiler option fsigned-char
)

9082 32 215 32 9077 = 3 u: 7 u: 226 141 186 32 195 151 32 226 141 181{a.

epilog''

