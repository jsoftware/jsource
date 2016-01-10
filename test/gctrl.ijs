NB. control word parsing ------------------------------------------------

f0 =. 3 : 'if. if. 1 do. 2 end. do. 3 else. 4 end.'
f1 =. 3 : 'if. while. 1 do. 2 end. do. 3 end.'
f2 =. 3 : 'if. try. 1 catch. 2 end. do. 3 else. 4 end.'
f3 =. 3 : 'if. 1 do. while. 2 do. 3 end. end.'

g0 =. 3 : 'while. try. 1  catch. 2 end. do. 3 end.'
g1 =. 3 : 'while. 0 try. 1  catch. 2 end. do. 3 end.'
g2 =. 3 : 'while. if. 1 do. 2 else. 3 end. do. 4 end.'
g3 =. 3 : 'while. 1 do. if. 2 do. 3 else. 4 end. end.'
g4 =. 3 : 'while. 0 if. 1 do. break.    end. 2 do. 3 end.' 
g5 =. 3 : 'while. 0 if. 1 do. continue. end. 2 do. 3 end.' 


NB. control words -------------------------------------------------------

test =. 3 : '''control error'' -: ". etx ''3 : y'''

test 'if.'
test 'else.'
test 'elseif.'
test 'end.'
test 'do.'
test 'try.'
test 'catch.'
test 'while.'
test 'whilst.'
test 'continue.'
test 'break.'
test 'goto_.'
test 'goto_nonexist.'

test 'do. 1 2 3 end.'
test 'else. 1 2 3 end.'
test 'elseif. 1 do. 2 end.'
test 'end. 1'
test '1 end.'
test '1 end. 2'
test 'if. 1 end.'
test 'if. 1 do.'
test 'if. 1 do. 2 elseif. 3 end.'
test 'if. 1 do. 2 do. 3 end.'
test 'while. 1 do.'
test 'while. 1 end.'
test 'whilst. 1 do.'
test 'whilst. 1 end.'
test 'try. ''without catch.'''
test 'try. 1 catch.'
test 'try. 1 end.'

test 'if. y do. continue. end.'
test 'if. y do. break.    end.'
test 'if. y do. return.'

test 'if. 1 do. 2 elseif. 3 do. 4 else. 5 end.'
test 'if. 1 do. 2 else. if. 3 do. 4 else. 5 end.'
test 'while.  1 do. 2 else. 3 end.'
test 'whilst. 1 do. 2 else. 3 end.'

'spelling error' -: ex '3 : ''goto.'''
'spelling error' -: ex '3 : ''label.'''
'spelling error' -: ex '3 : ''repeat.'''
'spelling error' -: ex '3 : ''until.'''
'spelling error' -: ex '3 : ''begin.'''

4!:55 ;:'f0 f1 f2 f3 g0 g1 g2 g3 g4 g5 test '


