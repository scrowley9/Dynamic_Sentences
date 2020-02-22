#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct sentence{
    int sentence_size;
    int num_words;
    char** sentence;
} sentence_t;

/* Hidden Functions */
static char* allocate_for_word(int length);
static char* store_word_on_heap(char* word, int length);
static sentence_t* create_sentence(void);
static sentence_t* create_sentence_struct(int number_words_in_sentence);
static void free_sentence_struct(sentence_t* sentence);
static int get_next_word_and_size(char* sentence, char** word_ptr, int first_word_check);

/* Visible Functions */
void print_sentence(sentence_t* sentence_data);
sentence_t* allocate_sentence_dynamically(char* sentence);


/**
 * @brief Allocate space for a string on the heap
 */
char* allocate_for_word(int length){
    char* out = calloc(1, sizeof(char)*length);
    if(!out){
        printf("ERROR on word allocation %d\n", __LINE__);
        exit(1);
    }
    return out;
}

/**
 * @brief Takes a word from off the stack and dynamically allocates it
 * 
 * NOTE: strncpy auto fills end of new_word space with '\0' if src_string < length
 */
char* store_word_on_heap(char* word, int length){
    if(!word){
        printf("ERROR store_word_on_heap -- word is NULL %d\n", __LINE__);
        exit(1);
    }
    if(length <= 0){
        return NULL;
    }

    char* new_word = allocate_for_word(length+1);         // Make space in memory for word -- Exits on error
    strncpy(new_word, (const char*) word, length);        // new_word = word
    return new_word;                                      // Return malloc'd word
}

/**
 * @brief Dynamically allocate for a sentence struct
 * 
 * @return sentence_t* 
 */
sentence_t* create_sentence(void){
    sentence_t* sentence = calloc(1, sizeof(sentence_t));
    if(!sentence){
        printf("Calloc failed -- create sentence %d\n", __LINE__);
        exit(1);
    }
    return sentence;
}

/**
 * @brief Dynamically allocate struct for sentence 
 *        and allocate the double pointer for the sentence
 * 
 * @param number_words_in_sentence 
 * @return sentence_t* 
 */
sentence_t* create_sentence_struct(int number_words_in_sentence){
    
    // Default number of words == 5
    if(number_words_in_sentence <= 0){
        number_words_in_sentence = 5;
    }

    // Create Sentence Metadata
    sentence_t* info_sentence = create_sentence();
    
    // Create Sentence
    info_sentence->sentence = (char**)calloc(number_words_in_sentence, sizeof(char*));
    if(!info_sentence->sentence){
        free(info_sentence);
        printf("Calloc failed -- get_word_count %d\n", __LINE__);
        exit(1);
    }

    // Assign init values
    info_sentence->num_words = 0;
    info_sentence->sentence_size = number_words_in_sentence;
    return info_sentence;
}

/**
 * @brief Free the entirety of a sentence struct
 * 
 */
void free_sentence_struct(sentence_t* sentence){

    // Free words in sentence
    for(int i = 0; i < sentence->num_words; i++){
        free(sentence->sentence[i]);
    }

    // Free sentence
    free(sentence->sentence);

    // Free sentence metadata
    free(sentence);
}


/**
 * @brief Called while looping over a sentence. Used to retrieve the beginning of the next word.
 * 
 *        The next word is defined as any character after a space character, that is not itself a space character.
 *        Example: 
 *          "Hi there!"     -- The 't' in 'there' follows a space character so it must be the next word.
 *          "Hi   there!"   -- The 't' in 'there' follows a space character so it must be the next word.
 * 
 * @param sentence          -- Sentence to look through (Note: this string should be on the stack)
 * @param word_ptr          -- Double ptr used to return a ptr to the beginning of the next word
 * @param first_word_check  -- Set this ONCE in the loop that you are using to parse the sentence (Caller: allocate_sentence_dynamically)
 * 
 * @return int              -- Size of the next word (used to allocate memory for the word elsewhere)
 *                             returns 0 on NULL terminator, return > 0 otherwise
 */
static int get_next_word_and_size(char* sentence, char** word_ptr, int first_word_check){
    if(!sentence){
        printf("Sentence is NULL -- get_next_word_and_size %d\n", __LINE__);
        exit(1);
    }

    // Move sentence ptr to END of current word -- if it isn't the first word
    if(!first_word_check){
        while(*sentence){
            if(*sentence != ' ') sentence++;
            else break;
        }
    }

    // Move sentence ptr to START of next word
    while(*sentence){
        if(*sentence == ' ') sentence++;
        else break;
    }

    // If ptr is at NULL terminator -- maybe return -1?
    if(!(*sentence)){
        (*word_ptr) = NULL;
        return 0;
    }

                /* FOUND WORD! */

    //Assign word_ptr to start of next word in sentence
    (*word_ptr) = sentence;

    // Get length of word
    int length = 0;
    while(*sentence){
        if(*sentence++ != ' ') length++;
        else break;
    }

    return length;
}


/**
 * @brief Creates a sentence with the sentence passed in.
 * @return sentence_t*
 * 
 * NOTE: The sentence passed in should be stored on the stack!
 */
sentence_t* allocate_sentence_dynamically(char* sentence){
    if(!sentence){
        printf("Sentence is NULL -- get_word_count %d\n", __LINE__);
        exit(1);
    }

    // Create Sentence Struct
    sentence_t* sentence_data = create_sentence_struct(5); // Default number of words

    char* heap_word = NULL;
    int word_size = 0;
    char* word_ptr_into_sentence = NULL;
    char** new_sentence_ptr = NULL;

    int first_word_signal = 1;
    
    // while (sentence != null and sentence != '\0') -- NOTE: 'num_words' is used as index into heap_sentence
    while(sentence && *sentence){

        word_size = get_next_word_and_size(sentence, &word_ptr_into_sentence, first_word_signal);
        if(!word_ptr_into_sentence) break;
        heap_word = store_word_on_heap(word_ptr_into_sentence, word_size);
        
        /* Double Sentence Size if needed */
        if(sentence_data->num_words >= sentence_data->sentence_size){
            sentence_data->sentence_size*=2;
            new_sentence_ptr = (char**) realloc(sentence_data->sentence, sizeof(char*)*sentence_data->sentence_size);

            if(!new_sentence_ptr){
                free(heap_word);                        // Free word just allocated
                free_sentence_struct(sentence_data);    // Free entire sentence
                exit(1);                                // Exit
            }else{
                sentence_data->sentence = new_sentence_ptr;
            }
        }

        // Store malloc'd word -- each index is char*
        sentence_data->sentence[sentence_data->num_words] = heap_word;

        // Point sentence at next word
        sentence = word_ptr_into_sentence;
        
        // Added another word to the sentence
        sentence_data->num_words+=1;

        // Clear First Word Signal
        first_word_signal = 0;
    }

    return sentence_data;
}


/**
 * @brief Used to print out dynamically allocated sentence
 * 
 * @param sentence_data struct of dynamically allocated sentence
 */
void print_sentence(sentence_t* sentence_data){
    if(!sentence_data){
        printf("Print Sentence Failure -- Sentence Data NULL %d\n", __LINE__);
        exit(1);
    }

    for(int i = 0; i < sentence_data->num_words; i++){
        printf("%s ", sentence_data->sentence[i]);
    }

    printf("\n");
}



/**
 * @brief The entry point for now.
 * 
 */
int main(int argc, char const *argv[])
{
    if(argc < 2){
        printf("Must have more than 1 arg\n");
        return 1;
    }
    
    sentence_t* sentence = allocate_sentence_dynamically((char*)argv[1]);
    print_sentence(sentence);
    free_sentence_struct(sentence);
    return 0;
}
