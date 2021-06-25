@rem test windows on github actions

cd j64
dir

jconsole -lib j.dll -jprofile ver.ijs
jconsole -lib javx.dll -jprofile ver.ijs
jconsole -lib javx2.dll -jprofile ver.ijs
