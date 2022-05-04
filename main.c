/*
 *This is a program created for the course 004 - Structured Programming
 *of the Aristotle University.
 *Gerasimou Dimitrios
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <time.h>

//Each array with its properties is registred on memory with this struct.
struct array{
	char name[64];
	int type;
	int rows;
	int columns;
};

/*
 *The array names and properties are stored globally in an array,
 *along with this array's length and OS type for Windows compatibility.
*/
struct array *list = NULL;
int list_size = 0;
int os_type;

void clear_terminal(){
	if(os_type == 1)
		system("clear");
	else
		printf("\e[1;1H\e[2J\n");
}

//Function that returns the dotfile text filename of a string.
void to_file(char *name, char *filename){
	
	strcpy(filename, ".");
	strcat(filename, name);
	strcat(filename, ".txt");
}

//This is a simple function that halts and waits user input.
void enter_to_continue(){
	char temp;
	
	printf("Press ENTER to continue... ");
	do{
	temp = getchar();
	}while((temp != '\n') && (temp != EOF));
	
	clear_terminal();
}

//This function simply prints the main menu of the application.
void print_instructions(){
	printf("Select the type of operation you want to execute:\n\n");
	printf("	[1] Create array\n");
	printf("	[2] View available arrays\n");
	printf("	[3] Delete arrays\n");
	printf("	[4] Matrix operations\n\n");
	printf("	[0] to terminate the program.\n\n");
	printf("Enter your selection: ");
} 

//This function loads the arrays' names and properties for the 
//header dotfile, or if non existant, creates it.
void load_arrays(){
	FILE *fheader = fopen(".header.txt", "r");

	if(fheader == NULL){
		fheader = fopen(".header.txt", "w");
		list_size = 0;
		fprintf(fheader, "0\n");
	}else{
		fscanf(fheader, "%d", &list_size);
		
		if(list != NULL){
			free(list);
		}

		list = (struct array*) malloc(list_size * sizeof(struct array));

		for(int i=0; i<list_size; i++)
			fscanf(fheader, "%s %d %d %d", list[i].name, &list[i].type, &list[i].rows, &list[i].columns);
		
	}

	fclose(fheader);
}

//Function that writes an array to a text file and updates the header file.
void array_to_file(double **array, char *name, bool is_int, int rows, int columns){
	char filename[128];
	int tempi;
	
	to_file(name, filename);

	FILE *foutput = fopen(filename, "w");

	if(is_int == true)
		tempi = 1;
	else
		tempi = 2;

	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			if(array[i][j] == 0.0)
				array[i][j] = +0.0;

			if(is_int == true){				
				fprintf(foutput, "%.0lf ", array[i][j]);
			}else{
				fprintf(foutput, "%lf ", array[i][j]);
			}
		}
		fprintf(foutput, "\n");
	}
	
	remove(".header.txt");
	FILE *fheader = fopen(".header.txt", "w");

	fprintf(fheader, "%d\n", list_size + 1);
	for(int i=0; i<list_size; i++)
		fprintf(fheader, "%s %d %d %d\n", list[i].name, list[i].type, list[i].rows, list[i].columns);
	
	fprintf(fheader, "%s %d %d %d", name, tempi, rows, columns);

	fclose(foutput);
	fclose(fheader);

	load_arrays();
}

//Function that returns the max amount of int digits in a given array's data.
int max_digits(double **array, int rows, int columns){
	int max = 1;
	int count;
	int temp;

	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			count = 1;
			temp = (int)(array[i][j]);
			while(temp/10 != 0){
				temp = temp/10;
				count++;
			}
			if(count > max)
				max = count;
		}
	}

	return max;
}

//Function that controls the correct input of a filename and checks
//wherever it is valid.
void scan_name(char *name){
	char temp[256];
	bool check;
	void *in;
	int i;

RESTART:
	in = fgets(temp, sizeof(temp), stdin); 

	if(in == NULL){
		fprintf(stderr, "Input error. Program terminating.\n");
		free(list);
		exit(-1);
	}
	
	i=0;	
	check = false;

	while(temp[i] != '\0'){
		if(i == 255){
			fprintf(stderr, "Input error. Program terminating.\n");
			free(list);
			exit(-1);
		}

		if(temp[i] == 0 || temp[i] == '/')
			check = true;
		
		if(os_type == 2){
			if(temp[i] == 34 || temp[i] == 42 || temp[i] == 58 || temp[i] == 60)
				check = true;

			if(temp[i] == 62 || temp[i] == 63 || temp[i] == 92 || temp[i] == 124 || temp[i] == 127)
				check = true;
		}

		if(temp[i] == ' '){
			printf("Given name has whitespace. Enter one without: ");
			goto RESTART;
		}

		if(check == true){
			printf("Illegal characters for filename used.\nEnter again: ");
			goto RESTART;
		}

		i++;
	}

	if(i > 63){
		printf("The name '%s' is too large for a filename. Enter again: ", temp);
		goto RESTART;
	}

	sscanf(temp, "%s", name);
}

//Function that scans and validades the input of int numbers.
int scan_int(){
	char temp[32];
	void *check;
	int out;
	int i;

RESTART:
	check = fgets(temp, sizeof(temp), stdin);

	i=0;

	while(temp[i] != '\0'){
		if(i == 31 || check == 0){
			fprintf(stderr, "Input error. Program terminating.\n");
			free(list);
			exit(-1);
		}
		if(temp[i] < '0' || temp[i] >'9'){
			if(temp[i] != '\n'){
				printf("You have not entered a digit. Enter again: ");
				goto RESTART;
			}
		}
		i++;
	}
	
	sscanf(temp, "%d", &out);
	return out;
}

//Function that reads and returns the data type of an array.
bool scan_type(){
	char temp[64];
	void *check;
	char out[64];

	printf("Enter the array's data type [i]nteger/[d]ouble: ");

RESTART:
	check = fgets(temp, sizeof(temp), stdin);

	if(check == NULL){
		fprintf(stderr, "Input error. Program terminating.\n");
		free(list);
		exit(-1);
	}
	sscanf(temp, "%s", out);

	if(out[1] != '\0'){
		printf("Input not a single character. Enter again: ");
		goto RESTART;
	}

	if((out[0] != 'i') && (out[0] != 'd')){
		printf("Character '%c' is not valid. Enter again: ", out[0]);
		goto RESTART;
	}

	if(out[0] == 'i')
		return true;
	else
		return false;
}

//Function that reads and returns a double number and whetever it is an integer or not.
double scan_double(bool *is_int){
	char temp[32];
	void *check;
	double out;
	int i;
	int countdot = 0;
	int countdash = 0;

RESTARTD:
	check = fgets(temp, sizeof(temp), stdin);

	i=0;

	while(temp[i] != '\0'){
		if(i == 31 || check == 0){
			fprintf(stderr, "Input error. Program terminating.\n");
			free(list);
			exit(-1);
		}
		if((temp[i] < '0' || temp[i] >'9') && !(temp[i] == '.' || temp[i] == '-' || temp[i] == '\n')){
			printf("You have not entered a digit. Enter again: ");
			goto RESTARTD;
		}
		if(temp[i] == '.')
			countdot ++;
		
		if(temp[i] == '-')
			countdash ++;
		
		i++;
	}
	if(countdash > 0 && temp[0] != '-'){
		printf("Not a valid format. Enter again: ");
		goto RESTARTD;
	}

	if(countdot > 1 || countdash > 1){
		printf("Not a valid format. Enter again: ");
		goto RESTARTD;
	}else if(countdot == 1)
		*is_int = false;
	
	else if(countdot == 0)
		*is_int = true;

	else{
		fprintf(stderr, "Input error. Program terminating.\n");
		free(list);
		exit(-1);
	}
	
	sscanf(temp, "%lf", &out);
	return out;
}


//Function that asks the user a yes or no question.
bool yes_no(){
	char temp[64];
	void *check;
	char out[64];

	printf("[y]es/[n]o: ");

RESTART:
	check = fgets(temp, sizeof(temp), stdin);

	if(check == NULL){
		fprintf(stderr, "Input error. Program terminating.\n");
		free(list);
		exit(-1);
	}
	sscanf(temp, "%s", out);

	if(out[1] != '\0'){
		printf("Input not a single character. Enter again: ");
		goto RESTART;
	}

	if((out[0] != 'y') && (out[0] != 'n')){
		printf("Character '%c' is not valid. Enter again: ", out[0]);
		goto RESTART;
	}

	if(out[0] == 'y')
		return true;
	else
		return false;
}

//Function that asks the user for an array name and checks if its available.
void input_array_name(char *name){
	bool found = false;

	printf("Enter the output array's name: ");
	scan_name(name);
	
	for(int i=0; i<list_size; i++){
		if(strcmp(name, list[i].name) == 0)
			found = true;
	}

	while(found == true){
		printf("Name already in use. Pick a different one: ");
		scan_name(name);
		found = false;
		for(int i=0; i<list_size; i++){
			if(strcmp(name, list[i].name) == 0)
				found = true;
		}
	}
}

//Function that prints the list of all arrays and their properties in memory.
void print_list(){
	char type[16];
	
	clear_terminal();
	
	if(list_size == 0){
		printf("There are no available arrays to print.\n\n");
	}else{
		printf("Available arrays:\n\n");
		for(int i=0; i<list_size; i++){
			if(list[i].type == 1)
				strcpy(type, "int");
			
			else if(list[i].type == 2)
				strcpy(type, "double");
			
			else
				strcpy(type, "undefined");
			
			printf("\t[%d]\tType: %s\tDimensions: %dx%d\t\tArray name: %s\n", i+1, type, list[i].rows, list[i].columns, list[i].name);
		}
		printf("\n");
	}
}

/*
 * This is the function that creates the arrays, saving them on a .txt file
 * and adding its details on the header.
 * Takes an initial value where:
 * type = 1: Manually entering all the data in the array.
 * type = 2: The array data is entered by reading them through a text file.
 * type = 3: The array data is generated randomly in this proccess.
*/
void array_creation(int type){
	char filename[128];
	char name[64];
	int rows;
	int columns;
	bool is_int;

	clear_terminal();

	input_array_name(name);

	is_int = scan_type();
	
	printf("Enter the array's number of rows: ");
	rows = scan_int();
	while(rows <= 0){
		printf("Can't have 0 or negative number of rows. Enter again: ");
		rows = scan_int();
	}

	printf("Enter the array's number of columns: ");
	columns = scan_int();
	while(columns <= 0){
		printf("Can't have 0 or negative number of columns. Enter again: ");
		columns = scan_int();
	}

	double **matrix = (double**) malloc(rows * sizeof(double*));
	for(int i=0; i<rows; i++){
		matrix[i] = (double*) malloc(columns * sizeof(double));
	}
	if(type == 1){
		printf("Enter the '%s' array's data: ", name);
		
		for(int i=0; i<rows; i++){
			for(int j=0; j<columns; j++){
				scanf("%lf", &matrix[i][j]);
			}
		}
		getchar();
	}
	
	if(type == 2){
		printf("Enter the name of the file you want to import the array from: ");
		scan_name(filename);
		
		FILE *finput = fopen(filename, "r");
		while(finput == NULL){
			printf("File is empty, or filename is wrong. Enter again: ");
			scan_name(filename);
			finput = fopen(filename, "r");
		}
		
		for(int i=0; i<rows; i++){
			for(int j=0; j<columns; j++){
				fscanf(finput, "%lf", &matrix[i][j]);
			}
		}
		fclose(finput);
	}
	
	if(type == 3){
		time_t t;
		int lim;
		srand((unsigned)time(&t));
		double tempf;

		printf("Enter a number to set as limit to the generator: ");
		lim = scan_int();
		while(lim <= 0){
			printf("Enter a number of 1 and over: ");
			lim = scan_int();
		}

		for(int i=0; i<rows; i++){
			for(int j=0; j<columns; j++){
				if(is_int == true)
					tempf = (double)(rand()%lim);
				
				if(is_int == false)
					tempf = ((double)rand()/(double)(RAND_MAX)) * (double)(lim);
				
				if((rand() % 2) == 0)
					tempf = -tempf;

				matrix[i][j] = tempf;
			}
		}
	}
	
	array_to_file(matrix, name, is_int, rows, columns);

	for(int i=0; i<rows; i++)
		free(matrix[i]);

	free(matrix);

	printf("\nDone!\n\n");
	enter_to_continue();
}

//This function prints the array formated to standard output.
void print_array(int index){
	clear_terminal();

	char filename[64];
	to_file(list[index-1].name, filename);

	FILE *finput = fopen(filename, "r");

	if(finput == NULL){
		fprintf(stderr, "File '%s' does not exist, or is corrupted.\n", filename);
		fprintf(stderr, "It is recomended to clear the header file and manually reimport all the arrays.\n");
		fprintf(stderr, "Program terminating.");

		free(list);

		exit(-1);
	}

	int rows = list[index-1].rows;
	int columns = list[index-1].columns;

	double **matrix = (double**) malloc(rows * sizeof(double*));
	for(int i=0; i<rows; i++){
		matrix[i] = (double*) malloc(columns * sizeof(double));
		for(int j=0; j<columns; j++){
			fscanf(finput, "%lf", &matrix[i][j]);
		}
	}

	printf("Array name: %s\t\tDimensions: %dx%d\t\tData type: ", list[index-1].name, rows, columns);

	int maxdigits = max_digits(matrix, rows, columns);
	if(list[index-1].type == 1){
		printf("Integer\n\n");
		maxdigits = maxdigits + 3;
	}
	else{
		printf("Double\n\n");
		maxdigits = maxdigits + 9;
	}

	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			if(list[index-1].type == 1)
				printf("%*.0lf", maxdigits, matrix[i][j]);

			else
				printf("%*lf", maxdigits, matrix[i][j]);

		}
		printf("\n");
	}

	for(int i=0; i<rows; i++){
		free(matrix[i]);
	}
	free(matrix);

	fclose(finput);

	printf("\n\n");

	enter_to_continue();
}

//Function that prints formated an array to a txt file with a header.
void export_array(int index){
	char filename[64];
	to_file(list[index-1].name, filename);

	FILE *finput = fopen(filename, "r");

	strcpy(filename, "");
	strcat(filename, list[index-1].name);

	FILE *foutput = fopen(filename, "w");

	if(finput == NULL){
		fprintf(stderr, "File '%s' does not exist, or is corrupted.\n", filename);
		fprintf(stderr, "It is recomended to clear the header file and manually reimport all the arrays.\n");
		fprintf(stderr, "Program terminating.");

		free(list);

		exit(-1);
	}

	int rows = list[index-1].rows;
	int columns = list[index-1].columns;

	double **matrix = (double**) malloc(rows * sizeof(double*));
	for(int i=0; i<rows; i++){
		matrix[i] = (double*) malloc(columns * sizeof(double));
		for(int j=0; j<columns; j++){
			fscanf(finput, "%lf", &matrix[i][j]);
		}
	}

	fprintf(foutput, "Array name: %s\t\tDimensions: %dx%d\t\tData type: ", list[index-1].name, rows, columns);

	int maxdigits = max_digits(matrix, rows, columns);
	if(list[index-1].type == 1){
		fprintf(foutput, "Integer\n\n");
		maxdigits = maxdigits + 3;
	}
	else{
		fprintf(foutput, "Double\n\n");
		maxdigits = maxdigits + 9;
	}


	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			if(list[index-1].type == 1)
				fprintf(foutput, "%*.0lf", maxdigits, matrix[i][j]);

			else
				fprintf(foutput, "%*lf", maxdigits, matrix[i][j]);

		}
		fprintf(foutput, "\n");
	}

	for(int i=0; i<rows; i++){
		free(matrix[i]);
	}
	free(matrix);

	fclose(finput);
	fclose(foutput);

	printf("\n\nDone!\n\n");

	enter_to_continue();
}

//function that adds or subtracts matrices and outputs to a different file.
void matrix_addition(int index1, int index2, bool is_difference){

	bool is_compatible = true;

	if(list[index1-1].rows != list[index2-1].rows)
		is_compatible = false;

	if(list[index1-1].columns != list[index2-1].columns)
		is_compatible = false;

	if(is_compatible == false){
		char temp[16] = "addition";
		if(is_difference == true)
			strcpy(temp, "subtraction");

		printf("\nMatrices '%d' and '%d' are not compatible for %s.\n\n", index1, index2, temp);
		enter_to_continue();
		return;
	}

	int maxi = list[index1-1].rows;
	int maxj = list[index1-1].columns;
	bool is_int = true;

	if((list[index1-1].type == 2) || (list[index2-1].type == 2))
		is_int = false;

	double matrix1;
	double matrix2;
	char filename[128];

	to_file(list[index1-1].name, filename);
	FILE *finput1 = fopen(filename, "r");

	to_file(list[index2-1].name, filename);
	FILE *finput2 = fopen(filename, "r");

	char name[64];
	input_array_name(name);

	double **matrix3 = (double**) malloc(maxi * sizeof(double*));
	for(int i=0; i<maxi; i++)
		matrix3[i] = (double*) malloc(maxj * sizeof(double));

	for(int i=0; i<maxi; i++){
		for(int j=0; j<maxj; j++){
			fscanf(finput1, "%lf", &matrix1);
			fscanf(finput2, "%lf", &matrix2);
			if(is_difference == false)
				matrix3[i][j] = matrix1 + matrix2;
			else
				matrix3[i][j] = matrix1 - matrix2;
		}
	}

	array_to_file(matrix3, name, is_int, maxi, maxj);

	fclose(finput1);
	fclose(finput2);

	for(int i=0; i<maxi; i++)
		free(matrix3[i]);

	free(matrix3);

	printf("\nDone!\n\n");
	enter_to_continue();
}

//Function that multiplies 2 matrices and outputs into a new file.
void matrix_multiplication(int index1, int index2){

	if(list[index1-1].columns != list[index2-1].rows){
		printf("\nThese matrices cannot be multiplied together.\n\n");
		enter_to_continue();
		return;
	}

	int maxi = list[index1-1].rows;
	int maxj = list[index2-1].columns;
	int maxk = list[index1-1].columns;

	bool is_int = true;
	char filename[128];

	if(list[index1-1].type == 2 || list[index2-1].type == 2)
		is_int = false;

	to_file(list[index1-1].name, filename);
	FILE *finput1 = fopen(filename, "r");

	to_file(list[index2-1].name, filename);
	FILE *finput2 = fopen(filename, "r");

	char name[64];
	input_array_name(name);

	double **matrix1 = (double**) malloc(maxi * sizeof(double*));
	double **matrix2 = (double**) malloc(maxk * sizeof(double*));
	double **matrix3 = (double**) malloc(maxi * sizeof(double*));

	for(int i=0; i<maxi; i++){
		matrix1[i] = (double*) malloc(maxk * sizeof(double));
		matrix3[i] = (double*) malloc(maxj * sizeof(double));
		for(int k=0; k<maxk; k++){
			fscanf(finput1, "%lf", &matrix1[i][k]);
		}
	}

	for(int k=0; k<maxk; k++){
		matrix2[k] = (double*) malloc(maxj * sizeof(double));
		for(int j=0; j<maxj; j++){
			fscanf(finput2, "%lf", &matrix2[k][j]);
		}
	}

	for(int i=0; i<maxi; i++){
		for(int j=0; j<maxj; j++){
			matrix3[i][j] = 0;
			for(int k=0; k<maxj; k++){
				matrix3[i][j] += (matrix1[i][k] * matrix2[k][j]);
			}
		}
	}

	array_to_file(matrix3, name, is_int, maxi, maxj);

	fclose(finput1);
	fclose(finput2);

	for(int i=0; i<maxi; i++){
		free(matrix1[i]);
		free(matrix3[i]);
	}

	for(int k=0; k<maxk; k++){
		free(matrix2[k]);
	}

	free(matrix1);
	free(matrix2);
	free(matrix3);

	printf("\nDone!\n\n");
	enter_to_continue();
}

//Function that multiplies an array with a constand number.
void scalar_multiplication(int index){
	char name[64];
	char filename[128];
	int rows;
	int columns;
	double scale;
	bool is_int;
	
	clear_terminal();
	printf("Enter the number you want to multiply the matrix with: ");
	scale = scan_double(&is_int);
	
	input_array_name(name);
	
	rows = list[index-1].rows;
	columns = list[index-1].columns;

	double **matrix = (double **) malloc(rows * sizeof(double *));
	
	for(int i=0; i<rows; i++){
		matrix[i] = (double *) malloc(columns * sizeof(double));
	}
	
	to_file(list[index-1].name, filename);

	FILE *finput = fopen(filename, "r");

	for(int i=0; i<rows; i++){
		for(int j=0; j<columns; j++){
			fscanf(finput, "%lf", &matrix[i][j]);
			matrix[i][j] = matrix[i][j] * scale;
		}
	}

	if(list[index-1].type == 2)
		is_int = false;
	
	array_to_file(matrix, name, is_int, rows, columns);

	fclose(finput);

	for(int i=0; i<rows; i++)
		free(matrix[i]);

	free(matrix);

	printf("\nDone!\n\n");
	enter_to_continue();
}

//Function that transposes a matrix.
void matrix_transposition(int index){
	char name[64];
	char filename[128];
	int rows_1;
	int rows_2;
	int columns_1;
	int columns_2;
	bool is_int;
	
	clear_terminal();
	input_array_name(name);
	
	rows_1 = list[index-1].rows;
	columns_1 = list[index-1].columns;
	rows_2 = columns_1;
	columns_2 = rows_1;
	if(list[index-1].type == 1)
		is_int = true;
	else
		is_int = false;

	double **matrix1 = (double **) malloc(rows_1 * sizeof(double *));
	double **matrix2 = (double **) malloc(rows_2 * sizeof(double *));

	for(int i=0; i<rows_1; i++){
		matrix1[i] = (double *) malloc(columns_1 * sizeof(double));
	}
	
	for(int i=0; i<rows_2; i++){
		matrix2[i] = (double *) malloc(columns_2 * sizeof(double));
	}

	to_file(list[index-1].name, filename);

	FILE *finput = fopen(filename, "r");

	for(int i=0; i<rows_1; i++){
		for(int j=0; j<columns_1; j++){
			fscanf(finput, "%lf", &matrix1[i][j]);
		}
	}
	for(int i=0; i<rows_1; i++){
		for(int j=0; j<columns_1; j++){
			matrix2[j][i] = matrix1[i][j]; 
		}
	}

	array_to_file(matrix2, name, is_int, rows_2, columns_2);

	fclose(finput);

	for(int i=0; i<rows_1; i++)
		free(matrix1[i]);
	for(int i=0; i<rows_2; i++)
		free(matrix2[i]);
	free(matrix1);
	free(matrix2);

	printf("\nDone!\n\n");
	enter_to_continue();
}

//This function is the submenu to the array creation option.
void array_creation_submenu(){
	int choice = -1;
	bool print = true;

	while(choice != 0){
		if(print == true){
			clear_terminal();
			printf("Select the type of operation you want to execute:\n\n");
			printf("	[1] Create new array manually\n");
			printf("	[2] Load new array from file\n");
			printf("	[3] Create new random array\n\n");
			printf("	[0] To exit this menu\n\n");
			printf("Enter your selection: ");
		}

		choice = scan_int();
		switch (choice){
			case 0: break;

			case 1:	print = true;
				array_creation(choice);
				break;

			case 2:	print = true;
				array_creation(choice);
				break;

			case 3:	
				print = true;
				array_creation(choice);
				break;

			default:
				printf("Number '%d' is not a valid choice. Enter again: ", choice);
				print = false;
		}
	}
}

//Function that controls the view available arrays submenu.
void view_arrays_submenu(){
	int choice = -1;
	bool print = true;

	while(choice != 0){
		if(print == true){
			clear_terminal();
			printf("Select the type of operation you want to execute:\n\n");
			printf("	[1] Print a list of all available arrays\n");
			printf("	[2] Print an array\n");
			printf("	[3] Export formated array to a .txt file\n\n");
			printf("	[0] To exit this menu\n\n");
			printf("Enter your selection: ");
		}
		choice = scan_int();
		switch (choice){
			case 0:	break;

			case 1:	print = true;
				print_list();

				enter_to_continue();
				break;

			case 2:	print = true;
				print_list();
				
				if(list_size == 0){	
					enter_to_continue();
					goto NOPRINT;
				}
				
				printf("Enter the index of the array you want to print: ");
				choice = scan_int();
				
				while((choice > list_size) || (choice < 1)){
					printf("Number '%d' is invalid. Enter again: ", choice);
					choice = scan_int();
				}
				
				print_array(choice);
				
NOPRINT:
				break;

			case 3:	print = true;
				print_list();

				if(list_size == 0){
					enter_to_continue();
					goto NOPRINT2;
				}

				printf("Enter the index of the array you want to export: ");
				choice = scan_int();
				while((choice > list_size) || (choice < 1)){
					printf("Number '%d' is invalid. Enter again: ", choice);
					choice = scan_int();
				}

				export_array(choice);

NOPRINT2:
				break;

			default:
				printf("Index '%d' is not a valid option. Enter again: ", choice);
				print = false;
		}
	}
}

/*
 *Function that controls the delete array submenu. It deletes the header
 *from the dotfile of the matrix and the file with the data itself.
*/
void delete_array_submenu(){
	int choice = -1;
	char filename[128];
	bool print = true;
	bool y_n;

	while(choice != 0){
		
		if(print == true){
			clear_terminal();
			printf("Select the type of operation you want to execute:\n\n");
			printf("	[1] Delete a single array\n");
			printf("	[2] Delete all arrays\n\n");
			printf("	[0] To exit this menu\n\n");
			printf("Enter your selection: ");
		}

		choice = scan_int();
		switch (choice){
			case 0: break;

			case 1:	print = true;
				choice = -1;
				
				while(choice != 0){
					print_list();
					
					printf("Select the array index you want to delete ([0] to exit): ");
					choice = scan_int();
					
					if(choice == 0)
						goto OUT;
					else if((choice > list_size) ||(choice < 0)){
						printf("Number '%d' is an invalid option.\n\n", choice);
						enter_to_continue();
					}else{					
						printf("Are you sure you want to delete the '%s' array? ", list[choice-1].name);
						y_n = yes_no();
						if(y_n == false){
							goto OUT;
						}
						
						to_file(list[choice-1].name, filename);
						remove(filename);
						
						for(int i=choice-1; i<list_size; i++)
							list[i] = list[i+1];
						
						list_size --;
						remove(".header.txt");
						
						if(list_size != 0){
							
							FILE *header = fopen(".header.txt", "w");
							fprintf(header, "%d\n", list_size);

							for(int i=0; i<list_size; i++)
								fprintf(header, "%s %d %d %d\n", list[i].name, list[i].type, list[i].rows, list[i].columns);
							
							fclose(header);
							
							load_arrays();
						}else{
							choice = 0;
							printf("\nNo more files to delete.\n\n");
							enter_to_continue();
							goto OUT;
						}
					}
				}
OUT:
				break;

			case 2:	printf("Are you sure you want to delete all files?");
				y_n = yes_no();
				if(y_n == false){
					goto OUT2;
				}

				for(int i=0; i<list_size; i++){
					to_file(list[i].name, filename);
					remove(filename);
				}
				
				remove(".header.txt");
				
				load_arrays();
				
				printf("\nDone!\n\n");
				enter_to_continue();
				choice = 0;
OUT2:
				break;

			default:
				printf("Index '%d' is not a valid option. Enter again: ", choice);
				print = false;
		}
	}
}

//Function that controls the matrix operations submenu.
void matrix_operations_submenu(){
	int choice = -1;
	int array_index_1;
	int array_index_2;
	bool print = true;

	if(list_size < 1){
		clear_terminal();
		printf("Not enough available matrices for operations\n");
		enter_to_continue();
		return;
	}
	while(choice != 0){

		if(print == true){
			clear_terminal();
			printf("Select the type of operation you want to execute:\n\n");
			printf("	[1] Matrix addition\n");
			printf("	[2] Matrix difference\n");
			printf("	[3] Matrix multiplication\n");
			printf("	[4] Scalar multiplication\n");
			printf("	[5] Matrix transposition\n\n");
			printf("	[0] To exit this menu\n\n");
			printf("Enter your selection: ");
		}
		choice = scan_int();
		switch (choice){
			case 0:	break;

			case 1: print = true;
				
				print_list();
				
				printf("Enter the index of the two matrices you want to add ([0] to exit):\n\n");
				printf("Index 1: ");
				array_index_1 = scan_int();
				if(array_index_1 == 0)
					goto ADDEND;

				printf("Index 2: ");
				array_index_2 = scan_int();
				if(array_index_2 == 0)
					goto ADDEND;

				while((array_index_1 < 1 || array_index_2 < 1) || (array_index_1 > list_size || array_index_2 > list_size)){
					printf("The indexes are out of bounds. Enter again:\n\n");
					printf("Index 1: ");
					array_index_1 = scan_int();
					if(array_index_1 == 0)
						goto ADDEND;

					printf("Index 2: ");
					array_index_2 = scan_int();
					if(array_index_2 == 0)
						goto ADDEND;
				}
				
				matrix_addition(array_index_1, array_index_2, false);
				
ADDEND:
				break;

			case 2: print = true;
				
				print_list();
				
				printf("Enter the index of the two matrices you want to subtract ([0] to exit):\n\n");
				printf("Index 1: ");
				array_index_1 = scan_int();
				if(array_index_1 == 0)
					goto SUBEND;

				printf("Index 2: ");
				array_index_2 = scan_int();
				if(array_index_2 == 0)
					goto SUBEND;

				while((array_index_1 < 1 || array_index_2 < 1) || (array_index_1 > list_size || array_index_2 > list_size)){
					printf("The indexes are out of bounds. Enter again:\n\n");
					printf("Index 1: ");
					array_index_1 = scan_int();
					if(array_index_1 == 0)
						goto SUBEND;

					printf("Index 2: ");
					array_index_2 = scan_int();
					if(array_index_2 == 0)
						goto SUBEND;
				}
				
				matrix_addition(array_index_1, array_index_2, true);
				
SUBEND:
				break;

			case 3:	print = true;
				
				print_list();
				
				printf("Enter the index of the two matrices you want to multiply([0] to exit):\n\n");
				printf("Index 1: ");
				array_index_1 = scan_int();
				if(array_index_1 == 0)
					goto MULTEND;
				
				printf("Index 2: ");
				array_index_2 = scan_int();
				if(array_index_1 == 0)
					goto MULTEND;

				while((array_index_1 < 1 || array_index_2 < 1) || (array_index_1 > list_size || array_index_2 > list_size)){
					printf("The indexes are out of bounds. Enter again:\n\n");
					printf("Index 1: ");
					array_index_1 = scan_int();
					if(array_index_1 == 0)
						goto MULTEND;

					printf("Index 2: ");
					array_index_2 = scan_int();
					if(array_index_2 == 0)
						goto MULTEND;

				}
				
				matrix_multiplication(array_index_1, array_index_2);
				
MULTEND:
				break;

			case 4: print = true;

				print_list();

				printf("Enter the index of the matrix you want to multiply([0] to exit): ");
				array_index_1 = scan_int();
				if(array_index_1 ==0)
					goto SCEND;

				while(array_index_1 < 1 || array_index_1 > list_size){
					printf("The index is out of bounds. Enter again: ");
					array_index_1 = scan_int();
					if(array_index_1 == 0)
						goto SCEND;
				}

				scalar_multiplication(array_index_1);

SCEND:
				break;

			case 5: print = true;

				print_list();

				printf("Enter the index of the matrix you want to transpose([0] to exit): ");
				array_index_1 = scan_int();
				if(array_index_1 ==0)
					goto TREND;

				while(array_index_1 < 1 || array_index_1 > list_size){
					printf("The index is out of bounds. Enter again: ");
					array_index_1 = scan_int();
					if(array_index_1 == 0)
						goto TREND;
				}

				matrix_transposition(array_index_1);

TREND:
				break;

			default:
				printf("Index '%d' is not a valid option. Enter again: ", choice);
				print = false;
		}
	}
}	



int main(){
	#ifdef __linux__
		os_type = 1;
	#else
		os_type = 2;
	#endif
	
	int choice = -1;
	bool print = true;
	load_arrays();	

	while(choice != 0){
		
		if(print == true){
			clear_terminal();
			print_instructions();
		}

		choice = scan_int();
		switch (choice){
			case 0:	break;

			case 1:	print = true;
				array_creation_submenu();
				break;

			case 2:	print = true;
				view_arrays_submenu();
				break;

			case 3:	print = true;
				delete_array_submenu();
				break;

			case 4:	print = true;
				matrix_operations_submenu();
				break;

			default:
				printf("Index '%d' is not a valid option. Enter again: ", choice);
				print = false;
		}
	}
	free(list);
	clear_terminal();
	return 0;
}
