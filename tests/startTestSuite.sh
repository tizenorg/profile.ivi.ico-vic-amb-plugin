#!/bin/sh

echo "==========Prepare Test ico-vic-amb-plugin=========="
./testVehiclePluginWSApp 2>&1 | tee wslog &
sleep 10
echo "==========Start Test ico-vic-amb-plugin=========="
./testVehiclePluginMW 2>&1 |tee mwlog
