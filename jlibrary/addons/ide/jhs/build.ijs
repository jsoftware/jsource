NB. build

mkdir_j_ jpath '~addons/ide/jhs/config'
mkdir_j_ jpath '~addons/ide/jhs/demo'
mkdir_j_ jpath '~addons/ide/jhs/js'

3 : 0''
T=. jpath '~addons/ide/jhs/'
S=. jpath '~Addons/ide/jhs/'

mkdir_j_ each T&, each cutopen 'config demo'

f=. {."1 dirtree S
t=. T&, each (#S) }.each f
empty t fcopynew each f
)
