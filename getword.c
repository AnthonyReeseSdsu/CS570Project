/*
Program: getword.c
Author: Anthony Reese
RED ID: 817839276
Professor: John Carroll
Class: CS570 - Operating Systems
Due Date: 9/5/18

Synopsis - Takes input from stdin, splits words on certain delimeters
 and places them into an array to be printed to stdout. The program
 will overwrite each word with the next word and return the letter count
 to the main function. The main function will then use the array which
 was filled with the getword function, as well as the returned int value,
 to print to stdout

*/
#include <stdlib.h>
#include "getword.h"
int backSlashFlag;
int environFlag;
int userLookFlag;


int getword(char *w)
{
   int c;
   int z; 
   int tildeFlag =0;
   int count = 0;
   int negative = 1;
   char *user; 
   userLookFlag =0; 
   backSlashFlag =0; 
   environFlag = 0;  
  while((c= getchar()) != EOF && count < STORAGE-1)
   {
	/*
	This if statement will check for spaces in between words and if
	 one is  found, it is null terminated so that the word can print
	properly. Leading spaces are also skipped. 
*/  

     if(count > 0 && c== ' ')
       {  
           *w = '\0';
           return count;
       }
	/*
	This if statement checks for a leading "~", while ignoring any non
	leading "~". If found, getenv() is used in order to copy into our 
	buffer to be printed. Continue is used so that we can scan the next
	characters to see if they need to be added on to our environment
	that was found.
 */
	if(tildeFlag == 1)
	{
		if(count == 0){	
			if(c == ' ' || c == '&' || c == '\n'){	
				userLookFlag = 0;	
				user = getenv("HOME");	
				while(*user != NULL)
				{
					*w++ = *user++;
					count++;
				}	
				if(c == ';' || c == '\n')
					ungetc(c,stdin);		
				return count;	
			}	
		}
	}
	if(c == '~' && count == 0 && negative == 1){
		tildeFlag =1;
		userLookFlag =1;	
		continue;
	}

	/*
 	This statement will find any '\' on a line and place the next char 
	into our buffer to be printed, while ignoring the '\'. If a new line
	is detected to be the char after a '\', then it is treated like a space.
	*/ 

	if(c == '\\')
	 {

		if((z = getchar()) == EOF)
			break;
	    
		else if(z == '\n')
		{
			backSlashFlag =1;	
			ungetc(' ',stdin);
			*w++ = '\0';
			return count;		
		}
		else
		{
			backSlashFlag = 1;		
			*w++ = (char) z;
			 count++;
			 continue;
		 }
	}
	/*
	Here we check for a '<', if one is found then any word detected is null terminated.
	We then ungetc to detect the char again. If the next char is another '<', then both
	are placed into the array to be printed and 2 is return. If not,then the char is 
	returned to stdin, we then place the single '<' into the array to be printed and return 1.
 */

	if(c == '<')
	 {
		if(count > 0 || negative == -1)
		{
			*w++ = '\0';
			ungetc(c,stdin);
			return count;
		}
		else if((z = getchar()) == '<')
		{
			*w++ = (char) c;
			*w++ = (char) z;
			*w++ = '\0';
			return 2;
		} 
		else
		{
			*w++ =(char) c;
			*w++ = '\0';
			ungetc(z,stdin);
			return 1;
		}
	}
	/*
	Here we check for '>','|', and '&'. If one of these characters is found to be after
	a word, then the word is null terminated and our char is placed back into stdin in 
	order to be detected again. Once that is done(or it is not detected to be after a word),
	we place the single char into the array to be printed and return 1. 
 */

	if(c == '>' || c == '|' || c == '&')
	{
		if(count > 0 || negative == -1) {
			*w++ = '\0';
			ungetc(c,stdin);
			return count;	
		} 
		else{
			*w++ =(char) c;
			*w++ = '\0';
			return 1;	
		}	
	}
	/*
	This if statement will check for semicolons or newlines and null
	terminate the char if one is found.The second if is used to detect
	 if the char was directly after a word or single "$", then ungetc
	 is used in order to detect the semicolon/new line again.  

*/
  	  if(c == ';' || c == '\n')
       {
           *w = '\0';
           if(count > 0 || negative == -1)
           {
               ungetc(c,stdin);
           }
           return count;
       }
	
	/*
	This if statement is used in order to detect a leading "$" on a word, 
 	while ignoring any "$" signs in any other part of the word.
 */

       if( c== '$' && count == 0 && negative == 1)
       {
           negative = -1;
      	environFlag = 1; 
	}
	/*
	This statement is used to skip leading spaces on a line and then copy 
 	characters into the given array. If we reach this part of the loop,
	we know that the character must either be a leading zero, or a 
	valid character.*
 */      
	 else if(c != ' ')
       {          
           *w++ = (char) c ;
           count++;
       }
   }
	/*
 	This if statement is used to detect any words that weren't printed
	before our while loop ended(EOF found). This statement will also
 	catch special cases where "$" is by itself.
 */

	 if(count>0 || negative == -1)
  	 {
      		 *w++ = '\0';
	       ungetc(c,stdin);
     		  return count;
  	 }
   *w = '\0';
   return -255;
}
