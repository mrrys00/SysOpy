cp shelf.txt .checker_temp
echo -ne "\n\n\n\n\n" >> .checker_temp

echo -ne "\nExpected: "
head -n 1 storage1.txt
echo -ne "\nReceived: "
head -n 1 .checker_temp | tail -n 1

echo -ne "\nExpected: "
head -n 1 storage2.txt
echo -ne "\nReceived: "
head -n 2 .checker_temp | tail -n 1

echo -ne "\nExpected: "
head -n 1 storage3.txt
echo -ne "\nReceived: "
head -n 3 .checker_temp | tail -n 1

echo -ne "\nExpected: "
head -n 1 storage4.txt
echo -ne "\nReceived: "
head -n 4 .checker_temp | tail -n 1

echo -ne "\nExpected: "
head -n 1 storage5.txt
echo -ne "\nReceived: "
head -n 5 .checker_temp | tail -n 1
echo

rm -f ./checker_temp
