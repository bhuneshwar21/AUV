#! /bin/bash
PROGRAM=/home/pi/AUV/comm/lnccs/lnccs
Trigger=/home/pi/AUV/comm/lnccs/serialTrigger
TMOUT=1s
T2=2s
T3=3s
echo "Wait for water!!!"
$Trigger 
echo "Water detected, Camera will On"&
#PROCESSNAME=lnccs  # name of the process to be monitored
#MYPID=`pidof $PROCESSNAME`

function run_with_tmout {
  $PROGRAM &
  PID=$!
  echo "Cam On" &
  PROCESSNAME=lnccs
  MYPID=`pidof $PROCESSNAME`
  Rss=`echo 0 $(cat /proc/$MYPID/smaps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;` &
for ((;Rss<200000;));do
  PROCESSNAME=lnccs
  MYPID=`pidof $PROCESSNAME`
  Rss=`echo 0 $(cat /proc/$MYPID/smaps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;` 
#  echo "Rss: " $Rss &
  sleep $T3
done
  sleep $TMOUT  
  kill -9 $PID &
  Rss=0;
  sleep $T2
}

for ((i=0; i < 10000; i++)); do 
    run_with_tmout #./myprog 100 &
done

# wait for all child processes to end
wait  && echo "all done"
