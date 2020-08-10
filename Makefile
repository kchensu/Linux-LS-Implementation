all:
	gcc myls.c  check_functions.c  print_info.c -o myls
clean:
	rm -f myls