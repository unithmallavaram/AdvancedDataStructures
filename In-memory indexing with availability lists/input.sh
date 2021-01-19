
./assn_2 --first-fit student.db < input-01.txt > my-output-01.txt
./assn_2 --first-fit student.db < input-02.txt > my-output-02.txt

rm index.db availlist.db student.db
./assn_2 --best-fit student.db < input-01.txt > my-output-03.txt
./assn_2 --best-fit student.db < input-02.txt > my-output-04.txt
                                                               
rm index.db availlist.db student.db
./assn_2 --worst-fit student.db < input-01.txt > my-output-05.txt
./assn_2 --worst-fit student.db < input-02.txt > my-output-06.txt

rm index.db availlist.db student.db     
