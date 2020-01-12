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
  for (char_index = 0ul; char_index < text->length; ++char_index)
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
  Load  text.
*/
Text getText(const char *path)
{
  Text text;
  FILE *in_file = NULL;
  size_t bytes_read = 0;

  /* reset. */
  text.length       = 0;
  text.data         = NULL;
  text.current_word = NULL;

  /* Open file. */
  if (!(in_file = fopen(path, "rb")))
  {
    fprintf(stderr, "Couldn't open %s!", path);
    exit(2);
  }

  /*allocate buffer,by text length. */
  fseek(in_file, 0, SEEK_END);
  text.length = ftell(in_file);
  if ( !(text.data = calloc(text.length + 1, sizeof(*text.data))) )
  {
    fclose(in_file);
    fprintf(stderr, "Couldn't allocate memory (%zd bytes)!",
            sizeof(*text.data) * text.length + 1);
    exit(3);
  }

  /* Read the text */
  fseek(in_file, 0, SEEK_SET);
  bytes_read = fread(text.data, 1, text.length, in_file);
  fclose(in_file);

  /* Check all the data has been readed. */
  if (bytes_read != text.length)
  {
    if (text.data)
      free(text.data);
    fprintf(stderr, "Failed to read expected number of chars!\n");
    exit(4);
  }

  /* reset the "current word" pointer to the start. */
  text.current_word = text.data;
  return text;
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
  Add the contents of a text file to the word count.  
*/
trienode * countTextFile(const char *path)
{
  const char *word = NULL;   /* The current word */
  Text text = getText(path);
  
  preprocess(&text);
  trienode *count_tree = calloc(1, sizeof(trienode));
  
  while ((word = getNextWord(&text)) != NULL){ addWord(word, count_tree);
} 
  freeText(&text);

  return count_tree;
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



int main(int argc,char const *argv[])
{
 

if(argv[1][0]=='r'){
trienode *counts_root = countTextFile(argv[2]);
  printCountsDown(stdout, counts_root);
 freeTree(counts_root);
}else{
 trienode *counts_root = countTextFile(argv[1]);
 printCounts(stdout, counts_root);
 freeTree(counts_root);
}
  return 0;
}


