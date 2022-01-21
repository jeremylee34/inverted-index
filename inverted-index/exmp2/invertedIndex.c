// invertedIndex.c 
// z5316983: by Jeremy Lee
// Started Week 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "invertedIndex.h"

#define MAX_LENGTH 500


InvertedIndexBST insert_node(InvertedIndexBST node, char *word, char *file);
InvertedIndexBST new_node(char *word, char *file);
FileList insert_file(FileList node, char *file, int new);
FileList binarySearch(InvertedIndexBST node, char *searchWord);
void printTree(InvertedIndexBST tree, FILE *outputFileName);
double tf(FILE *file_stream, char *key);

/*
int main() {
     
    InvertedIndexBST node = generateInvertedIndex("collection.txt");
    // updating tf values occurs in printInvertedIndex
    printInvertedIndex(node); 

    FileList list = binarySearch(node, "mars");
    while (list != NULL) {
        printf("%s (%lf)\n", list->filename, list->tf);
        list = list->next;
        
    }
    
}
*/


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
    
    // If list is empty, set new_file_node as head
    if (empty == 1) {
        // creates a tree
        new_file_node->next = NULL;   
        head = new_file_node;
    
    // Else, find correct position to insert
    } else if (head != NULL && empty == 0) {

        // If before head, insert normally
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
            // if not found, add to end of list.
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
    printTree(tree, pf);
    fclose(pf);
}

// Recursive function to print inorder
void printTree(InvertedIndexBST tree, FILE *outputFileName) {

    if (tree == NULL) return;
    printTree(tree->left, outputFileName);

    // Print the word, including the whole fileList
    FileList curr_file = tree->fileList;
    fprintf(outputFileName, "%s ", tree->word);
    // Loop through each file in the current node's linked list
    while (curr_file != NULL) {
        // store the number of words in each file temporarily
        FILE *file_stream = fopen(curr_file->filename, "r");
        // update the value of tf
        curr_file->tf = tf(file_stream, tree->word);
        fclose(file_stream);
        fprintf(outputFileName, "%s (%lf) ", curr_file->filename, curr_file->tf);
        curr_file = curr_file->next;
    }
    fprintf(outputFileName, "\n");

    printTree(tree->right, outputFileName); 
}

// Returns the tf value of a given word in a given file
double tf(FILE *file_stream, char *key) {

    char word[MAX_LENGTH][MAX_LENGTH];
    double counter = 0;
    int i = 0;
    for (i = 0; fscanf(file_stream, "%s", word[i]) == 1; i++) {
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
    for (i = 0; str[i] != '\0'; i++) str[i] = tolower(str[i]);        

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
    i = strlen(str);
    if (str[i - 1] == '.' || str[i - 1] == ',' || 
        str[i - 1] == ';' || str[i - 1] == '?') str[i - 1] = '\0';

    return str;
}

/// make a TfIdfList for each word, then print the intersection
// of all the lists, summing the tfidf values
TfIdfList calculateTfIdf(InvertedIndexBST tree, char *searchWord, int D) {
    TfIdfList new = NULL;
    
    return new;
}


FileList binarySearch(InvertedIndexBST node, char *searchWord) {

    if (strcmp(node->word, searchWord) == 0) {
        return node->fileList;
    } else if (strcmp(node->word, searchWord) < 0) {
        return binarySearch(node->right, searchWord);
    } else if (strcmp(node->word, searchWord) > 0) {
        return binarySearch(node->left, searchWord);
    } else {
        return node->fileList;
    }
    
}

TfIdfList retrieve(InvertedIndexBST tree, char *searchWords[], int D) {
    TfIdfList new = NULL;

    /* int i = 0;
    for (i = 0; searchWords[i] != NULL; i++) {


    } */
    
    return new;
}




