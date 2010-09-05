#!/bin/sh

status=0

for i in *.png; do
    for j in RGB RGBA GRAY GA; do
	for k in 8 16; do
	    out=$i.$j.$k.out
	    echo ./convert_color_space $j $k $i $out
	    ./convert_color_space $j $k $i $out && cmp $out cmp/$out \
	    || status=1
	done;
    done;
done

echo ./generate_gray_packed
./generate_gray_packed || status=1
for i in 1 2 4; do
    out=gray_packed_$i.png.out
    cmp $out cmp/$out || status=1
done

for i in 1 2 4; do
    in=basn0g0$i.png
    out=$in.out
    echo ./read_write_gray_packed $i $in $out
    ./read_write_gray_packed $i $in $out && cmp $out cmp/$out || status=1
done

echo ./generate_palette
./generate_palette || status=1
for i in 1 2 4 8; do
    out=palette$i.png.out
    cmp $out cmp/$out || status=1
done
cmp palette8_tRNS.png.out cmp/palette8_tRNS.png.out || status=1

echo ./write_gray_16
./write_gray_16 && cmp gray_16.out cmp/gray_16.out || status=1

test $status -eq 0 && echo 'PNG++ PASSES TESTS' || echo 'PNG++ FAILS TESTS'
exit $status
