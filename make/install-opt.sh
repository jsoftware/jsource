#!/bin/bash
# install jbld folders - new install or a reinstall

OPT=/opt/J
ROOT=..

rm -fr $OPT
mkdir $OPT
mkdir $OPT/j32
mkdir $OPT/j64
cp -r $ROOT/build/j32/* $OPT/j32
cp -r $ROOT/build/j64/* $OPT/j64
cat <<EOF >$OPT/j32/bin/jhs
#!/bin/sh
jconsole ~addons/ide/jhs/core.ijs -js " init_jhs_'' "
EOF
cat <<EOF >$OPT/j64/bin/jhs
#!/bin/sh
jconsole ~addons/ide/jhs/core.ijs -js " init_jhs_'' "
EOF
chmod +x $OPT/j32/bin/jhs $OPT/j64/bin/jhs

echo "install complete"
echo ""
echo "$ROOT/license.txt"

cat $ROOT/license.txt
exit 0

