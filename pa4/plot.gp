set term postscript eps color 
set output 'cwnd.eps'
set ylabel 'cwnd'
set xlabel 'time'
plot 'tcp-example.cwnd' using 1:2 with lines title 'cwnd vs time'