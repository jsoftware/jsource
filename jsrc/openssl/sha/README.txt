Common usage pattern is to invoke script and pass "flavour" and output
file name as command line arguments. "Flavour" refers to ABI family or
specfic OS. E.g. x86_64 scripts recognize 'elf', 'elf32', 'macosx',
'mingw64', 'nasm'. PPC scripts recognize 'linux32', 'linux64',
'linux64le', 'aix32', 'aix64', 'osx32', 'osx64'. And so on... Some
x86_64 scripts even examine CC environment variable in order to
determine if AVX code path should be generated. ["AVX" refers to *all*
AVX versions.]

clone repos
https://github.com/openssl/openssl.git

run sha-asm.sh in the crypto/sha/asm folder there
