#include <stdio.h>
#include <string.h>
#include <stdlib.h>  /* calloc */
#define N_CHARS 27


//////structors:
typedef struct Text
{
  unsigned long length;///len of text
  char *data;///pointer to start of text
  char *current_word;//pointer to current word
} Text;

typedef struct trienode trienode;
struct trienode
{
  trienode *children[N_CHARS];///childerns
  unsigned int count;///count preformens
};
/* max char. */
const int n_chars = N_CHARS;
/* The map of chars to indices. */
const char characters[N_CHARS] = "abcdefghijklmnopqrstuvwxyz\'";



/*
  recursive funtion printing unempty  words .
*/
void printNode(FILE *stream, const trienode *node, char c, int depth)
{
  /* contains string we build in deep search on tree */
  static char stack[64];

  /* put this char in stack. */
  stack[depth] = c;

  /* If there are changes  print the String and number of ocurrences */
  if (node->count)
    printf("%s\t%d\n", stack, node->count);

  /* run over the other characters and call recursive function we need so. */
  int i;
  for (i = 0; i < n_chars; ++i)
  {
    if (node->children[i] != NULL)
      printNode(stream, node->children[i], characters[i], depth+1);
  }
  stack[depth] = '\0';
}


/*
  recursive funtion printing unempty  words from the end .
*/
void printNodeDown(FILE *stream, const trienode *node, char c, int depth)
{
  /* contains string we build in deep search on tree */
  static char stack[64];

  /* put this char in stack. */
  stack[depth] = c;

  /* If there are changes  print the String and number of ocurrences */
  if (node->count)
    printf("%s\t%d\n", stack, node->count);

  /* run over the other characters and call recursive function we need so. */
  int i;
  for (i = n_chars-1; i >= 0; --i)
  {
    if (node->children[i] != NULL )
      printNodeDown(stream, node->children[i], characters[i], depth+1);
  }
  stack[depth] = '\0';
}


/*
  free the memory allocced to  input  .
*/
void freeText(Text *text)
{
  if (text->data)
  {
    free(text->data);
    text->data = NULL;
    text->current_word = NULL;
    text->length = 0;
  }
}

/*
    next word pointer .
*/
const char * getNextWord(Text *text)
{
  const char *word = NULL;

  /* when space characters between words skip */
  while ((unsigned long)(text->current_word - text->data) < text->length &&
         *text->current_word == '\0')
  {
    ++text->current_word;
  }

  word = text->current_word;  /* we are at a word */

  /* get  the end of the  word  because need to get ready for the next call from user. */
  while ((unsigned long)(text->current_word - text->data) < text->length &&
         *text->current_word != '\0')
  {
    ++text->current_word;
  }

  /* chack if we are not at the end of the text. */
  if (word < (text->data + text->length))
    return word;
  else
    return NULL;
}
 


/*
   char  to lowercase.

*/
char lower(char c)
{
  return c | 32;
}


/*
  chack if the character is between a to z and Ato Z
*/
_Bool isAlpha(char c)
{
  return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

/*
   ru on text and Lower text and replace illegal characters with \0s, creating an array of
  words which are null terminated by one or more null bytes.
*/
void preprocess(Text *text)
{
  unsigned long char_index;
  char previous = '\0';
  char current  = '\0';
  char next = text->data[0];  /* Next is the first charcter as we go into the loop */
  for (char_index = 0; char_index < text->length; ++char_index)
  {
    previous = current;
    current = next;
    next = text->data[char_index+1];
    if (isAlpha(current))
    {
      text->data[char_index] = lower(current);
    }
    else if (isAlpha(previous) && isAlpha(next) && current == '\'')
    {
      continue;
    }
    else
    {
      text->data[char_index] = '\0';
    }
  }
}



/*
    index of a character.
*/
int indexOf(char c)
{
  return c == '\'' ? 26 : c - 'a';
}

/*
  Get the count of a certain word in an trie rooted at root.
*/
unsigned int getCount(const char *word, const trienode *root)
{
  int character;
  const trienode *node = root;
  for (character = 0; word[character] != '\0'; ++character)
  {
    if (node->children[indexOf(word[character])] != NULL)
      return 0;  
    else
      node = node->children[indexOf(word[character])]; 
  }
  return node->count;
}


/*
  Add a word to the tree
*/
void addWord(const char *word, trienode *root)
{
  int character;
  trienode *node = root;
  
  /* if we have an actual word... */
  if (word != NULL)
  {
    /* for each character... */
    for (character = 0; word[character] != '\0'; ++character)
    {
      /* Move to existing node */
    	if (node->children[indexOf(word[character])] != NULL)
        node = node->children[indexOf(word[character])]; 
      /* Make new node to move to */
      else
        node = node->children[indexOf(word[character])] = calloc(1, sizeof(trienode));  
    }
    /* increment the counter for this word */
    ++(node->count);
  }
}






/*
  Print word counts to stream, defaulting to stdout.
*/
void printCounts(FILE *stream, trienode *root)
{
  int i;
  for (i = 0; i < n_chars; ++i)
  {
    if (root->children[i] != NULL)
    {
      printNode(stream, root->children[i], characters[i], 0);
    } 
  }
}


/*
  Print word counts to stream, defaulting to stdout.
*/
void printCountsDown(FILE *stream, trienode *root)
{
  int i;
  for (i = n_chars; i >= 0; i--)
  {
    if (root->children[i] != NULL)
    {
      printNodeDown(stream, root->children[i], characters[i], 0);
    } 
  }
}


/*
  De-allocate a tree or subtree.
*/
void freeTree(trienode *root)
{
  int i;
  for (i = 0; i < n_chars; ++i)
  {
    if (root->children[i] != NULL)
    {
      freeTree(root->children[i]);

    }
  }
  free(root);
}


char *inputString(FILE* fp, size_t size){
//The size is extended by the input with the value of the provisional
    char *str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(char)*size);//size is start size
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp))){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=16));
            if(!str)return str;
        }
    }
    str[len++]='\0';


    return realloc(str, sizeof(char)*len);
}

	int main(int argc,char const *argv[])
	{

		 char *m;

    
    m = inputString(stdin, 10);

const char *word = NULL;   /* The current word */
  Text text ;
 	text.current_word = m;
  	text.data = m;
  	text.length = strlen(m);
    preprocess(&text);										
  
  trienode *count_tree = calloc(1, sizeof(trienode));
	 while ((word = getNextWord(&text)) != NULL){ 			

		addWord(word, count_tree);
} 
  

		if(argc>=2){
	if(argv[1][0]=='r'){
	
	  printCountsDown(stdout, count_tree);
	 freeTree(count_tree);
	}
	}else{
	 printCounts(stdout, count_tree);
	 freeTree(count_tree);
	}
	freeText(&text);
	//free(m);

	  return 0;
	}


