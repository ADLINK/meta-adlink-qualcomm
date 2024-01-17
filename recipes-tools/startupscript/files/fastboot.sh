#/bin/sh 

function ProgressBar {
# Process data
    let _progress=(${1}*100/${2}*100)/100
    let _done=(${_progress}*4)/10
    let _left=40-$_done
# Build progressbar string lengths
    _fill=$(printf "%${_done}s")
    _empty=$(printf "%${_left}s")

printf "\rProgress : [${_fill// /* } ${_empty// /-}] ${_progress}%%" 
}

# Variables
_start=1

# This accounts as the "totalState" variable for the ProgressBar function
_end=2

echo "Preparing to enter fastboot mode"
task1= `dd if=/dev/zero of=/dev/sde11 > /dev/null 2>&1`
task2= `dd if=/dev/zero of=/dev/sde34 > /dev/null 2>&1`

printf '\nInitiate fastboot!\n'
# Proof of concept
for number in $(seq ${_start} ${_end})
do
    echo $task$number
    ProgressBar ${number} ${_end}
    sleep 1
done
reboot 
