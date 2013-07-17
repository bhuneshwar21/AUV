#! /bin/bash
PROGRAM=~/AUV/ubu/cam2_3/opt2
TMOUT=2m
T2=2s
T3=3s


function run_with_tmout {
  $PROGRAM &
  PID=$! 
  echo "Cam On" &
  PROCESSNAME=opt2
  MYPID=`pidof $PROCESSNAME`
#  Rss=`echo 0 $(cat /proc/$MYPID/maps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;` &
#  Rss=ps -o rss $(pgrep PROCESSNAME) &
#for ((;Rss<200000;));do
#  PROCESSNAME=opt2
#  MYPID=`pidof $PROCESSNAME`
#  Rss=`echo 0 $(cat /proc/$MYPID/maps  | grep Rss | awk '{print $2}' | sed 's#^#+#') | bc;` 
#  echo "Rss: " $Rss &
  sleep $T3
#done
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
