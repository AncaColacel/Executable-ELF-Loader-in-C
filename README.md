# Executable-ELF-Loader-in-C

**ORGANIZARE**

-Exceptand functiile deja implementate din scheletul temei, rezolvarea propriu zisa a constat in implementarea functiei segv_handler. Aceasta functie este apelata la
primirea semnalului SIG_SEGV, ocupandu-se cu tratarea acestuia. 
-Am parcurs vectorul de segmente si am cautat daca in cadrul segmentului curent se afla pagina cauzatoare
de PAGE_FAULT.  
-Am marcat acest lucru cu o variabila gasit. 
-In cazul in care variabila gasit are valoarea 0 se apeleaza handlerul default.
-In cazul in care variabila gasit are valoarea 1 intervin mai multe cazuri:
1.  memsize == filesize =>
		1.1 daca pagina este ultima, mapez fix dimensiunea acesteia
		1.2 daca pagina nu este ultima, mapez cu dimesniunea standard de 4096
2. memsize > filesize =>
		2.1 se mapeaza numarul paginii * 4096
		2.2 se mapeaza cat contine pagina (mai putin de 4096)
		2.3 se mapeaza cat contine pagina si restul se zeroizeaza (cazul bss)
-O alta situatie in care se ruleaza handlerul default este in lipsa permisiunilor.
-Consider tema utila pentru ca m-a ajutat sa ma familiarizez cu multe notiuni importande,  de pilda: mecanismul demand-paging, maparea paginilor in memorie,
tratarea semnalelor cu ajutorul functiilor handler.

**COMPILARE && RULARE**
-Am folosit Makefile-ul din schelet
-Rularea testelor se face cu ./run_tests.sh noTest sau cu ./run_all.sh

--- BIBLIOGRAFIE ---
	-Vechile laboratoare 4, 5, 6
	- Notite de curs
	-https://man7.org/linux/man-pages/man2/mmap.2.html
	-https://man7.org/linux/man-pages/man2/sigaction.2.html
	-https://stackoverflow.com/questions/1564372/what-causes-a-sigsegv
