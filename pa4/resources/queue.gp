set term postscript eps color
set output 'queue_delay.eps'
set ylabel 'Queue delay'
set xlabel 'Time'
plot 'queue_delay.txt' using 1:2 with lines title 'Queue delay'
