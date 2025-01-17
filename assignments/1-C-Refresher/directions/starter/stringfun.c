//Name :Poorv Lal
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int reverse(char *,int, int);
int word_print(char *,int, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    //check that the user's string length is not too big
    
    //go though the user_str
    int str_allowed = 0;
    int us_count=0;
    int index_b=0; //the index of the buffer that we are crrently working on; at the end this will be the character count that is returned
    while(*user_str != '\0' )
    {
    	//if you enounter a space and you are allowed to have a space add the space to the buffer
		if( (*user_str==' ' || *user_str =='\t') && str_allowed==1 )
		{
			buff[index_b] = ' ';
			str_allowed =0;
			index_b++;
		}
		//else if we have a none whitespace character add it into the buffer
		else if(*user_str !=' ' && *user_str !='\t' )
		{	
			buff[index_b]=*user_str;
			str_allowed =1;
			index_b++;
		}
		//else it will be a non-allowed white-space and we are ignoreing it
		us_count++;
		user_str++;
    }
	//if the user string lenth is greater than the buffer limit then return -1
	if(us_count>len)
	{
		exit(-1);
	}

    //if the last item added to the beffer was a whitespace, replace with a .
    if(buff[index_b -1] ==' ')
    {
		buff[index_b -1] = '.';
    }

    //fill in the rest of the buffer with '.'
    for (int i=index_b ; i<=len; i++)
    {
		buff[i] ='.';
    }
	
	return index_b;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    //YOU MUST IMPLEMENT
    //if the lenths are wrong
    if(str_len>len) {return -1;}

    // count up the number of spaces
    int count=0;
    for(int i=0; i<=str_len;i++)
    {
		if(buff[i] == ' ' )
		{
			count++;
		}
    }
    //add one for the last word (there is one extra word than the number of spaces)
    count++;
    return count;
}

int reverse(char *buff, int len, int str_len)
{
	//if the lenths are wrong
    if(str_len>len) {return -1;}
	
	//store the reverse of the buffer in temp and print it
	char* temp=malloc(sizeof(char) * str_len+1 );
	int j=0;
	for(int i=str_len- 1 ; i>=0;i--)
	{
		temp[j]=buff[i];
		j++;
	}
	temp[j]='\0';
	printf("%s\n", temp);
	free(temp);
	return 0;
}

int word_print(char *buff, int len, int str_len)
{
	//if the lenths are wrong
	if(str_len>len) {return -1;}

	//start print statment construction
	printf("Word Print\n");
	printf("----------\n");

	//start going though the buff
	int index_b=0;
	int char_count=0 ;

	for(int i=1; i<= count_words(buff,len,str_len) ; i++)
	{
		//print information for word
		printf("%d. ",i);
		while(buff[index_b] !=' ' && buff[index_b] !='.')
		{
			printf("%c",buff[index_b]);
			index_b++;
			char_count++;
		}
		printf(" (%d)\n",char_count );

		//reset chra_count
		index_b++;
		char_count =0;
	}

	return 0;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //     This is checking if the users input makes sence. we need a minimum of 2 arguments (the function call and the operation tag).
    //		if there are less than 2 arguments then the if statment will catch it at the first condition of the or statment.
    //		if argv[1] does not exist, you will have less than 2 arguments and since it checks that condition first it has already going into he if statment to tell the user they have a bad call and it doesnt attempt to locate argv[1]
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      none of the operations we are writing requires more than 3 arguments 
    //		at minimum we need 2 arguments for the -h command
    // 		at most we need 3 arguments (not including the -x command which takes 5, this will need to change in that case from 3 to 5 if the extra credit is attempted)
    //		if there are more than 3 arguments something has gone wrong and we need to send back an error becuase we have recived a bad call
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

    buff = malloc(BUFFER_SZ);
    if (buff == NULL)
    {
		exit(99);
	}

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
		case 'r':
			rc = reverse(buff, BUFFER_SZ, user_str_len);  //you need to implement
		 	if (rc< 0){
		 		printf("Error reversing string, rc = %d", rc);
		 		exit(2);
		 	}
		 	//printf("Reversed: %d\n", rc);
		 	//free(temp);
		 	break;
		
		case 'w':
			rc = word_print(buff, BUFFER_SZ, user_str_len);  //you need to implement
			if (rc < 0){
				printf("Error printing words, rc = %d", rc);
				exit(2);
			}
			//printf("Reversed: %s\n", rc);
			break;

        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
	//free(buff);
    print_buff(buff,BUFFER_SZ);
    free(buff);
    
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter takes both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all, we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          this makes sure that as you move from function to function, you arent running in to any memory issues. 
//			also, if later we want to the string to be more or less than 50 bytes then it is easy to change the one global variable at the top. 
