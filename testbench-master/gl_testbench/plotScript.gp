reset

set key autotitle columnhead
unset key 

plot "data16.txt" using 1:2 with lines,\
"" using 1:3 with lines,\
"data32.txt" using 1:3 with lines,\
"data6.txt" using 1:3 with lines,\
"data6.txt" using 1:2 with lines