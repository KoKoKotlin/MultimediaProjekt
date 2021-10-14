set xlabel "Level"
set ylabel "Zeitintervall zwischen drop()"

l = 3

set term png font "Monospaced,20.0" size 1700,700
set output "level_speed.png"

set xrange [0:40]
set yrange [0:1]

set key
set key outside

f(x) = 1.0 -  log(x) * .285

plot f(x) title "t(l)" with lines lw l
