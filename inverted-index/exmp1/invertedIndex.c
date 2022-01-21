// invertedIndex.c 
// z5316983: by Jeremy Lee
// Started Week 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include "invertedIndex.h"

#define MAX_LENGTH 500


InvertedIndexBST insert_node(InvertedIndexBST node, char *word, char *file);
InvertedIndexBST new_node(char *word, char *file);
FileList insert_file(FileList node, char *file, int new);
FileList binary_search(InvertedIndexBST node, char *searchWord);
TfIdfList translate_list(FileList files, int D);
TfIdfList insert_tfIdf_node(char *filename, double tf_idf_val);
void print_tree(InvertedIndexBST tree, FILE *output_file);
double tf(FILE *file_stream, char *key);
double tf_idf(int cases, int D, double tf);
TfIdfList combine(TfIdfList reference, TfIdfList curr);
TfIdfList sort_list(TfIdfList combined_list);
void swap(TfIdfList temp1, TfIdfList temp2);

int main() {
     
    InvertedIndexBST node = generateInvertedIndex("collection.txt");
    // updating tf values occurs in printInvertedIndex
    printInvertedIndex(node); 
   
    int i = 7;
    char *words[MAX_LENGTH] = { "nasa", "mars", "moon", NULL };
    TfIdfList list = retrieve(node, words, i);
    printf("Sorted:\n");
    TfIdfList temp = list;
    while (list != NULL) {
        printf("%lf  %s\n", list->tfIdfSum, list->filename);
        list = list->next;
    }
    
    FILE *print = fopen("nasa_mars_moon_TfIdfList.txt", "w");
    while (temp != NULL) {
        fprintf(print, "%lf  %s\n", temp->tfIdfSum, temp->filename);
        temp = temp->next;
    }
    fclose(print);    
}

// Generates an inverted index and returns the tree
InvertedIndexBST generateInvertedIndex(char *collectionFilename) {

    char file_name[MAX_LENGTH];
    FILE *fp = fopen(collectionFilename, "r");
    InvertedIndexBST node = NULL;
    // Scanning the names of files in collection.txt into file_name
    while (fscanf(fp, "%s", file_name) == 1) {
        
        // Opening the file and scanning strings from the stream "file_stream"
        FILE *file_stream = fopen(file_name, "r");
        int i = 0;
        char word[MAX_LENGTH];
        
        for (i = 0; fscanf(file_stream, "%s", word) == 1; i++) {
            // inserting into tree
            node = insert_node(node, normaliseWord(word), file_name);
        }        
        fclose(file_stream);
    }   
    fclose(fp);
    return node;
}

// Inserting a node
InvertedIndexBST insert_node(InvertedIndexBST node, char *word, char *file) {
    
    if (node == NULL) {
        node = new_node(word, file);
    } else if (strcmp(node->word, word) < 0) {
        node->right = insert_node(node->right, word, file);
    } else if (strcmp(node->word, word) > 0) {
        node->left = insert_node(node->left, word, file);
    } else if (strcmp(node->word, word) == 0) {
        
        // search through linked list to find file
        FileList curr = node->fileList;
        while (curr->next != NULL && strcmp(curr->filename, file) != 0) {
            curr = curr->next;
        }
        // If the fileList doesn't have file, insert new file.
        if (strcmp(curr->filename, file) != 0) {    
            int empty = 0;
            node->fileList = insert_file(node->fileList, file, empty);
        }
    }

    return node;
}

// Create a new index node
InvertedIndexBST new_node(char *word, char *file) {

    InvertedIndexBST new_node = malloc(sizeof(struct InvertedIndexNode));
    new_node->word = malloc(sizeof(char[MAX_LENGTH][MAX_LENGTH]));
    new_node->right = NULL;
    new_node->left = NULL;
    strcpy(new_node->word, word);
    // Create a file linked list
    // if new == 1, create the fileList as a standalone node
    int new = 1;
    new_node->fileList = insert_file(new_node->fileList, file, new);

    return new_node;
}

// Update the fileList linked list
FileList insert_file(FileList node, char *file, int empty) {

    // Store the head of the list to return later on
    FileList head = node;
    // First, create the node and populate its fields.
    FileList new_file_node = malloc(sizeof(struct FileListNode));
    new_file_node->filename = malloc(sizeof(char[MAX_LENGTH][MAX_LENGTH]));
    strcpy(new_file_node->filename, file);
    new_file_node->tf = 0;
    
    // if list is empty, set new_file_node as head
    if (empty == 1) {
        // creates a tree
        new_file_node->next = NULL;   
        head = new_file_node;   
    // else, find correct position to insert
    } else if (head != NULL && empty == 0) {
        // if before head, insert normally
        if (strcmp(file, head->filename) < 0) {
            new_file_node->next = head;
            head = new_file_node;        
        // else, insert in correct position by looping through
        } else {
            FileList curr = head;
            FileList prev = NULL;
            while (curr != NULL && strcmp(file, curr->filename) > 0) {
                prev = curr;
                curr = curr->next;
            }
            // Now need to insert between prev and curr, 
            // if not found, added to end of list
            new_file_node->next = prev->next;
            prev->next = new_file_node;
        }
    }
    return head;

}

// Printing the tree inorder into invertedIndex.txt
void printInvertedIndex(InvertedIndexBST tree) {

    FILE *pf = fopen("invertedIndex.txt", "w");
    // Call a recursive function to print
    print_tree(tree, pf);
    fclose(pf);
}

// Recursive function to print inorder
void print_tree(InvertedIndexBST tree, FILE *output_file) {

    if (tree == NULL) return;
    print_tree(tree->left, output_file);

    // Print the word, including the whole fileList
    fprintf(output_file, "%s ", tree->word);
    // Loop through each file in the current node's linked list
    FileList curr_file = tree->fileList;
    for (; curr_file != NULL; curr_file = curr_file->next) {
        // store the number of words in each file temporarily
        FILE *file_stream = fopen(curr_file->filename, "r");
        // update the value of tf
        curr_file->tf = tf(file_stream, tree->word);
        fclose(file_stream);
        fprintf(output_file, "%s (%lf) ", curr_file->filename, curr_file->tf);
        //curr_file = curr_file->next;
    }
    fprintf(output_file, "\n");

    print_tree(tree->right, output_file); 
}

// Returns the tf value of a given word in a given file
double tf(FILE *file_stream, char *key) {

    char word[MAX_LENGTH][MAX_LENGTH];
    double counter = 0;
    int i = 0;
    for (; fscanf(file_stream, "%s", word[i]) == 1; i++) {
        if (strcmp(normaliseWord(word[i]), key) == 0) counter++;      
    }
    double total_words = i;
    return counter / total_words;
}

// Normalises a given string. See the spec for details. Note: you should
// modify the given string - do not create a copy of it.
char *normaliseWord(char *str) {
    
    int i = 0;
    // Changing all letters to lowercase
    for (; str[i] != '\0'; i++) str[i] = tolower(str[i]);        

    // Finding the length of the leading white space
    int spaces = 0;
    int length = strlen(str);
    for (i = 0; isspace(str[i]) != 0; i++, spaces++, length--);
    // Shifting the string forwards to remove the white space
    int actual_length = strlen(str);
    for (int j = 0; i < actual_length; i++, j++) str[j] = str[i];
    // Setting the first space on the tail as null terminator to end the string
    for (i = strlen(str) - 1; isspace(str[i]) != 0; i--) str[i] = '\0';

    // Removing certain symbols
    int k = strlen(str);
    if (str[k - 1] == '.' || str[k - 1] == ',' || 
        str[k - 1] == ';' || str[k - 1] == '?') str[k - 1] = '\0';

    return str;
}

/// Make a TfIdfList for each word, then print the combine
// of all the lists, summing the tfidf values
TfIdfList calculateTfIdf(InvertedIndexBST tree, char *searchWord, int D) {
    
    FileList files = binary_search(tree, searchWord);
    TfIdfList new = translate_list(files, D);
    
    return new;
}

// Insert a tfIdf node into the list
TfIdfList insert_tfIdf_node(char *filename, double tf_idf_val) {

    TfIdfList new_node = malloc(sizeof(struct TfIdfNode));
    new_node->filename = malloc(sizeof(char[MAX_LENGTH]));
    new_node->tfIdfSum = tf_idf_val;
    new_node->next = NULL;
    strcpy(new_node->filename, filename);
    return new_node;
}

// Returns the tfIdf value for a file corresponding to a searchWord
double tf_idf(int cases, int D, double tf) {
    
    double total = D, matches = cases;
    double tfIdf = tf * log10(total / matches);
    return tfIdf;
}

// Convert a FileList into a TfIdfList, storing new tf_idf_vals
TfIdfList translate_list(FileList files, int D) {

    // Create temp to count number of files for idf calc
    FileList temp = files;
    int cases, appended_to_head = 0;
    for (cases = 0; temp != NULL; cases++) temp = temp->next;

    // First, create a head.
    double tf_idf_value = tf_idf(cases, D, files->tf);
    TfIdfList new_node = insert_tfIdf_node(files->filename, tf_idf_value);

    // Store the head
    TfIdfList head = new_node;
    if (files->next == NULL) return head;
    // Convert each files node into a valid TfIdfNode and add to head
    for (FileList temp = files; temp != NULL; temp = temp->next) {
        tf_idf_value = tf_idf(cases, D, temp->tf);
        TfIdfList node = insert_tfIdf_node(temp->filename, tf_idf_value);
        new_node->next = node;
        // If head is created, move insertion point one down
        if (appended_to_head == 1) new_node = new_node->next;
        appended_to_head = 1;
    }
    return head;
}

// A function used to search a tree using a binary search algorithm
FileList binary_search(InvertedIndexBST node, char *searchWord) {

    if (strcmp(node->word, searchWord) == 0) {
        return node->fileList;
    } else if (strcmp(node->word, searchWord) < 0) {
        return binary_search(node->right, searchWord);
    } else if (strcmp(node->word, searchWord) > 0) {
        return binary_search(node->left, searchWord);
    } else {
        return node->fileList;
    }
}

TfIdfList retrieve(InvertedIndexBST tree, char *searchWords[], int D) {
    
    // Obtain the union of tfIdfLists by comparing
    // each subsequent tfIdfList against the first one
    int i = 0;
    // Create a list that will be iteraively updated
    TfIdfList combined_list = calculateTfIdf(tree, searchWords[i], D);

    for (i = 1; searchWords[i] != NULL; i++) {
        // Create a list of the next word to compare to combined_list
        TfIdfList curr_list = calculateTfIdf(tree, searchWords[i], D);
        combined_list = combine(combined_list, curr_list);
    } 
    combined_list = sort_list(combined_list);
    return combined_list;
}

TfIdfList sort_list(TfIdfList combined_list) {

    TfIdfList counter = combined_list;
    TfIdfList temp1, temp2;
    // Calculate the length of the list
    int length = 0;
    for (; counter != NULL; length++, counter = counter->next);
    for (int i = length - 2; i >= 0; i--) {
        temp1 = combined_list;
        temp2 = temp1->next;
        for (int j = 0; j <= i; j++) {
            int word_diff = strcmp(temp1->filename, temp2->filename);
            if (temp1->tfIdfSum < temp2->tfIdfSum) swap(temp1, temp2);
            else if (temp1->tfIdfSum == temp2->tfIdfSum && word_diff > 0) swap(temp1, temp2);
            temp1 = temp2;
            temp2 = temp2->next;
        }
    }
    return combined_list;
}

void swap(TfIdfList temp1, TfIdfList temp2) {

    char temp_file[MAX_LENGTH];
    double k = 0;
    // Swap filenames
    strcpy(temp_file, temp1->filename);
    strcpy(temp1->filename, temp2->filename);
    strcpy(temp2->filename, temp_file);
    // Swap tfIdf
    k = temp1->tfIdfSum;
    temp1->tfIdfSum = temp2->tfIdfSum;
    temp2->tfIdfSum = k;

}

// Given two TfIdfLists, return the combine of both lists
TfIdfList combine(TfIdfList reference, TfIdfList curr) {

    TfIdfList head = NULL;
    head = reference;
    // check whether an element of curr is not in reference yet
    TfIdfList current = curr;
    while (current != NULL) {

        TfIdfList ref = head;
        TfIdfList prev = NULL;
        int found = 0;
        while (ref != NULL) {
            if (strcmp(ref->filename, current->filename) == 0) {
                ref->tfIdfSum += current->tfIdfSum;
                found = 1;
            }
            prev = ref;
            ref = ref->next;
        }
        if (found == 0) {
            TfIdfList new_node = NULL;
            new_node = insert_tfIdf_node(current->filename, current->tfIdfSum);
            new_node->next = NULL;
            prev->next = new_node;
        }
        current = current->next;
    }
    head = reference;
    return head;
    
}



