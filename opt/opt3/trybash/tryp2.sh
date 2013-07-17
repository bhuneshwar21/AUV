#! /bin/bash
PROGRAM=~/AUV/opt/linetrack/opt2
TMOUT=10s
T2=1s

PROCESSNAME=opt2  # name of the process to be monitored
MYPID=`pidof $PROCESSNAME`

function run_with_tmout {
  $PROGRAM &
  PID=$!
  echo "Cam On" &
  sleep $TMOUT  
  kill -9 $PID &
  sleep $T2
}

for ((i=0; i < 3; i++)); do 
    run_with_tmout #./myprog 100 &
done

# wait for all child processes to end
wait  && echo "all done"
