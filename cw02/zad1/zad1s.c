//
// Created by mrrys00 on 3/20/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

//void line_cutter(char *fn1, char *fn2) {
//    int fp1 = open(fn1, O_RDONLY);
//    if (fp1 == -1) {
//        printf("cannot open source file\n");
//        exit(1);
//    }
//    int fp2 = open(fn2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
//    if (fp2 == -1) {
//        printf("cannot open destination file\n");
//        exit(1);
//    }
//    int cnt = 0;
//    char block[max_line_len];
//
//    while ((cnt = read(fp1, block, max_line_len)) > 0) {
//        int char_n_l_i = -1; // -1 = no new line
//        int qualified_to_copy = 1; // 1=true 0=false
//        int offset = 0;
//        
//        for (int i = 0; i < cnt && char_n_l_i == -1; i++) {
//            if (block[i] == '\n')
//                char_n_l_i = i;
//            if (qualified_to_copy && isspace((int) block[i]) == 0)
//                qualified_to_copy = 0;
//        }
//        if (char_n_l_i == -1 && cnt < max_line_len && !qualified_to_copy)
//            char_n_l_i = cnt - 1;
//        if (char_n_l_i == -1)
//            offset += cnt;
//        if (char_n_l_i != -1) {
//            if (!qualified_to_copy) {
//                if (offset > 0) {
//                    lseek(fp1, -(cnt + offset), SEEK_CUR);
//                    while (offset > 0) {
//                        offset -= max_line_len;
//                        read(fp1, block, max_line_len);
//                        write(fp2, block, max_line_len);
//                    }
//                    read(fp1, block, max_line_len);
//                }
//                write(fp2, block, char_n_l_i + 1);
//            }
//            lseek(fp1, -(cnt - char_n_l_i - 1), SEEK_CUR);
//        }
//    }
//    close(fp1);
//    close(fp2);
//}

int main(int argc, char *args[]) {
    char* fn1;
    char* fn2;
    if (argc < 3) {
        char buffer[4095];

        scanf("input file name:\n %s", (char *) &buffer);
        fn1 = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(fn1, buffer);

        scanf("output file name:\n %s", (char *) &buffer);
        fn2 = calloc(strlen(buffer) + 1, sizeof(char));
        strcpy(fn2, buffer);
    } else {
        fn1 = args[1];
        fn2 = args[2];
    }

//    start time
//    line_cutter(fn1, fn2);
    int fp1 = open(fn1, O_RDONLY);
    if (fp1 == -1) {
        printf("cannot open source file\n");
        exit(1);
    }
    int fp2 = open(fn2, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fp2 == -1) {
        printf("cannot open destination file\n");
        exit(1);
    }
    int cnt = 0, max_line_len = 2, qualified_to_copy = 0, offset = 0;
    char block[max_line_len];

    while ((cnt = read(fp1, block, max_line_len)) > 0) {
        int i = 0, char_n_l_i = -1; // -1 = no new line

        while (i < cnt && char_n_l_i == -1) {
            if (block[i] == '\n') {
                char_n_l_i = i;
            } else if (isspace((int) block[i]) == 0) {
                qualified_to_copy = 1;
            }
            i++;
        }

        if (char_n_l_i == -1 && cnt < max_line_len && qualified_to_copy)        // plik jest za krótki i niema końca linii na ostatniej linii i nie pusta
            char_n_l_i = cnt - 1;

        if (char_n_l_i == -1)       // buffer się skończuł a linijka nie
            offset += max_line_len;     // ile musimy się cofnąć aby odzyskać buffer który pominęliśmy

        if (char_n_l_i != -1) {         // mamy koniec linii
            if (qualified_to_copy) {    // linia nie pusta
                if (offset > 0) {       // linijka większa od buffera -ew można wyrzucić
                    lseek(fp1, -(cnt + offset), SEEK_CUR);
                    while (offset > 0) {
                        offset -= max_line_len;
                        read(fp1, block, max_line_len);
                        write(fp2, block, max_line_len);
                    }
                    read(fp1, block, max_line_len);
                }
                write(fp2, block, char_n_l_i + 1);
            }
            lseek(fp1, -cnt + char_n_l_i + 1, SEEK_CUR);
            offset = 0; qualified_to_copy = 0;
        }
    }
    close(fp1);
    close(fp2);
//    end time

    return 0;

}
