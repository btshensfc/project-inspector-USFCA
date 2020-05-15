/**
*  Name: Porfirio Mohabir
 * @file
 *
 * System inspector implementation: a Unix utility that inspects the system it
 * runs on and creates a summarized report for the user using the proc pseudo
 * file system.
 *
 * See specification here: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-1.html
 */

#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>   
#include <limits.h>
#include <math.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#define BUF_SZ 128


/* Function prototypes */
void print_usage(char *argv[]);
char *next_token(char **str_ptr, const char *delim);
void system_info();
void hardware_info();
void task_info();
void uptime_format(int seconds);
int read_line(char *str, ssize_t size, int fd);


/**
 * This struct is a collection of booleans that controls whether or not the
 * various sections of the output are enabled.
 */
struct view_opts {
    bool hardware;
    bool live_view;
    bool system;
    bool task_list;
};

/**
 * Prints help/program usage information.
 *
 * This output is displayed if there are issues with command line option parsing
 * or the user passes in the -h flag.
 */
void print_usage(char *argv[])
{
    printf("Usage: %s [-ahrst] [-l] [-p procfs_dir]\n" , argv[0]);
    printf("\n");
    printf("Options:\n"
"    * -a              Display all (equivalent to -rst, default)\n"
"    * -h              Help/usage information\n"
"    * -l              Live view. Cannot be used with other view options.\n"
"    * -p procfs_dir   Change the expected procfs mount point (default: /proc)\n"
"    * -r              Hardware Information\n"
"    * -s              System Information\n"
"    * -t              Task Information\n");
    printf("\n");
}

/**
* Tokenization function
*/
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}

/**
* Function opens:
*  "sys/kernal/hostname"
*  "sys/kernal/osrelease"
*  "uptime". 
* Display information
*/
void system_info(){
    printf("System Information\n------------------\n");
    int hostname = open("sys/kernel/hostname", O_RDONLY);
    int version = open("sys/kernel/osrelease", O_RDONLY);;
    int uptime = open("uptime", O_RDONLY);
    int seconds = 0;

    char host_name[1024] = {0};
    char version_type[1024]= {0};
    char num_uptime[1024] = {0};

   
    read_line(host_name, 1024, hostname);
    read_line(version_type, 1024, version);
    read_line(num_uptime, 1024, uptime);

    seconds = atoi(num_uptime);

    printf("Hostname: %s", host_name);
    printf("Kernel Version: %s", version_type);
    uptime_format(seconds);
    printf("\n");
}


/**
* Funtions take in a paramater seconds and covrts the seconds
* to years, days, minutes etc. 
* Then dispays the uptime in correct format
*/
void uptime_format(int seconds){
    printf("Uptime: ");

    int days_year = 365;
    int hours_year = 24;
    int seconds_year = 3600; 

    int year = seconds / (days_year * hours_year * seconds_year);

    if(year!=0){
        printf("%d years, ", year);
    }

    seconds %= (days_year * hours_year * seconds_year);

    int day = seconds / (hours_year * seconds_year);

    if(day!=0){
        printf("%d days, ", day);
    }

    seconds %= (hours_year * seconds_year);

    int hour = seconds / (seconds_year);

    if(hour!=0){
        printf("%d hours, ", hour);
    }

    seconds %= (seconds_year);

    int minutes = seconds / 60; 

    if(minutes != 0){
        printf("%d minutes, ", minutes);
    }

    seconds %= 60;

    if(seconds != 0){
        printf("%d seconds ", seconds);
    }
}


/**
*Funtion opens "loadavg" in that in the Proc Directory
*Reads the line and displays the line (load_info)
*/
void load_info(){
    int load_avg = open("loadavg", O_RDONLY);

    char str_loadavg[1024];

    read_line(str_loadavg, BUF_SZ, load_avg);

    printf("Load Average (1/5/15 min): %.15s\n", str_loadavg);
}


/**
* Function opens cpuinfo in the Proc Directory 
* Uses tokenization to display Name of Model and number
* Number of Processors
*/
void cpu_info(){
    int cpu_info = open("cpuinfo", O_RDONLY);

    //Storing line
    char str_cpu[1024] = {0}; 

    //Byte Reader
    ssize_t size_count = 0;

    //ARRAY to print modelname when flag == 1;
    char cpu_model[50] = {0};

    //Flag variable to get first "model name"
    int flag = 0;

    int count_processor = 0;

    while((size_count = read_line(str_cpu, BUF_SZ, cpu_info))){
        //tokenization
        char *current_token;
        char *next_tok = str_cpu;
        char *modelName = strstr(str_cpu, "model name");
        char *processer = strstr(str_cpu, "processor");


        while((current_token = next_token(&next_tok, ":")) != NULL){

            if(current_token == processer){
                count_processor++;
            }//First if
            if(current_token == modelName){
               if(flag < 1){
                    //printf("CPU Model:%s",next_tok);
                    strcpy(cpu_model, next_tok);
                    flag++;
                }//Nested if
            }//Second if
        }//Second Whilemak
    }//First While
    printf("CPU Model:%s",cpu_model);
    printf("Processing Units: %d", count_processor);
    printf("\n");
}//cpu_info


/**
*Funtion opens stat in that in the Proc Directory
*Uses tokenization to display CPU Usage
*/
void stat_info(){
    float idle[2] = {0};
    float total[2] = {0};

    char str_stat[1024]; 

    for(int i = 0; i<2; i++){
        int stat = open("stat", O_RDONLY);
        read_line(str_stat, BUF_SZ, stat);
        char *next_tok = str_stat;
        char *current_token;

        int index = 0;
        while((current_token = next_token(&next_tok, " "))!=NULL){
            if((index >= 1) && (index <= 10)){
                total[i] += atof(current_token);
            }
            if(index == 4){
                idle[i] = atof(current_token);
            }
            index++;
        }
        close(stat);
        sleep(1);
    }

    float cpu_usage = idle[1] - idle[0];
    cpu_usage = cpu_usage/(total[1] - total[0]);
    cpu_usage = 1 - cpu_usage;

    if(isnan(cpu_usage)){
        cpu_usage = 0; 
    }

    char percent_char = '%';
    cpu_usage *= 100; 
    char hashtag_Char = '#';
    char cpu_usage_bar[21]= "--------------------";

    int hashtag = cpu_usage/5;

    printf("CPU Usage:\t[");
    for(int i = 1; i<=20;i++){
        if(hashtag != 0){
            cpu_usage_bar[i] = hashtag_Char;
            hashtag -= 1;
        }
    }
    printf("%s] %.1f%c \n", cpu_usage_bar, cpu_usage, percent_char);
}


/**
 * Function calls other methods that perform the retrieval 
 * of certain hardware inforamtion
 */
void mem_info(){
    int memory_info = open("meminfo", O_RDONLY);
    char str_meminfo [1024]; 
    char *next_tok;
    float total_memory = 0;
    float active_memory = 0;
    
    ssize_t size_count = 0;
    while((size_count = read_line(str_meminfo, BUF_SZ, memory_info))){
       
        next_tok = str_meminfo;
        char *current_token;
        while((current_token = next_token(&next_tok, " ")) != NULL){

            if(strcmp(current_token, "MemTotal:") == 0){
                total_memory = atof(next_token(&next_tok, " "));
            }
            if(strcmp(current_token, "Active:")==0){
                active_memory = atof(next_token(&next_tok, " "));
            }
            //printf("%s\n", current_token);
        }//Second While
    }//First Whilex



    total_memory = total_memory/1024/1024;
    active_memory = active_memory/1024/1024;
    float percent_memory = (active_memory/total_memory)*100;
    int hashtag= round(percent_memory)/5;

    char percent_char = '%';
    char hashtag_Char = '#';
    char mem_usage_bar[21]= "--------------------";


  printf("Memory Usage: [");
    for(int i = 0; i<=20;i++){
        if(hashtag != 0){
            mem_usage_bar[i] = hashtag_Char;
            hashtag -= 1;
        }
    }
    printf("%s] %.1f%c ", mem_usage_bar, percent_memory, percent_char);
    printf("(%.1f GB / %.1f GB)\n",active_memory, total_memory);
}

/**
 * Function calls other methods that perform the retrieval 
 * of certain hardware inforamtion
 */
void hardware_info(){
    printf("Hardware Information\n------------------\n");
    cpu_info();
    load_info();
    stat_info();
    mem_info();
}


/**
 * The task_count function traverse throigh files in the Proc directory and counts
 * the valid files that have tasks or status.
 */
void task_count(){
    DIR *directory;

    int task_count = 0;

    if ((directory = opendir(".")) == NULL) {
        perror("opendir");
    }

    struct dirent *entry;

    while ((entry = readdir(directory)) != NULL) {
       char path[1024];
       strcpy(path, entry->d_name);
       strcat(path, "/status");

       int path_open = open(path, O_RDONLY);

       if(path_open >0 && atoi(entry->d_name)){
        task_count++;
       }
         close(path_open);
    }
    printf("Tasks Running: %d\n", task_count);

  closedir(directory);
}


/**
 * task_info first calls a traverse fucntion that counts the task of multiple
 * proc directories status. 
 * The program also go through each valid file in the directory and uses tokeniation
 * to retrieve the Name, Pid, State, Root User, and Threads. 
 */
void task_info(){
    printf("Task Information\n------------------\n");

    task_count();

    printf("%5s | %12s | %25s | %15s | %s \n",
    "PID", "State", "Task Name", "User", "Tasks");
    printf("------+--------------+---------------------------+-----------------+-------\n");

    DIR *directory;

    if ((directory = opendir(".")) == NULL) {
        perror("opendir");
    }

    struct dirent *entry;
    
    while ((entry = readdir(directory)) != NULL) {
        char path[1024];
        strcpy(path, entry->d_name);
        strcat(path, "/status");
  
        int path_open = open(path, O_RDONLY);

        if(path_open >0 && atoi(entry->d_name)){


            struct passwd *pwd;
            struct stat stat_struct;

            char str_status[1024] = {0};
            char Name[26];
            char State[25];
            char thread_info[25];
            char pid[25];


            while(read_line(str_status, 1024, path_open)){
                  char *current_token;
                  char *next_tok = str_status;
               
                while((current_token = next_token(&next_tok, ":"))!= NULL){
                    if(strcmp(current_token, "Name")==0){
                        strcpy(Name, current_token = next_token(&next_tok, " \t\n"));
                        Name [25] = '\0';

                    }
                    if(strcmp(current_token, "State")==0){
                        strcpy(State, current_token = next_token(&next_tok," \t"));
                        strcpy(State, current_token = next_token(&next_tok,"\t()"));
                    }
                    if(strcmp(current_token, "Threads")==0){
                        strcpy(thread_info, current_token = next_token(&next_tok," \t\n"));
                    }
                    if(strcmp(current_token, "Pid")==0){
                        strcpy(pid, current_token = next_token(&next_tok," \t\n"));
                        //printf("%s\n",pid);
                    }
                    if(strcmp(current_token,"Uid")==0){
                        stat(path,&stat_struct);
                        pwd = getpwuid(stat_struct.st_uid);
                        //printf("%s\n",pwd->pw_name);
                    }
                }
            }
        close(path_open);

       printf("%5s | %12s | %25s | %15s | %s \n", pid, State, Name, pwd->pw_name, thread_info);

        }//if
 
    }//big while
    closedir(directory);
}//task_info

/**
 * read_line program takes in three parameters (Buffer String, Buffer Size, file Descriptor)
 * Reads each line of the file an return the count of bytes
 */
int read_line(char *str, ssize_t size, int fd){
   
    int count = 0;

    char character = {0};

    while(character!='\n' && count < size){
        int size_count = read(fd, &character, 1);
        if(size_count == 0){//END OF LINE
            return count;
        }
        else if(size_count == -1){
            perror("read");
            return -1;
        }

        str[count] = character;
        count++;
    }
    return count;
}

/**
 * Main program entrypoint. Reads command line options and runs the appropriate
 * subroutines to display system information.
 */
int main(int argc, char *argv[])
{
    /* Default location of the proc file system */
    char *procfs_loc = "/proc";

    /* Set to true if we are using a non-default proc location */
    bool alt_proc = false;

    struct view_opts defaults = { true, false, true, true };
    struct view_opts options = { false, false, false, false };

    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "ahlp:rst")) != -1) {
        switch (c) {
            case 'a':
                options = defaults;
                break;
            case 'h':
                print_usage(argv);
                return 0;
            case 'l':
                options.live_view = true;
                break;
            case 'p':
                procfs_loc = optarg;
                alt_proc = true;
                break;
            case 'r':
                options.hardware = true;
                break;
            case 's':
                options.system = true;
                break;
            case 't':
                options.task_list = true;
                break;
            case '?':
                if (optopt == 'p') {
                    fprintf(stderr,
                            "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                } else {
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n", optopt);
                }
                print_usage(argv);
                return 1;
            default:
                abort();
        }
    }

    if (alt_proc == true) {
        LOG("Using alternative proc directory: %s\n", procfs_loc);

        /* Remove two arguments from the count: one for -p, one for the
         * directory passed in: */
        argc = argc - 2;
    }

    if (argc <= 1) {
        /* No args (or -p only). Enable default options: */
        options = defaults;
    }


    if(chdir(procfs_loc)==-1){
        return -1;
    }

    if (options.live_view == true) {
        /* If live view is enabled, we will disable any other view options that
         * were passed in. */
        options = defaults;
        options.live_view = true;
        LOGP("Live view enabled. Ignoring other view options.\n");
    } else {
        LOG("View options selected: %s%s%s\n",
                options.hardware ? "hardware " : "",
                options.system ? "system " : "",
                options.task_list ? "task_list" : "");
    }

    if(options.system){
        system_info();
    }
    if(options.hardware){
        hardware_info();
    }
    if(options.task_list){
        task_info();
    }

    return 0;
}


