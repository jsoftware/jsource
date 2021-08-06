@rem test windows on github actions

dir j64

j64\jconsole -lib j.dll -jprofile ver.ijs
j64\jconsole -lib javx.dll -jprofile ver.ijs
j64\jconsole -lib javx2.dll -jprofile ver.ijs

j64\jconsole -lib javx.dll testga.ijs
