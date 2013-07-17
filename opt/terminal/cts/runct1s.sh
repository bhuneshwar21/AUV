#! /bin/bash
PROGRAM=/home/pi/AUV/opt/terminal/cts/ct1s
TMOUT=1s
T2=2s
T3=3s

function run_with_tmout {
  $PROGRAM &
  PID=$!
  echo "Cam On" &
  PROCESSNAME=ct1s
  MYPID=`pidof $PROCESSNAME`
  Rss=`echo 0 $(cat /proc/$MYPID/smaps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;` &
for ((;Rss<200000;));do
  PROCESSNAME=ct1s
  MYPID=`pidof $PROCESSNAME`
  Rss=`echo 0 $(cat /proc/$MYPID/smaps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;` 
  echo "Rss: " $Rss &
  sleep $T3
done
  echo "Rss: " $Rss &
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
