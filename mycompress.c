#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#pragma pack(push, 1)

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

/* struct declarations */
typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
}fileHeader;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
}infoHeader;

typedef struct node
{
    BYTE pixel;
    int freq;
    struct node *left, *right;
    int code[50];
    int code_length;
}node;

typedef struct Heap {
    node **arr;
    int size;
    int capacity;
}heap;

typedef struct code {
    int code[50];
    int code_length;
}code;

void HuffmanCodes(char data[], int freq[], int size);
heap* createHeap(int capacity, node **nums);
void insertHelper(heap *h, int index);
void heapify(heap *h, int index);
node* extractMin(heap* h);
void insert(heap *h, node *data);
void printHeap(heap* h);
void treeTraversal(node* current, int* array, int top);

int main(int argc, char *argv[]) {


    int row_size, i, j, k, num_nodes, compressed, color; 
    float time;
    FILE *in, *out; 
    fileHeader in_fh; 
    infoHeader in_ih; 
    BYTE *in_data, *pad;
    int pixel_freq[256], *array;
    node **list_of_nodes, *root, *tempNode;
    heap *hp;
    code *code_array;
    char *loc, *out_name;
    const char *out_name_;
    clock_t start, end;

    int pixel_freq_red[256], pixel_freq_green[256], pixel_freq_blue[256];
    int num_nodes_red, num_nodes_blue, num_nodes_green;
    node **list_of_nodes_red, **list_of_nodes_green, **list_of_nodes_blue;
    node *root_red, *root_green, *root_blue;
    heap *hp_red, *hp_blue, *hp_green;
    code *code_array_red, *code_array_green, *code_array_blue;
    int *array_red, *array_green, *array_blue;

    start = clock();
    loc = malloc(sizeof(char) * 1);
    loc = strchr(argv[1], '-');
    if(*(loc+1) == (char)'g') {
        color = 0;
    }
    else if(*(loc+1) == (char)'c') {
        color = 1;
    }
    loc = strchr(argv[2], (char)'.');
    if(*(loc+1) == (char)'b') {
        compressed = 0;
    }
    else if(*(loc+1) == (char)'c') {
        compressed = 1;
    }

    out_name = malloc((sizeof(argv[2]) * sizeof(char)) + 1);
    for(i = 0; argv[2][i] != (char)'.'; i++) {
        out_name[i] = argv[2][i];
    }
    out_name[i] = (char)'.';
    if(compressed) {
        out_name[i+1] = (char)'b';
        out_name[i+2] = (char)'m';
        out_name[i+3] = (char)'p';
    } else {
        out_name[i+1] = (char)'c';
        out_name[i+2] = (char)'b';
        out_name[i+3] = (char)'m';
        out_name[i+4] = (char)'p';
    }
    out_name_ = out_name;

    in = fopen(argv[2], "rb");
    out = fopen(out_name_, "wb");

    /* get input image header */
    fread(&in_fh.bfType, sizeof(WORD), 1, in);
    fread(&in_fh.bfSize, sizeof(DWORD), 1, in);
    fread(&in_fh.bfReserved1, sizeof(WORD), 1, in);
    fread(&in_fh.bfReserved2, sizeof(WORD), 1, in);
    fread(&in_fh.bfOffBits, sizeof(DWORD), 1, in);
    fread(&in_ih.biSize, sizeof(DWORD), 1, in);
    fread(&in_ih.biWidth, sizeof(LONG), 1, in);
    fread(&in_ih.biHeight, sizeof(LONG), 1, in);
    fread(&in_ih.biPlanes, sizeof(WORD), 1, in);
    fread(&in_ih.biBitCount, sizeof(WORD), 1, in);
    fread(&in_ih.biCompression, sizeof(DWORD), 1, in);
    fread(&in_ih.biSizeImage, sizeof(DWORD), 1, in);
    fread(&in_ih.biXPelsPerMeter, sizeof(LONG), 1, in);
    fread(&in_ih.biYPelsPerMeter, sizeof(LONG), 1, in);
    fread(&in_ih.biClrUsed, sizeof(DWORD), 1, in);
    fread(&in_ih.biClrImportant, sizeof(DWORD), 1, in);

    /* print header to outfile */
    fwrite(&in_fh.bfType, sizeof(WORD), 1, out);
    fwrite(&in_fh.bfSize, sizeof(DWORD), 1, out);
    fwrite(&in_fh.bfReserved1, sizeof(WORD), 1, out);
    fwrite(&in_fh.bfReserved2, sizeof(WORD), 1, out);
    fwrite(&in_fh.bfOffBits, sizeof(DWORD), 1, out);
    fwrite(&in_ih.biSize, sizeof(DWORD), 1, out);
    fwrite(&in_ih.biWidth, sizeof(LONG), 1, out);
    fwrite(&in_ih.biHeight, sizeof(LONG), 1, out);
    fwrite(&in_ih.biPlanes, sizeof(WORD), 1, out);
    fwrite(&in_ih.biBitCount, sizeof(WORD), 1, out);
    fwrite(&in_ih.biCompression, sizeof(DWORD), 1, out);
    fwrite(&in_ih.biSizeImage, sizeof(DWORD), 1, out);
    fwrite(&in_ih.biXPelsPerMeter, sizeof(LONG), 1, out);
    fwrite(&in_ih.biYPelsPerMeter, sizeof(LONG), 1, out);
    fwrite(&in_ih.biClrUsed, sizeof(DWORD), 1, out);
    fwrite(&in_ih.biClrImportant, sizeof(DWORD), 1, out);

    row_size = ((in_ih.biWidth * in_ih.biBitCount + 31) / 32) * 4;

    if(!compressed) {

        /* create room for data */
        in_data = mmap(NULL, row_size * in_ih.biHeight * sizeof(BYTE), 
            PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
        if (in_data == (void*)-1) {
            printf("MMAP FAILED for INFILE DATA");
            exit(1);
        }
        
        pad = mmap(NULL, in_ih.biHeight * 4 * sizeof(BYTE) + 1, 
            PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
        if (pad == (void*)-1) {
            printf("MMAP FAILED for IN/OUTFILE PADDING DATA");
            exit(1);
        }

        /* get input image pixel data */
        for (i = 0; i < in_ih.biHeight; i++) {
            for (j = 0; j < row_size; j++) {
                if (j < (in_ih.biWidth*3)) {
                    fread(in_data + i * row_size + j, sizeof(BYTE), 1, in);
                } else {
                    fread(&pad, sizeof(BYTE), 1, in);
                }
            }
        }
        /*
        for (i = 0; i < in_ih.biHeight; i++) {
            for (j = 0; j < row_size; j++) {
                printf("%d ", in_data[i * row_size + j]);
            }
        }*/

        /* set all pixel frequencies to 0 so i can increment */
        for (i = 0; i < 256; i++) {
            if(!color) {
                pixel_freq[i] = 0;
            } else {
                pixel_freq_red[i] = 0;
                pixel_freq_green[i] = 0;
                pixel_freq_blue[i] = 0;
            }
        }

        /* get pixel frequencies */
        for (i = 0; i < in_ih.biHeight; i++) {
            for (j = 0; j < row_size; j+=3) {
                if(!color) {
                    int gray = in_data[i * row_size + j] + 
                                in_data[i * row_size + j + 1] + 
                                in_data[i * row_size + j + 2];
                    gray /= 3;
                    in_data[i * row_size + j] = gray;
                    in_data[i * row_size + j + 1] = gray;
                    in_data[i * row_size + j + 2] = gray;
                    pixel_freq[gray]++;
                } else {
                    pixel_freq_red[in_data[i * row_size + j]]++;
                    pixel_freq_green[in_data[i * row_size + j + 1]]++;
                    pixel_freq_blue[in_data[i * row_size + j + 2]]++;
                }
            }
        }

        /* get number of pixels that have frequency > 0*/
        if(!color) {
            num_nodes = 0;
            for(i = 0; i < 256; i++) {
                if (pixel_freq[i] > 0) {
                    num_nodes++;
                }
            }
        } else {
            num_nodes_red = 0;
            num_nodes_green = 0;
            num_nodes_blue = 0;
            for(i = 0; i < 256; i++) {
                if (pixel_freq_red[i] > 0) {
                    num_nodes_red++;
                }
            }
            for(i = 0; i < 256; i++) {
                if (pixel_freq_green[i] > 0) {
                    num_nodes_green++;
                }
            }
            for(i = 0; i < 256; i++) {
                if (pixel_freq_blue[i] > 0) {
                    num_nodes_blue++;
                }
            }
        }
    
        /* create list of pixels with freq */
        if (!color) {
            list_of_nodes = malloc(sizeof(node) * num_nodes);
            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes[j] = n;
                    j++;
                }
            }
        } else {
            list_of_nodes_red = malloc(sizeof(node) * num_nodes_red);
            list_of_nodes_green = malloc(sizeof(node) * num_nodes_green);
            list_of_nodes_blue = malloc(sizeof(node) * num_nodes_blue);

            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq_red[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq_red[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes_red[j] = n;
                    j++;
                }
            }

            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq_green[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq_green[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes_green[j] = n;
                    j++;
                }
            }

            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq_blue[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq_blue[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes_blue[j] = n;
                    j++;
                }
            }
        }
/*
        for(i = 0; i < num_nodes; i++) {
            printf("%3d, %d \n", list_of_nodes[i]->pixel, list_of_nodes[i]->freq);
        }*/

        /* create heap implemention of priorty queue to keep constant order of nodes 
            HELPER FUNCTIONS BELOW USED */
        if (!color) {
            hp = createHeap(num_nodes, list_of_nodes);
        } else {
            hp_red = createHeap(num_nodes_red, list_of_nodes_red);
            hp_green = createHeap(num_nodes_green, list_of_nodes_green);
            hp_blue = createHeap(num_nodes_blue, list_of_nodes_blue);
        }

        /* create huff tree */
        if (!color) {
            while(hp->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp);
                r = extractMin(hp);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp, p);
            }
            root = extractMin(hp);
        } else {
            while(hp_red->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp_red);
                r = extractMin(hp_red);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp_red, p);
            }
            root_red = extractMin(hp_red);

            while(hp_green->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp_green);
                r = extractMin(hp_green);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp_green, p);
            }
            root_green = extractMin(hp_green);

            while(hp_blue->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp_blue);
                r = extractMin(hp_blue);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp_blue, p);
            }
            root_blue = extractMin(hp_blue);
        }

        /* traverse huff tree */
        if (!color) {
            array = (int*)malloc(50 * sizeof(int));
            treeTraversal(root, array, 0);
        } else {
            array_red = (int*)malloc(50 * sizeof(int));
            treeTraversal(root_red, array_red, 0);

            array_green = (int*)malloc(50 * sizeof(int));
            treeTraversal(root_green, array_green, 0);

            array_blue = (int*)malloc(50 * sizeof(int));
            treeTraversal(root_blue, array_blue, 0);
        }

        /* create new array where index is pixel for better search time */
        if (!color) {
            code_array = (code*)malloc(sizeof(code) * 256);
            j = 0;
            for (i = 0; i < 256; i++) {
                code c;
                if(j < num_nodes && list_of_nodes[j]->pixel == (char)i) {
                    c.code_length = list_of_nodes[j]->code_length;
                    for(k = 0; k < c.code_length; k++) {
                        c.code[k] = list_of_nodes[j]->code[k];
                    }
                    j++;
                } else {
                    c.code_length = 0;
                    c.code[0] = 0;
                }   
                code_array[i] = c;
            }
        } else {
            code_array_red = (code*)malloc(sizeof(code) * 256);
            j = 0;
            for (i = 0; i < 256; i++) {
                code c;
                if(j < num_nodes_red && list_of_nodes_red[j]->pixel == (char)i) {
                    c.code_length = list_of_nodes_red[j]->code_length;
                    for(k = 0; k < c.code_length; k++) {
                        c.code[k] = list_of_nodes_red[j]->code[k];
                    }
                    j++;
                } else {
                    c.code_length = 0;
                    c.code[0] = 0;
                }   
                code_array_red[i] = c;
            }

            code_array_green = (code*)malloc(sizeof(code) * 256);
            j = 0;
            for (i = 0; i < 256; i++) {
                code c;
                if(j < num_nodes_green && list_of_nodes_green[j]->pixel == (char)i) {
                    c.code_length = list_of_nodes_green[j]->code_length;
                    for(k = 0; k < c.code_length; k++) {
                        c.code[k] = list_of_nodes_green[j]->code[k];
                    }
                    j++;
                } else {
                    c.code_length = 0;
                    c.code[0] = 0;
                }   
                code_array_green[i] = c;
            }

            code_array_blue = (code*)malloc(sizeof(code) * 256);
            j = 0;
            for (i = 0; i < 256; i++) {
                code c;
                if(j < num_nodes_blue && list_of_nodes_blue[j]->pixel == (char)i) {
                    c.code_length = list_of_nodes_blue[j]->code_length;
                    for(k = 0; k < c.code_length; k++) {
                        c.code[k] = list_of_nodes_blue[j]->code[k];
                    }
                    j++;
                } else {
                    c.code_length = 0;
                    c.code[0] = 0;
                }   
                code_array_blue[i] = c;
            }
        }
/*
        printf("\n");

        for(i = 0; i < 256; i++) {
            if(code_array[i].code_length != 0) {
                printf("%3d -> ", i);
                for(j = 0; j < code_array[i].code_length; j++) {
                    printf("%d", code_array[i].code[j]);
                }
                printf("\n");
            }
        }*/

        /* write the pixel data into the outfile so it can be accessed after compression */
        if (!color) {
            fwrite(pixel_freq, sizeof(int) * 256, 1, out);
        } else {
            fwrite(pixel_freq_red, sizeof(int) * 256, 1, out);
            fwrite(pixel_freq_green, sizeof(int) * 256, 1, out);
            fwrite(pixel_freq_blue, sizeof(int) * 256, 1, out);
        }

        /* encode image */
        if (!color) {
            for (i = 0; i < in_ih.biHeight; i++) {
                for (j = 0; j < row_size; j+=3) {
                    int pixel = in_data[i * row_size + j];
                    for(k = 0; k < code_array[pixel].code_length; k++) {
                        fwrite(&(code_array[pixel].code[k]), sizeof(int), 1, out);
                    }
                }
            }
        } else {
            for (i = 0; i < in_ih.biHeight; i++) {
                for (j = 0; j < row_size; j+=3) {
                    int pixel = in_data[i * row_size + j];
                    for(k = 0; k < code_array_red[pixel].code_length; k++) {
                        fwrite(&(code_array_red[pixel].code[k]), sizeof(int), 1, out);
                    }
                    for(k = 0; k < code_array_blue[pixel].code_length; k++) {
                        fwrite(&(code_array_green[pixel].code[k]), sizeof(int), 1, out);
                    }
                    for(k = 0; k < code_array_green[pixel].code_length; k++) {
                        fwrite(&(code_array_blue[pixel].code[k]), sizeof(int), 1, out);
                    }
                }
            }
        }
        
    } else {
        if (!color) {
            fread(pixel_freq, sizeof(int) * 256, 1, in);
        } else {
            fread(pixel_freq_red, sizeof(int) * 256, 1, in);
            fread(pixel_freq_green, sizeof(int) * 256, 1, in);
            fread(pixel_freq_blue, sizeof(int) * 256, 1, in);
        }

        if(!color) {
            num_nodes = 0;
            for(i = 0; i < 256; i++) {
                if (pixel_freq[i] > 0) {
                    num_nodes++;
                }
            }
        } else {
            num_nodes_red = 0;
            num_nodes_green = 0;
            num_nodes_blue = 0;
            for(i = 0; i < 256; i++) {
                if (pixel_freq_red[i] > 0) {
                    num_nodes_red++;
                }
            }
            for(i = 0; i < 256; i++) {
                if (pixel_freq_green[i] > 0) {
                    num_nodes_green++;
                }
            }
            for(i = 0; i < 256; i++) {
                if (pixel_freq_blue[i] > 0) {
                    num_nodes_blue++;
                }
            }
        }

        if (!color) {
            list_of_nodes = malloc(sizeof(node) * num_nodes);
            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes[j] = n;
                    j++;
                }
            }
        } else {
            list_of_nodes_red = malloc(sizeof(node) * num_nodes_red);
            list_of_nodes_green = malloc(sizeof(node) * num_nodes_green);
            list_of_nodes_blue = malloc(sizeof(node) * num_nodes_blue);

            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq_red[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq_red[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes_red[j] = n;
                    j++;
                }
            }

            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq_green[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq_green[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes_green[j] = n;
                    j++;
                }
            }

            for(i = 0, j = 0; i < 256; i++) {
                if(pixel_freq_blue[i] > 0) {
                    node *n;
                    n = (node*)malloc(sizeof(node));
                    n->freq = pixel_freq_blue[i];
                    n->pixel = i;
                    n->left = NULL;
                    n->right = NULL;
                    n->code_length = 0;
                    list_of_nodes_blue[j] = n;
                    j++;
                }
            }
        }

        /* create heap implemention of priorty queue to keep constant order of nodes 
            HELPER FUNCTIONS BELOW USED */
        if (!color) {
            hp = createHeap(num_nodes, list_of_nodes);
        } else {
            hp_red = createHeap(num_nodes_red, list_of_nodes_red);
            hp_green = createHeap(num_nodes_green, list_of_nodes_green);
            hp_blue = createHeap(num_nodes_blue, list_of_nodes_blue);
        }

        /* create huff tree */
        if (!color) {
            while(hp->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp);
                r = extractMin(hp);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp, p);
            }
            root = extractMin(hp);
        } else {
            while(hp_red->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp_red);
                r = extractMin(hp_red);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp_red, p);
            }
            root_red = extractMin(hp_red);

            while(hp_green->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp_green);
                r = extractMin(hp_green);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp_green, p);
            }
            root_green = extractMin(hp_green);

            while(hp_blue->size > 1) {
                node *l, *r, *p;

                l = (node*)malloc(sizeof(node));
                r = (node*)malloc(sizeof(node));
                p = (node*)malloc(sizeof(node));

                l = extractMin(hp_blue);
                r = extractMin(hp_blue);

                p->freq = l->freq + r->freq;
                if(l->pixel > r->pixel) {
                    p->pixel = l->pixel;
                } else {
                    p->pixel = r->pixel;
                }
                p->left = l;
                p->right = r;

                insert(hp_blue, p);
            }
            root_blue = extractMin(hp_blue);
        }

        /* traverse huff tree */
        if (!color) {
            array = (int*)malloc(50 * sizeof(int));
            treeTraversal(root, array, 0);
        } else {
            array_red = (int*)malloc(50 * sizeof(int));
            treeTraversal(root_red, array_red, 0);

            array_green = (int*)malloc(50 * sizeof(int));
            treeTraversal(root_green, array_green, 0);

            array_blue = (int*)malloc(50 * sizeof(int));
            treeTraversal(root_blue, array_blue, 0);
        }

        if (!color) {
            code_array = (code*)malloc(sizeof(code) * 256);
            j = 0;
            for (i = 0; i < 256; i++) {
                code c;
                if(j < num_nodes && list_of_nodes[j]->pixel == (char)i) {
                    c.code_length = list_of_nodes[j]->code_length;
                    for(k = 0; k < c.code_length; k++) {
                        c.code[k] = list_of_nodes[j]->code[k];
                    }
                    j++;
                } else {
                    c.code_length = 0;
                    c.code[0] = 0;
                }   
                code_array[i] = c;
            }
        }

        /* decode image */
        if(!color) {
            for (i = 0; i < in_ih.biHeight; i++) {
                for (j = 0; j < row_size;) {
                    int bit;
                    tempNode = root;
                    while(1 < 2) {
                        if(feof(in)) {
                            break;
                        }
                        if(!tempNode->left && !tempNode->right) {
                            bit = tempNode->pixel;
                            fwrite(&bit, sizeof(BYTE), 1, out);
                            fwrite(&bit, sizeof(BYTE), 1, out);
                            fwrite(&bit, sizeof(BYTE), 1, out);
                            tempNode = root;
                            break;
                        } else {
                            fread(&bit, sizeof(int), 1, in);
                            if (bit == 0) {
                                tempNode = tempNode->left;
                            } else {
                                tempNode = tempNode->right;
                            }
                        }
                    }
                    j+=3;
                    
                }
            }
        } else {
            for (i = 0; i < in_ih.biHeight; i++) {
                for (j = 0; j < row_size; ) {
                    int bit;
                    tempNode = root_red;
                    while(1 < 2) {
                        if(feof(in)) {
                            break;
                        }
                        if(!tempNode->left && !tempNode->right) {
                            bit = tempNode->pixel;
                            fwrite(&bit, sizeof(BYTE), 1, out);
                            tempNode = root;
                            break;
                        } else {
                            fread(&bit, sizeof(int), 1, in);
                            if (bit == 0) {
                                tempNode = tempNode->left;
                            } else {
                                tempNode = tempNode->right;
                            }
                        }
                    }

                    tempNode = root_green;
                    while(1 < 2) {
                        if(feof(in)) {
                            break;
                        }
                        if(!tempNode->left && !tempNode->right) {
                            bit = tempNode->pixel;
                            fwrite(&bit, sizeof(BYTE), 1, out);
                            tempNode = root;
                            break;
                        } else {
                            fread(&bit, sizeof(int), 1, in);
                            if (bit == 0) {
                                tempNode = tempNode->left;
                            } else {
                                tempNode = tempNode->right;
                            }
                        }
                    }

                    tempNode = root_blue;
                    while(1 < 2) {
                        if(feof(in)) {
                            break;
                        }
                        if(!tempNode->left && !tempNode->right) {
                            bit = tempNode->pixel;
                            fwrite(&bit, sizeof(BYTE), 1, out);
                            tempNode = root;
                            break;
                        } else {
                            fread(&bit, sizeof(int), 1, in);
                            if (bit == 0) {
                                tempNode = tempNode->left;
                            } else {
                                tempNode = tempNode->right;
                            }
                        }
                    }
                    j+=3;
                    /*
                    if(j > in_ih.biWidth*3) {
                        int diff = row_size - in_ih.biWidth*3;
                        int zerobit = 10;
                        for (k = 1; k < diff; k++) {
                            fwrite(&zerobit, sizeof(BYTE), 1, out);
                        }
                    }
                    */
                }
            }
        }
    }
    
    /* close files, free memory, return from main */
    end = clock();
    fclose(in);
    fclose(out);
    munmap(in_data, row_size * in_ih.biHeight * sizeof(BYTE));
    munmap(pad, in_ih.biHeight * 4 * sizeof(BYTE) + 1);
    time = (end-start) / (CLOCKS_PER_SEC / 1000);
    printf("done in %f ms\n", time);
    return 0;   
}

void treeTraversal(node* current, int* array, int top) {
    int i;
    if(current->left) {
        array[top] = 0;
        treeTraversal(current->left, array, top + 1);
    }
    if(current->right) {
        array[top] = 1;
        treeTraversal(current->right, array, top + 1);
    }
    if(!(current->left || current->right)) {
        for(i = 0; i < top; i++) {
            current->code[i] = array[i];
            current->code_length = top;
        }
    }
}

heap* createHeap(int capacity, node **nums)
{
    int i;
    heap* h = (heap*)malloc(sizeof(heap));
 
    if (h == NULL) {
        printf("Memory error");
        return NULL;
    }
    h->size = 0;
    h->capacity = capacity;
 
    h->arr = (node**)malloc(capacity * sizeof(node));
 
    if (h->arr == NULL) {
        printf("Memory error");
        return NULL;
    }
    for (i = 0; i < capacity; i++) {
        h->arr[i] = nums[i];
    }
 
    h->size = i;
    i = (h->size - 2) / 2;
    while (i >= 0) {
        heapify(h, i);
        i--;
    }
    return h;
}

void insertHelper(heap* h, int index)
{
    node *temp;
    int parent = (index - 1) / 2;
    if ((*(h->arr[parent])).freq > (*(h->arr[index])).freq) {
        temp = (node*)malloc(sizeof(node));
        temp = h->arr[parent];
        h->arr[parent] = h->arr[index];
        h->arr[index] = temp;
 
        insertHelper(h, parent);
    }
}

void heapify(heap* h, int index)
{
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int min = index;
    node* temp;
 
    if (left >= h->size || left < 0)
        left = -1;
    if (right >= h->size || right < 0)
        right = -1;
 
    if (left != -1 && (*(h->arr[left])).freq < (*(h->arr[index])).freq)
        min = left;
    if (right != -1 && (*(h->arr[right])).freq < (*(h->arr[min])).freq)
        min = right;
 
    if (min != index) {
        temp = (node*)malloc(sizeof(node));
        temp = h->arr[min];
        h->arr[min] = h->arr[index];
        h->arr[index] = temp;
        heapify(h, min);
    }
}
 
node* extractMin(heap* h)
{
    node *deleteItem;
    node *failure;

    deleteItem = (node*)malloc(sizeof(node));
    failure = (node*)malloc(sizeof(node));

    failure->freq = -1;
    failure->pixel = -1;
    failure->left = NULL;
    failure->right = NULL;
 
    if (h->size == 0) {
        printf("\nHeap id empty.");
        return failure;
    }
 
    deleteItem = h->arr[0];
 
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
 
    heapify(h, 0);
    return deleteItem;
}
 
void insert(heap *h, node *data)
{
    if (h->size < h->capacity) {
        h->arr[h->size] = data;
        insertHelper(h, h->size);
        h->size++;
    }
}
 
void printHeap(heap* h)
{
    int i;
    for (i = 0; i < h->size; i++) {
        printf("p: %3d, f: %6d\n", (*(h->arr[i])).pixel, (*(h->arr[i])).freq);
    }
    printf("\n");
}