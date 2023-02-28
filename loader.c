/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "utils.h"
#include "exec_parser.h"

static so_exec_t *exec;
static struct sigaction oldAction;
int fd;
int **deja_mapate;

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	
	/* TODO - actual loader implementation */
	
	int gasit = 0;
	int nr_pagini = 0;
	int dim_pagina = 4096; 
	char *point;
	int nr_pag_cur;
	so_seg_t segment;
	int nr_octeti;
	int ID_seg;
	int last_filesize;
	/*
	adresa cauzatoare de pagefault
	*/
	void* adresa_pgf = (void *)info->si_addr;
	
	for (int i = 0; i < exec->segments_no; i++) {
		segment = exec->segments[i];
	
	/*
	* se verifica daca segmentul contine pagina cauzatoare de pagefault
	*/
 		if ((void *)(segment.vaddr + segment.mem_size) >= adresa_pgf && (void *)adresa_pgf >= (void *)(segment.vaddr)) {
 			gasit = 1;
			ID_seg = i;
			break;
 		}
	}

	segment = exec->segments[ID_seg]; 
	nr_pag_cur = (int) (adresa_pgf - segment.vaddr) / dim_pagina;

	/* 
	*	daca segmentul nu contine pagina cauzatoare
	* 	atunci se apeleaza handlerul default
	*/

	if (gasit == 0) {
		oldAction.sa_sigaction (signum, info, context);
	}

	 if (deja_mapate[ID_seg][nr_pag_cur] == 1) {
	 	oldAction.sa_sigaction (signum, info, context);
	 } 

	if (gasit == 1) {
		nr_pag_cur = (int) (adresa_pgf - segment.vaddr) / dim_pagina;
		deja_mapate[ID_seg][nr_pag_cur] = 1;
	}

		/*
		* incep prin a trata cazul in care 
		* mem_size = file_size
		*/
		if (segment.mem_size == segment.file_size) {
			nr_pagini = segment.file_size / dim_pagina;
			/* 
			*ID-ul ultimei pagini este egal cu nr de pagini
			*din segment 				
			* daca pagina curenta este ultima pagina
			* atunci mapez fix cat contine ultima pagina
			*/
			if ( nr_pag_cur == nr_pagini) {
				point = mmap((void *)segment.vaddr + nr_pag_cur * dim_pagina, segment.file_size % dim_pagina, segment.perm, MAP_PRIVATE | MAP_FIXED, fd, segment.offset + nr_pag_cur * dim_pagina);
				DIE(point == (char *) -1, "mmap");
			}
			/*
			daca pagina nu este ultima, mapez fix cat contine o pagina intreaga
	        * adica, 4096
			*/
			else {
				point = mmap((void *)segment.vaddr + nr_pag_cur * dim_pagina, dim_pagina, segment.perm, MAP_PRIVATE | MAP_FIXED, fd, segment.offset + nr_pag_cur * dim_pagina);
				DIE(point == (char *) -1, "mmap");
			}
		}	
		

		/*
		* 	se studiaza cazul in care mem_size nu este egal cu file_size.
		*	acesta are alte 3 cazuri:
		*/
		 else {
			nr_pag_cur =  (int) (adresa_pgf - segment.vaddr) / dim_pagina;
			last_filesize = segment.file_size / dim_pagina;
			/*
			1. mapare cu dimensiunea numarul_paginii_curente * 4096
			*/
		 	if (nr_pag_cur < last_filesize) {
		 		point = mmap((void *)segment.vaddr + nr_pag_cur * dim_pagina, dim_pagina, segment.perm, MAP_PRIVATE | MAP_FIXED, fd, segment.offset + nr_pag_cur * dim_pagina);
		 		DIE(point == (char *) -1, "mmap");
		 		
		 	}
			 /*
			 2. mapare cu dimensiunea paginii < 4096
			 */
		 	else if (last_filesize < nr_pag_cur) {
		 		point = mmap((void *)segment.vaddr + nr_pag_cur * dim_pagina, dim_pagina, segment.perm, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, 0, 0);
		 		DIE(point == (char *) -1, "mmap");
		 	}

		 	/*
			3. mapare dimensiunea paginii si restul zeroizare
			*/
			else if (nr_pag_cur == last_filesize) {
				point = mmap((void *)segment.vaddr + nr_pag_cur * dim_pagina, dim_pagina, segment.perm, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, 0, 0);
				DIE(point == (char *) -1, "mmap");
				
		 		lseek(fd, segment.offset + nr_pag_cur * dim_pagina, SEEK_SET);
				nr_octeti = read(fd, point, segment.file_size % dim_pagina);
		 		DIE(nr_octeti == -1, "read");
				 
			}

		}
		

}
	
 	
 

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;
	

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	
	
	/*
	* deschidere fisier
	*/
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror("open");
	}
	exec = so_parse_exec(path);
	if (!exec)
		return -1;
	/*
	alocare memorie pentru o matrice care retine daca paginile sunt mapate 
	*/
	deja_mapate = (int **) calloc(exec->segments_no, sizeof(int *));
	for (int i = 0; i < exec->segments_no; i++) {
		deja_mapate[i] = (int *) calloc(500, sizeof(int));
	}

	so_start_exec(exec, argv);

	/*
	inchidere fisier
	*/
	close (fd);

	return -1;

}

