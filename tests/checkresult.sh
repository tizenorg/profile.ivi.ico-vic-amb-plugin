#!/bin/sh

mkdir -p log

systemctl stop ambd.service
echo "=====Config Test1====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Failed to Load Configfile for VIC-Plugin" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 0 ] 
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf1

echo "=====Config Test2====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Failed to Load Configfile for VIC-Plugin" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf2

echo "=====Config Test3====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Failed to Load Configfile for VIC-Plugin" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 0 ] 
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf3

echo "=====Config Test4====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong2 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Can't find key\[\"Config\"\]" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] 
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf4

echo "=====Config Test5====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong3 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Can't load vehicle information" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] 
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf5

echo "=====Config Test6====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong4 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Can't load vehicle information" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] 
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf6

echo "=====Config Test7====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong5 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "WARNING \"KeyEventType\"  is not defined." /tmp/ambd.log | wc -l`
ERRORCHECK2=`grep "Subscribing to:  EngineSpeed" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] && [ $ERRORCHECK2 -eq 0 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf7

echo "=====Config Test8====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong6 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "WARNING Don't allow length of  \"KeyEventType\"'s value." /tmp/ambd.log | wc -l`
ERRORCHECK2=`grep "Subscribing to:  EngineSpeed" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] && [ $ERRORCHECK2 -eq 0 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf8

echo "=====Config Test9====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong7 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "WARNING \"AMBPropertyName\"  is not defined." /tmp/ambd.log | wc -l`
ERRORCHECK2=`grep "Subscribing to:  EngineSpeed" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] && [ $ERRORCHECK2 -eq 0 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf9

echo "=====Config Test10====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong8 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "WARNING \"Type\"  is not defined." /tmp/ambd.log | wc -l`
ERRORCHECK2=`grep "Subscribing to:  EngineSpeed" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] && [ $ERRORCHECK2 -eq 0 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf10

echo "=====Config Test11====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong9 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "WARNING \"Type\"'s value  is not defined." /tmp/ambd.log | wc -l`
ERRORCHECK2=`grep "Subscribing to:  EngineSpeed" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] && [ $ERRORCHECK2 -eq 0 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf11

echo "=====Config Test12====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong10 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Can't load vehicle information." /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf12

echo "=====Config Test13====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong11 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR \"DefaultInfoPort\"-> \"DataPort\"  is not defined." /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf13

echo "=====Config Test14====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong12 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR Failed to Load Configfile for VIC-Plugin" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 0 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf14

echo "=====Config Test15====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong13 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR object value separator ',' expected" /tmp/ambd.log | wc -l`
ERRORCHECK2=`grep "ERROR Failed to Load Configfile for VIC-Plugin" /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ] && [ $ERRORCHECK2 -eq 1 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf15

echo "=====Config Test16====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong14 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR \"Config\" is not array." /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf16

echo "=====Config Test17====="
systemctl restart murphyd.service
ambd -D -c/usr/local/tests/ico/etc/ambd/config.wrong15 -d5 -l/tmp/ambd.log
sleep 2
killall -15 ambd >/dev/null 2>&1
sleep 1
killall -9 ambd >/dev/null 2>&1
ERRORCHECK=`grep "ERROR \"VehicleInfoDefine\"  is not array." /tmp/ambd.log | wc -l`
if [ $ERRORCHECK -eq 1 ]
then
	echo "OK!"
else
	echo "NG"
fi
mv /tmp/ambd.log ambdlog_conf17

systemctl restart murphyd.service
systemctl start ambd.service

sleep 3

echo "=====Test1====="
./DBustest1 >applog 2>/dev/null &
sleep 1
./VICtest1 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

VICSET=`grep -c SET viclog`
VICGET=`grep -c GET viclog`
APPNOTI=`grep -c NOTIFY applog`
if [ $VICSET -ne $VICGET ]
then
	echo "NG"
else
	echo "OK!"
fi
if [ $VICSET -ne $APPNOTI ]
then
	echo "NG"
else
	echo "OK!"
fi
mv viclog viclog1
mv applog applog1

./VICReset >/dev/null 2>&1

echo "=====Test2====="
./VICtest3 >viclog 2>/dev/null
./DBustest3 >applog 2>/dev/null &
sleep 3
killall -15 DBustest3 >/dev/null 2>&1
sleep 1
killall -9 DBustest3 >/dev/null 2>&1
sleep 1

VICSET=`grep -c SET viclog`
APPGET=`grep -c GET applog`
if [ $VICSET -ne $APPGET ]
then
	echo "NG"
else
	echo "OK!"
fi
mv viclog viclog3
mv applog applog3

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standarddata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 0 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
awk -v standardvalue=1 -f checkinterspace.awk tmpapplog

mv viclog viclogis1
mv applog applogis1

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 1 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
awk -v standardvalue=1 -f checkinterspace.awk tmpapplog

mv viclog viclogis2
mv applog applogis2

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standarddata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 0 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis3
mv applog applogis3

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 2 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis4
mv applog applogis4

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standardata 1, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 1 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis5
mv applog applogis5

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standardata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 4 -n 0 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis6
mv applog applogis6

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standardata 3, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 3 -n 1 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis7
mv applog applogis7

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standardata 2, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 2 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis8
mv applog applogis8

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Standardata 1, Customdata 3)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 3 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis9
mv applog applogis9

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 1000, Customdata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 4 -i 1000 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis10
mv applog applogis10

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standarddata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 0 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog

mv viclog viclogis11
mv applog applogis11

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 1 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog

mv viclog viclogis12
mv applog applogis12

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standarddata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 0 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis13
mv applog applogis13

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 2 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis14
mv applog applogis14

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standardata 1, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 1 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis15
mv applog applogis15

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standardata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 4 -n 0 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis16
mv applog applogis16

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standardata 3, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 3 -n 1 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis17
mv applog applogis17

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standardata 2, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 2 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis18
mv applog applogis18

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Standardata 1, Customdata 3)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 3 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis19
mv applog applogis19

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 500, Customdata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 4 -i 500 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.5 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis20
mv applog applogis20

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standarddata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 0 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog

mv viclog viclogis21
mv applog applogis21

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 1 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog

mv viclog viclogis22
mv applog applogis22

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standarddata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 0 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis23
mv applog applogis23

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 2 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis24
mv applog applogis24

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standardata 1, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 1 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis25
mv applog applogis25

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standardata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 4 -n 0 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis26
mv applog applogis26

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standardata 3, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 3 -n 1 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis27
mv applog applogis27

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standardata 2, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 2 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis28
mv applog applogis28

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Standardata 1, Customdata 3)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 3 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis29
mv applog applogis29

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 100, Customdata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 4 -i 100 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.1 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis30
mv applog applogis30

echo "=====Interspace Test(Interspace 50, Standarddata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 0 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog

mv viclog viclogis31
mv applog applogis31

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 1 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog

mv viclog viclogis32
mv applog applogis32

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Standarddata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 0 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis33
mv applog applogis33

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 2 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis34
mv applog applogis34

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Standardata 1, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 1 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis35
mv applog applogis35

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Standardata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 4 -n 0 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis36
mv applog applogis36

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Standardata 3, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 3 -n 1 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis37
mv applog applogis37

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Standardata 2, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 2 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis38
mv applog applogis38

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Standardata 1, Customdata 3)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 3 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis39
mv applog applogis39

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 50, Customdata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 4 -i 50 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.05 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis40
mv applog applogis40

echo "=====Interspace Test(Interspace 24, Standarddata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 0 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog

mv viclog viclogis41
mv applog applogis41

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 1 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f1,4 >tmpviclog
cat applog | grep "NOTIFY" | cut -d " " -f1,4 >tmpapplog
awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog

mv viclog viclogis42
mv applog applogis42

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Standarddata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 0 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis43
mv applog applogis43

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 2 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis44
mv applog applogis44

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Standardata 1, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 1 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis45
mv applog applogis45

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Standardata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 4 -n 0 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis46
mv applog applogis46

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Standardata 3, Customdata 1)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 3 -n 1 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis47
mv applog applogis47

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Standardata 2, Customdata 2)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 2 -n 2 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis48
mv applog applogis48

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Standardata 1, Customdata 3)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 1 -n 3 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis49
mv applog applogis49

./VICReset >/dev/null 2>&1

echo "=====Interspace Test(Interspace 24, Customdata 4)====="
./DBustest1 >applog  2>/dev/null &
sleep 1
./VICSeq -s 0 -n 4 -i 24 >viclog 2>/dev/null
sleep 3
killall -15 DBustest1 >/dev/null 2>&1
sleep 1
killall -9 DBustest1 >/dev/null 2>&1
sleep 1

cat viclog | grep "SET" | cut -d " " -f4 | sort | uniq >tmpviclist
while read line
do
	echo "Check Vehicle Property is $line"
	cat viclog | grep "SET" | cut -d " " -f1,4 | grep $line >tmpviclog
	cat applog | grep "NOTIFY" | cut -d " " -f1,4 | grep $line >tmpapplog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpviclog
	awk -v standardvalue=0.024 -f checkinterspace.awk tmpapplog
done < tmpviclist

mv viclog viclogis50
mv applog applogis50

./VICReset >/dev/null 2>&1

mv viclog* log/
mv applog* log/

#End script
