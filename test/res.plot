set datafile separator ";"
set autoscale fix
set key inside top left

set ylabel 'GroundSteering'
set xlabel 'Time'

set terminal pngcairo size 2400,1080 nocrop enhanced
set grid

set style line 1 lc rgb '#E41A1C' pt 1 ps 1 lt 1 lw 2 # red
set style line 2 lc rgb '#377EB8' pt 6 ps 1 lt 1 lw 2 # blue
set style line 3 lc rgb '#00ffdd' pt 6 ps 1 lt 1 lw 2 # green

set style line 11 lc rgb '#000000' lt 1 lw 3
set border 0 back ls 11
set tics out nomirror

plot results using 2:3 title "Original" with lines ls 1, \
    	results using 2:4 title "Current" with lines ls 2, \
    	prev_results using 2:4 title "Previous" with lines ls 3,

