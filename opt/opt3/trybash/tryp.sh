#! /bin/bash
PROGRAM=~/projects/pi/trackline/opt/opt2
TMOUT1=5s
TMOUT2=2s
$PROGRAM &
PID=$! &   
echo $PID &
sleep $TMOUT1
for ((i=0; i < 7; i++)); do 
    kill -9 $PID 
    sleep $TMOUT2
    $PROGRAM & 
    PID=$! &
    echo $PID &
    sleep $TMOUT1
    echo $i
done

# wait for all child processes to end
wait  && echo "all done"
