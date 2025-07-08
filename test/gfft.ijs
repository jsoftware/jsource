prolog './gfft.ijs'
NB. FFT addon -----------------------------------------------------------

NB. obsolete s48=: 9!:48 ''
NB. obsolete 9!:49 ]1

load jpath '~addons/math/fftw/fftw.ijs'

x=: 1024 %~ ?100$1e5
1 >: +/ x ~:!.1e_12 ifftw fftw x
NB. obsolete 9!:49 s48





epilog''

