#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <io.h>
#include <fcntl.h>

#define PATH_MAX 256

#define PATH_MAX 256

TCHAR * concat (const TCHAR * s1,  const TCHAR * s2)
{
  TCHAR * answer = (TCHAR *) malloc ((lstrlen (s1) + lstrlen (s2) + 1) * sizeof(TCHAR));
  lstrcpy (answer, s1);
  lstrcat (answer, s2);

  return answer;
}


TCHAR *concatn(const TCHAR * str1,  ...)
{
  TCHAR * arg;
  TCHAR * ret;
  va_list ap;

  va_start (ap, str1);

  if (!str1)
    return NULL;

  ret = _tcsdup (str1);

  while ((arg = va_arg (ap, TCHAR *)) != NULL)
    {
      TCHAR * temp = concat (ret, arg);
      free (ret);
      ret = temp;
    }
  va_end (ap);

  return ret;
}


struct _popen_elt {
  FILE *f;      /* File stream returned */
  HANDLE hp;      /* Handle of associated process */
  struct _popen_elt *next;  /* Next list element */
};

static struct _popen_elt _z = { NULL, 0, &_z };
static struct _popen_elt *_popen_list = &_z;

int get_cmd(const TCHAR * cmd, TCHAR ** new_cmd, TCHAR ** app_name)
{
  TCHAR *env_path;
  TCHAR *p, *q;
  TCHAR pname[PATH_MAX], *fp;
  TCHAR *suffixes[] = { _T(".bat"), _T(".cmd"), _T(".com"), _T(".exe"), NULL };
  TCHAR **s;
  BOOL go_on;

  /* We should look for the application name along the PATH,
     and decide to prepend "%COMSPEC% /c " or not to the command line.
     Do nothing for the moment. */

  /* Another way to do that would be to try CreateProcess first without
     invoking cmd, and look at the error code. If it fails because of
     command not found, try to prepend "cmd /c" to the cmd line.
     */

  /* Look for the application name */
  for (p = (TCHAR *) cmd; *p && isspace(*p); p++);
  if (*p == '"') {
    q = ++p;
    while(*p && *p != '"') p++;
    if (*p != '\0') {
//      fprintf(stderr, "popen: malformed command (\" not terminated)\n");
//      return 1;
    }
  }
  else
    for (q = p; *p && !isspace(*p); p++);
  /* q points to the beginning of appname, p to the last + 1 TCHAR */
  if ((*app_name = (TCHAR *) malloc((p - q + 1) * sizeof(TCHAR)) ) == NULL) {
//    fprintf(stderr, "xpopen: malloc(app_name) failed.\n");
    return 1;
  }
//  _tcsncpy(*app_name, q, p - q );
  lstrcpyn(*app_name, q, p - q + 1 );
  (*app_name)[p - q] = '\0';

  pname[0] = '\0';

  env_path = _tgetenv(_T("PATH"));
  env_path = concat(_T(".;"), env_path);

  /* Looking for appname on the path */
  for (s = suffixes, go_on = TRUE; go_on; *s++) {
    if (SearchPath(env_path,  /* Address of search path */
       *app_name,  /* Address of filename */
       *s,    /* Address of extension */
       PATH_MAX,  /* Size of destination buffer */
       pname, /* Address of destination buffer */
       &fp)   /* File part of app_name */
      != 0) {
      *new_cmd = _tcsdup(cmd);
      free(*app_name);
      *app_name = _tcsdup(pname);
      break;
    }
    go_on = (*s != NULL);
  }

  if (go_on == FALSE) {
    /* the app_name was not found */
    *new_cmd = concatn(_tgetenv(_T("COMSPEC")), " /c ", cmd, NULL);
    free(*app_name);
    *app_name = NULL;
  }
  else {
  }
  if (env_path) free(env_path);
  return 0;
}

CString argpath();



FILE *popen(const TCHAR * cmd, const TCHAR * mode)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  FILE *f = NULL;
  int fno, i;
  HANDLE child_in = 0, child_out = 0;
  HANDLE father_in, father_out;
  HANDLE father_in_dup, father_out_dup;
  HANDLE current_pid;
  int binary_mode;
  TCHAR *new_cmd, *app_name = NULL;
  struct _popen_elt *new_process;

  if (get_cmd(cmd, &new_cmd, &app_name)) return NULL;

  current_pid = GetCurrentProcess();
  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;

  if (_tcschr(mode, 'b'))
    binary_mode = O_BINARY;
  else
    binary_mode = O_TEXT;

  /* Opening the pipe for writing */
  if (_tcschr(mode, 'w')) {
    binary_mode |= O_WRONLY;
    if (CreatePipe(&child_in, &father_out, &sa, 0) == FALSE) {
      return NULL;
    }
    si.hStdInput = child_in;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    if (DuplicateHandle(current_pid, father_out,
      current_pid, &father_out_dup,
      0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) {
      return NULL;
    }
    CloseHandle(father_out);
    fno = _open_osfhandle((long)father_out_dup, binary_mode);
    f = _tfdopen(fno, (TCHAR *) mode);
    i = setvbuf( f, NULL, _IONBF, 0 );
  }
  /* Opening the pipe for reading */
  else if (_tcschr(mode, 'r')) {
    binary_mode |= O_RDONLY;
    if (CreatePipe(&father_in, &child_out, &sa, 0) == FALSE) {
      return NULL;
    }
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = child_out;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (DuplicateHandle(current_pid, father_in,
      current_pid, &father_in_dup,
      0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) {
      return NULL;
    }
    CloseHandle(father_in);
    fno = _open_osfhandle((long)father_in_dup, binary_mode);
    f = _tfdopen(fno, (TCHAR *) mode);
    i = setvbuf( f, NULL, _IONBF, 0 );
  }
  else {
    return NULL;
  }

  /* creating child process */
  if (CreateProcess(app_name, /* pointer to name of executable module */
        new_cmd,  /* pointer to command line string */
        NULL, /* pointer to process security attributes */
        NULL, /* pointer to thread security attributes */
        TRUE, /* handle inheritance flag */
        0,    /* creation flags, do not touch this again ! (16/06/98) */
        NULL, /* pointer to environment */
        argpath(), /* pointer to current directory */
        &si,  /* pointer to STARTUPINFO */
        &pi   /* pointer to PROCESS_INFORMATION */
      ) == FALSE) {
    return NULL;
  }

   /* Only the process handle is needed */
  if (CloseHandle(pi.hThread) == FALSE) {
    return NULL;
  }

  if (new_cmd) free(new_cmd);
  if (app_name) free(app_name);

  /* Add the pair (f, pi.hProcess) to the list */
  if ((new_process = (struct _popen_elt *) malloc(sizeof(struct _popen_elt))) == NULL) {
    return NULL;
  }
  /* Saving the FILE * pointer, access key for retrieving the process
     handle later on */
  new_process->f = f;
  /* Closing the unnecessary part of the pipe */
  if (_tcschr(mode, 'r')) {
    CloseHandle(child_out);
  }
  else if (_tcschr(mode, 'w')) {
    CloseHandle(child_in);
  }
  /* Saving the process handle */
  new_process->hp = pi.hProcess;
  /* Linking it to the list of popen() processes */
  new_process->next = _popen_list;
  _popen_list = new_process;

  return f;

}

int pclose(FILE * f)
{
  struct _popen_elt *p, *q;
  int exit_code;

  /* Look for f is the access key in the linked list */
  for (q = NULL, p = _popen_list;
       p != &_z && p->f != f;
       q = p, p = p->next);

  if (p == &_z) {
//    fprintf(stderr, "pclose: error, file not found.");
    return -1;
  }

  /* Closing the FILE pointer */
//  fclose(f);

  if (WaitForSingleObject(p->hp, 20) != WAIT_OBJECT_0) {
    TerminateProcess( p->hp, 0);
  }

  /* Waiting for the process to terminate */
  if (WaitForSingleObject(p->hp, INFINITE) != WAIT_OBJECT_0) {
//  if (WaitForSingleObject(p->hp, 20) != WAIT_OBJECT_0) {
//    fprintf(stderr, "pclose: error, process still active\n");
    return -1;
  }

  /* retrieving the exit code */
  if (GetExitCodeProcess(p->hp, (unsigned long *)&exit_code) == 0) {
//    fprintf(stderr, "pclose: can't get process exit code\n");
    return -1;
  }

  /* Closing the process handle, this will cause the system to
     remove the process from memory */
  if (CloseHandle(p->hp) == FALSE) {
//    fprintf(stderr, "pclose: error closing process handle\n");
    return -1;
  }

  /* remove the elt from the list */
  if (q != NULL)
    q->next = p->next;
  else
    _popen_list = p->next;
  free(p);

  /* Closing the FILE pointer */
  fclose(f);

  return exit_code;
}

#if 0

HANDLE exec_con(const TCHAR * cmd, FILE **inf, FILE **outf, FILE **errf)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  int fno, i;
  HANDLE /*h_in_dup,*/ h_out_dup/*, h_err_dup*/;
  HANDLE /*inH, */outH/*, errH*/;
  HANDLE /*ChildIn,*/ ChildOut/*, ChildErr*/;
  HANDLE current_pid;
  int binary_mode;
  TCHAR *new_cmd, *app_name = NULL;
//  struct _popen_elt *new_process;

  if (get_cmd(cmd, &new_cmd, &app_name)) return 0;

  current_pid = GetCurrentProcess();
  ZeroMemory( &si, sizeof(STARTUPINFO) );
  si.cb = sizeof(STARTUPINFO);
  si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_HIDE;

  binary_mode = O_TEXT;

//  if (!CreatePipe(&ChildIn,&inH,&sa,0)) return 0;
  if (!CreatePipe(&outH,&ChildOut,&sa,0)) return 0;
//  if (!CreatePipe(&errH,&ChildErr,&sa,0)) return 0;

//  si.hStdInput = ChildIn;
  si.hStdOutput= ChildOut;
//  si.hStdError = ChildErr;

//  if (DuplicateHandle(current_pid, inH,  current_pid, &h_in_dup,  0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) return 0;
  if (DuplicateHandle(current_pid, outH, current_pid, &h_out_dup, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) return 0;
//  if (DuplicateHandle(current_pid, errH, current_pid, &h_err_dup, 0, FALSE, DUPLICATE_SAME_ACCESS) == FALSE) return 0;

//  CloseHandle(inH);
  CloseHandle(outH);
//  CloseHandle(errH);

//  fno = _open_osfhandle((long)h_in_dup, binary_mode|O_WRONLY);
//  *inf = _fdopen(fno, (TCHAR *) "w");
//  i = setvbuf( *inf, NULL, _IONBF, 0 );

  fno = _open_osfhandle((long)h_out_dup, binary_mode|O_RDONLY);
  *outf = _tfdopen(fno, (TCHAR *) "r");
  i = setvbuf( *outf, buffer, _IONBF, 128 );

//  fno = _open_osfhandle((long)h_err_dup, binary_mode|O_RDONLY);
//  *errf = _fdopen(fno, (TCHAR *) "r");
//  i = setvbuf( *errf, NULL, _IONBF, 0 );


  /* creating child process */
  if (CreateProcess(app_name, /* pointer to name of executable module */
        new_cmd,  /* pointer to command line string */
        NULL, /* pointer to process security attributes */
        NULL, /* pointer to thread security attributes */
        TRUE, /* handle inheritance flag */
        0,    /* creation flags, do not touch this again ! (16/06/98) */
        NULL, /* pointer to environment */
        NULL, /* pointer to current directory */
        &si,  /* pointer to STARTUPINFO */
        &pi   /* pointer to PROCESS_INFORMATION */
      ) == FALSE) {
    return 0;
  }

   /* Only the process handle is needed */
  if (CloseHandle(pi.hThread) == FALSE) {
    return 0;
  }

  if (new_cmd) free(new_cmd);
  if (app_name) free(app_name);

  return pi.hProcess;
}

#endif