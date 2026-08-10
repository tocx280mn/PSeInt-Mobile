mkdir -p cur
for a in c cpp cs java js m pas php py2 py3 qb vb; do
	echo $a...
	bash test.sh $a > cur/$a.txt
done
meld ref cur
