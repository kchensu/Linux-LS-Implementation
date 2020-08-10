all:
	gcc myls.c  check_functions.c  print_info.c sort_function.c -o myls
clean:
	rm -f myls