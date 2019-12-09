set testSimulator [new Simulator]

set qf [open queue.tr w]

set tcptrack [open tcptrack.tr w]

proc finish {} {
	global tcptrack testSimulator
	$testSimulator flush-trace
	close $tcptrack
	exec xgraph tcptrack.tr -geometry 300x300 &
	exit 0
}

proc resetQueue {} {
	global testSimulator qm
	$qm reset
	set now [$testSimulator now]
	$testSimulator at [expr $now + 0.04] "resetQueue"
}

set Src [$testSimulator node]
set Router [$testSimulator node]
set Dst [$testSimulator node]

$testSimulator duplex-link $Src $Router 2Mbps 10ms DropTail
$testSimulator duplex-link $Router $Dst 1Mbps 10ms DropTail

$testSimulator  duplex-link-op $Router $Dst queuePos 0.04


$testSimulator color 1 red
$testSimulator color 2 SeaGreen
$testSimulator color 3 blue

$testSimulator queue-limit $Src $Router [expr (35%9) + 4]

set testTCP [new Agent/TCP]
$testSimulator attach-agent $Src $testTCP
$testTCP set packetSize_ 1000

set testTCPS [new Agent/TCPSink]
$testSimulator attach-agent $Dst $testTCPS

$testSimulator connect $testTCP $testTCPS

set ftp [new Application/FTP]
$ftp attach-agent $testTCP
$ftp set type_ FTP

set qm [$testSimulator monitor-queue $Router $Dst $qf 0.04]
[$testSimulator link $Router $Dst] queue-sample-timeout

$testTCP attach $tcptrack
$testTCP tracevar cwnd_


$testSimulator at 0.01 "$ftp start"
$testSimulator at 10.0 "$ftp stop"

$testSimulator at 11.0 "finish"

$testSimulator run
