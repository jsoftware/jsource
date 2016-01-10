NB. initialise system default ctag and define z locale names

coclass 'jadetag'
coinsert 'ctag'

Tags=: Tags_j_
absolutepath=: 1
cleartags''
nms=. <;._1 ' cleartags loadtags ta tagcp tagopen tagselect'
". > nms ,each (<'_z_=:') ,each nms ,each (<'_jadetag_')
