set testSimulator [new Simulator]

set qf [open queue_task2.tr w]

set tcptrack1 [open tcptrack1_task2.tr w]
set tcptrack2 [open tcptrack2_task2.tr w]
proc finish {} {
	global tcptrack1 testSimulator tcptrack2
	$testSimulator flush-trace
	close $tcptrack1
	close $tcptrack2
	exit 0
}

proc resetQueue {} {
	global testSimulator qm
	$qm reset
	set now [$testSimulator now]
	$testSimulator at [expr $now + 0.04] "resetQueue"
}

set N1 [$testSimulator node]
set N2 [$testSimulator node]
set N3 [$testSimulator node]
set N4 [$testSimulator node]

$testSimulator duplex-link $N1 $N3 2Mbps 10ms DropTail
$testSimulator duplex-link $N2 $N3 2Mbps 10ms DropTail
$testSimulator duplex-link $N3 $N4 1Mbps 10ms DropTail

$testSimulator queue-limit $N3 $N4 [expr (35%11) + 11]

set TCP1 [new Agent/TCP]
$testSimulator attach-agent $N1 $TCP1
$TCP1 set packetSize_ 1000

set TCP2 [new Agent/TCP]
$testSimulator attach-agent $N2 $TCP2
$TCP2 set packetSize_ 1000

set TCP1S [new Agent/TCPSink]
$testSimulator attach-agent $N4 $TCP1S

set TCP2S [new Agent/TCPSink]
$testSimulator attach-agent $N4 $TCP2S

$testSimulator connect $TCP1 $TCP1S
$testSimulator connect $TCP2 $TCP2S

set ftp1 [new Application/FTP]
$ftp1 attach-agent $TCP1
$ftp1 set type_ FTP

set ftp2 [new Application/FTP]
$ftp2 attach-agent $TCP2
$ftp2 attach-agent $TCP2

set qm [$testSimulator monitor-queue $N3 $N4 $qf 0.04]
[$testSimulator link $N3 $N4] queue-sample-timeout

$TCP1 attach $tcptrack1
$TCP1 tracevar cwnd_

$TCP2 attach $tcptrack2
$TCP2 tracevar cwnd_


$testSimulator at 0.01 "$ftp1 start"
$testSimulator at 0.1 "$ftp2 start"
$testSimulator at 10.0 "$ftp1 stop"
$testSimulator at 10.0 "$ftp2 stop"

$testSimulator at 11.0 "finish"

$testSimulator run
