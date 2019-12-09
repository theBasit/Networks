set testSimulator [new Simulator]

set qf [open queue_task2.tr w]

set tcptrack [open tcptrack_task4.tr w]

proc finish {} {
	global tcptrack testSimulator
	$testSimulator flush-trace
	close $tcptrack
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

$testSimulator queue-limit $N3 $N4 [expr (35%18) + 10]

set TCP1 [new Agent/TCP]
$testSimulator attach-agent $N1 $TCP1
$TCP1 set packetSize_ 1000

set TCP1S [new Agent/TCPSink]
$testSimulator attach-agent $N4 $TCP1S

set udp [new Agent/UDP]
$testSimulator attach-agent $N2 $udp
set null [new Agent/Null]
$testSimulator attach-agent $N4 $null

$testSimulator connect $udp $null
$testSimulator connect $TCP1 $TCP1S

set ftp1 [new Application/FTP]
$ftp1 attach-agent $TCP1
$ftp1 set type_ FTP

set cbr [new Application/Traffic/CBR]
$cbr attach-agent $udp
$cbr set type_ CBR
$cbr set packet_size_ 1000
$cbr set rate_ 2Mb

set qm [$testSimulator monitor-queue $N3 $N4 $qf 0.04]
[$testSimulator link $N3 $N4] queue-sample-timeout

$TCP1 attach $tcptrack
$TCP1 tracevar cwnd_

$testSimulator at 0.01 "$ftp1 start"
$testSimulator at 5.0 "$cbr start"
$testSimulator at 15.0 "$cbr stop"
$testSimulator at 20.0 "$ftp1 stop"

$testSimulator at 22.0 "finish"

$testSimulator run
