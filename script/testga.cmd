@rem test windows on github actions

dir j64

j64\jconsole -lib j.dll -jprofile testga.ijs
j64\jconsole -lib javx.dll -jprofile testga.ijs
j64\jconsole -lib javx2.dll -jprofile testga.ijs
