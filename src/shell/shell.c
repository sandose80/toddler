#include "common/include/data.h"
#include "common/include/errno.h"
#include "klibc/include/stdio.h"
#include "klibc/include/stdlib.h"
#include "klibc/include/string.h"
#include "klibc/include/sys.h"


unsigned long cmd_block_salloc_id = 0;


/*
 * stdout and stderr tests
 */
static void test_out()
{
    char stdout_str[] = "Printing through stdout\n";
    char stderr_str[] = "Printing through stderr\n";
    
    kapi_stdout_write(0, stdout_str, sizeof(stdout_str));
    kapi_stderr_write(0, stderr_str, sizeof(stderr_str));
}


/*
 * Welcome message
 */
static void welcome()
{
    char *welcome_msg[] = {
        "  _______        _     _ _           \n",
        " |__   __|      | |   | | |          \n",
        "    | | ___   __| | __| | | ___ _ __ \n",
        "    | |/ _ \\ / _` |/ _` | |/ _ \\ '__|\n",
        "    | | (_) | (_| | (_| | |  __/ |   \n",
        "    |_|\\___/ \\__,_|\\__,_|_|\\___|_|   \n",
        "                                     \n",
        "                                     \n",
    };
    
    int i;
    for (i = 0; i < 8; i++) {
        kprintf(welcome_msg[i]);
    }
}


/*
 * Command input
 */
#define CMD_BLOCK_SIZE  64

struct cmd_block {
    char buf[CMD_BLOCK_SIZE];
    int count;
    
    struct cmd_block *next;
};

struct cmd_builder {
    int count;
    
    struct cmd_block *head;
};

static int input(char **cmd)
{
    char stdin_buf[32];
    int i, offset;
    int done = 0;
    
    char *input = NULL;
    
    struct cmd_builder build;
    struct cmd_block *cur_block = (struct cmd_block *)salloc(cmd_block_salloc_id);
    
    build.count = 0;
    build.head = cur_block;
    
    cur_block->count = 0;
    cur_block->next = NULL;
    
    // Input
    do {
        unsigned long size = kapi_stdin_read(0, stdin_buf, sizeof(stdin_buf));
        if (size) {
            for (i = 0; i < (int)size; i++) {
                char cur = stdin_buf[i];
                kprintf("%c", cur);
                
                if (cur == '\r' || cur == '\n') {
                    done = 1;
                    break;
                }
                
                else {
                    if (cur_block->count >= CMD_BLOCK_SIZE) {
                        struct cmd_block *new_block = (struct cmd_block *)salloc(cmd_block_salloc_id);
                        new_block->count = 0;
                        new_block->next = NULL;
                        
                        cur_block->next = new_block;
                        cur_block = new_block;
                    }
                    
                    cur_block->buf[cur_block->count] = cur;
                    cur_block->count++;
                    build.count++;
                }
            }
        }
    } while (!done);
    
    // Concatinate
    if (build.count) {
        input = calloc(build.count + 1, sizeof(char));
        i = 0;
        
        offset = 0;
        cur_block = build.head;
        while (cur_block && i < build.count) {
            input[i] = cur_block->buf[offset];
            i++;
            offset++;
            
            if (offset >= CMD_BLOCK_SIZE) {
                cur_block = cur_block->next;
            }
        }
    }
    
    // Clean up
    cur_block = build.head;
    while (cur_block) {
        struct cmd_block *rm = cur_block;
        cur_block = cur_block->next;
        sfree(rm);
    }
    
    // Done
    if (cmd) {
        *cmd = input;
    }
    
    return build.count;
}

static void prompt()
{
    int len;
    char *cmd;
    
    do {
        kprintf("system > ");
        len = input(&cmd);
    } while (1);
}


/*
 * Init
 */
static void init_shell()
{
    cmd_block_salloc_id = salloc_create(sizeof(struct cmd_block), 0, NULL, NULL);
}

int main(int argc, char *argv[])
{
    init_shell();
    kprintf("Toddler shell started!\n");
    test_out();
    kapi_process_started(0);
    
    welcome();
    prompt();
    
    // Block here
    do {
        syscall_yield();
    } while (1);
    
    return 0;
}
